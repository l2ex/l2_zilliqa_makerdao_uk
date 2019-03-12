#ifndef TRADING_PLATFORM_AERON_ITCH_PUBLISHER_H
#define TRADING_PLATFORM_AERON_ITCH_PUBLISHER_H

#include <iostream>

#include "system/stream.h"

#include "Aeron.h"
#include "concurrent/NoOpIdleStrategy.h"
#include "util/Exceptions.h"

#include "circular_buffer.h"
#include "configuration.h"
#include "thread.h"

namespace TradingPlatform {
namespace Aeron {


struct PublisherSettings
{
    std::string directory;
    std::string channel = DEFAULT_CHANNEL;
    std::int32_t streamId = DEFAULT_STREAM_ID;
    std::size_t bufferSize = DEFAULT_PUBLISHER_BUFFER_SIZE;
    std::size_t messageSize = DEFAULT_PUBLISHER_MESSAGE_SIZE;
    bool invalid = true;
};


class Publisher
{
public:
    Publisher(const PublisherSettings &settings)
        : _settings(settings)
    {
        try
        {
            if (!_settings.directory.empty())
                _context.aeronDir(_settings.directory);
                
            _context.newPublicationHandler(
                [](const std::string &channel, std::int32_t streamId, std::int32_t sessionId, std::int64_t correlationId) {
                    std::cout << "Publication: " << channel << " " << correlationId << ":" << streamId << ":" << sessionId << std::endl;
                }
            );

            _aeron = aeron::Aeron::connect(_context);

            // Add the publication to start the process
            std::int64_t id = _aeron->addPublication(_settings.channel, _settings.streamId);

            // Wait for the publication to be valid
            while (!_publication)
            {
                _publication = _aeron->findPublication(id);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::this_thread::yield();
            }

            const std::int64_t channelStatus = _publication->channelStatus();

            std::cout << "Publication channel status (id=" << _publication->channelStatusId() << ") "
                      << ((channelStatus == aeron::ChannelEndpointStatus::CHANNEL_ENDPOINT_ACTIVE) ? "ACTIVE" : std::to_string(channelStatus))
                      << std::endl;

            _bufferMessage.resize(_settings.messageSize);
            _bufferMessageAtomic.reset(new aeron::AtomicBuffer(&_bufferMessage[0], _bufferMessage.size()));
            _bufferCircular = CircularBufferCreate(_settings.bufferSize);

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

    virtual ~Publisher()
    {
        if (_bufferCircular)
            CircularBufferFree(_bufferCircular);
    }

    bool isFailed() { return _failed; }

    void start()
    {
        stop();
        wait();
        _running = true;
        _thread = std::make_unique<Thread>(&Publisher::loop, this);
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

    void publish(void *data, size_t size)
    {
        while (_running)
        {
            try
            {
                {
                    std::lock_guard<std::mutex> locker(_mutex);
                    auto bufferCapacity = CircularBufferGetCapacity(_bufferCircular);
                    auto bufferUsedBytes = CircularBufferGetDataSize(_bufferCircular);
                    auto bufferFreeBytes = bufferCapacity - bufferUsedBytes;
                    if (bufferFreeBytes >= size)
                    {
                        CircularBufferPush(_bufferCircular, reinterpret_cast<std::uint8_t *>(data), size);
                        break;
                    }
                }
                _idleStrategy.idle(0);
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
    void loop()
    {
        while (_running)
        {
            try
            {
                size_t readingBytes = 0;
                size_t readBytes = 0;
                size_t removedBytes = 0;
                {
                    std::lock_guard<std::mutex> locker(_mutex);
                    readingBytes = CircularBufferGetDataSize(_bufferCircular);
                    if (readingBytes > 0)
                    {
                        readingBytes = std::min(readingBytes, _settings.messageSize);
                        readBytes = CircularBufferRead(_bufferCircular, readingBytes, _bufferMessageAtomic->buffer());
                    }
                }
                if (readingBytes > 0)
                {
                    if (readingBytes != readBytes)
                    {
                        std::cout << "Circular buffer failed during reading " << readingBytes << " bytes" << std::endl;
                        std::this_thread::yield();
                    }
                    else
                    {
                        auto result = _publication->offer(*_bufferMessageAtomic, 0, readingBytes);
                        if (result < 0)
                        {
                            if (result == aeron::BACK_PRESSURED)
                            {
                                //std::cout << "Offer failed due to back pressure" << std::endl;
                            }
                            else if (result == aeron::NOT_CONNECTED)
                            {
                                std::cout << "Offer failed because publisher is not connected to subscriber" << std::endl;
                            }
                            else if (result == aeron::ADMIN_ACTION)
                            {
                                std::cout << "Offer failed because of an administration action in the system" << std::endl;
                            }
                            else if (result == aeron::PUBLICATION_CLOSED)
                            {
                                std::cout << "Offer failed publication is closed" << std::endl;
                            }
                            else
                            {
                                std::cout << "Offer failed due to unknown reason" << result << std::endl;
                            }
                            std::this_thread::yield();
                        }
                        else
                        {
                            std::lock_guard<std::mutex> locker(_mutex);
                            removedBytes = CircularBufferPop(_bufferCircular, readBytes, nullptr);
                            if (removedBytes != readBytes)
                            {
                                std::cout << "Circular buffer failed during popping out " << readBytes << " bytes" << std::endl;
                                std::this_thread::yield();
                            }
                        }
                        if (!_publication->isConnected())
                        {
                            std::cout << "No active subscribers detected" << std::endl;
                            std::this_thread::yield();
                        }
                    }
                }
                else
                {
                    std::this_thread::yield();
                }
                
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
    PublisherSettings _settings;
    aeron::Context _context;
    aeron::NoOpIdleStrategy _idleStrategy;

    std::shared_ptr<aeron::Aeron> _aeron;
    std::shared_ptr<aeron::Publication> _publication;

    CircularBuffer _bufferCircular = nullptr;
    std::vector<std::uint8_t> _bufferMessage;
    std::unique_ptr<aeron::AtomicBuffer> _bufferMessageAtomic;

    std::unique_ptr<Thread> _thread;
    std::mutex _mutex;
    std::atomic<bool> _running;

    bool _failed = false;
};


}}

#endif // TRADING_PLATFORM_AERON_ITCH_PUBLISHER_H