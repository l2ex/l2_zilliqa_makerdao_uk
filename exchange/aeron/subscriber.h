#ifndef TRADING_PLATFORM_AERON_ITCH_SUBSCRIBER_H
#define TRADING_PLATFORM_AERON_ITCH_SUBSCRIBER_H

#include <iostream>

#include "system/stream.h"

#include "Aeron.h"
#include "concurrent/NoOpIdleStrategy.h"
#include "util/Exceptions.h"

#include "command_option_parser.h"
#include "configuration.h"

namespace TradingPlatform {
namespace Aeron {


struct SubscriberSettings
{
    std::string directory;
    std::string channel = DEFAULT_CHANNEL;
    std::int32_t streamId = DEFAULT_STREAM_ID;
    int fragments = DEFAULT_FRAGMENT_COUNT_LIMIT;
    bool invalid = true;
};


class Subscriber
{
public:

using DataHandler = std::function<void(
    const aeron::AtomicBuffer &buffer,
    aeron::index_t offset,
    aeron::index_t length,
    const aeron::Header &header
)>;

using EndOfStreamHandler = std::function<void(
    aeron::Image &image
)>;

public:
    Subscriber(const SubscriberSettings &settings)
        : _settings(settings)
    {
        try
        {
            if (!_settings.directory.empty())
                _context.aeronDir(_settings.directory);

            _context.newSubscriptionHandler(
                [](const std::string &channel, std::int32_t streamId, std::int64_t correlationId) {
                    std::cout << "Subscription: " << channel << " " << correlationId << ":" << streamId << std::endl;
                }
            );
            _context.availableImageHandler(
                [](aeron::Image &image) {
                    std::cout << "Available image correlationId=" << image.correlationId() << " sessionId=" << image.sessionId();
                    std::cout << " at position=" << image.position() << " from " << image.sourceIdentity() << std::endl;
                }
            );
            _context.unavailableImageHandler(
                [](aeron::Image &image) {
                    std::cout << "Unavailable image on correlationId=" << image.correlationId() << " sessionId=" << image.sessionId();
                    std::cout << " at position=" << image.position() << " from " << image.sourceIdentity() << std::endl;
                }
            );

            _aeron = aeron::Aeron::connect(_context);

            // Add the subscription to start the process
            std::int64_t id = _aeron->addSubscription(_settings.channel, _settings.streamId);

            // Wait for the subscription to be valid
            while (!_subscription)
            {
                _subscription = _aeron->findSubscription(id);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::this_thread::yield();
            }

            const std::int64_t channelStatus = _subscription->channelStatus();

            std::cout << "Subscription channel status (id=" << _subscription->channelStatusId() << ") "
                      << ((channelStatus == aeron::ChannelEndpointStatus::CHANNEL_ENDPOINT_ACTIVE) ? "ACTIVE" : std::to_string(channelStatus))
                      << std::endl;
        }
        catch (const aeron::SourcedException &e)
        {
            std::cerr << "FAILED: " << e.what() << " : " << e.where() << std::endl;
            _failed = true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "FAILED: " << e.what() << " : " << std::endl;
            _failed = true;
        }
    }

    bool isFailed() { return _failed; }

    void setDataHandler(DataHandler handler)
    {
        _handlerData = handler;
    }

    void setEndOfStreamHandler(EndOfStreamHandler handler)
    {
        _handlerEndOfStream = handler;
    }

    void start()
    {
        stop();
        wait();
        _running = true;
        _thread = std::make_unique<Thread>(&Subscriber::loop, this);
    }

    void stop()
    {
        _running = false;
    }

    void wait()
    {
        if (_thread && _thread->joinable())
            _thread->join();
    }

private:

    void loop()
    {
        bool reachedEndOfStream = false;
        while (_running)
        {
            try
            {
                const int fragmentsRead = _handlerData ? _subscription->poll(_handlerData, _settings.fragments) : 0;
                if (fragmentsRead == 0)
                {
                    if (!reachedEndOfStream && _handlerEndOfStream && _subscription->pollEndOfStreams(_handlerEndOfStream) > 0)
                    {
                        reachedEndOfStream = true;
                    }
                }
                _idleStrategy.idle(fragmentsRead);
            }
            catch (const aeron::SourcedException &e)
            {
                std::cerr << "FAILED: " << e.what() << " : " << e.where() << std::endl;
            }
            catch (const std::exception &e)
            {
                std::cerr << "FAILED: " << e.what() << " : " << std::endl;
            }
        }
    }

private:
    SubscriberSettings _settings;
    aeron::Context _context;
    aeron::NoOpIdleStrategy _idleStrategy;

    std::shared_ptr<aeron::Aeron> _aeron;
    std::shared_ptr<aeron::Subscription> _subscription;

    std::unique_ptr<Thread> _thread;
    std::atomic<bool> _running;

    DataHandler _handlerData;
    EndOfStreamHandler _handlerEndOfStream;

    bool _failed = false;
};


}}

#endif // TRADING_PLATFORM_AERON_ITCH_SUBSCRIBER_H