#include <csignal>

#include "command_option_parser.h"
#include "publisher.h"
#include "subscriber.h"

#include "trader/l2ex/ouch_handler.h"
#include "trader/l2ex/market_handler.h"

using namespace TradingPlatform;
using namespace TradingPlatform::Aeron;
using namespace TradingPlatform::L2ex;

static std::unique_ptr<Subscriber> subscriber;

void handleSigInt(int)
{
    if (subscriber)
        subscriber->stop();
}

// Forward declaration
SubscriberSettings parseSubscriberSettings(int argc, char **argv);

int main(int argc, char **argv)
{
    std::signal(SIGINT, handleSigInt);

    // Parse settings

    auto subscriberSettings = parseSubscriberSettings(argc, argv);
    if (subscriberSettings.invalid)
        return -1;

    std::cout << "Subscribing OUCH to channel " << subscriberSettings.channel << " on stream " << subscriberSettings.streamId << std::endl;

    // Create and start OUCH subscriber
    
    subscriber = std::make_unique<Subscriber>(subscriberSettings);
    if (!subscriber || subscriber->isFailed())
        return -1;
    
    subscriber->setDataHandler([](const aeron::AtomicBuffer &buffer, aeron::index_t offset, aeron::index_t length, const aeron::Header &header)
    {
        if (length == 0)
            return;

        uint8_t* data = (uint8_t*)(buffer.buffer() + offset);

        switch (*data)
        {
            case 'S':
            {
                OUCH::SystemEventMessage message;
                auto deserialized = message.deserialize(data, length);
                if (deserialized)
                    std::cout << "[OUCH] Message received: " << message << std::endl;
                break;
            }
            case 'A':
            {
                OUCH::OrderAcceptedMessage message;
                auto deserialized = message.deserialize(data, length);
                if (deserialized)
                    std::cout << "[OUCH] Message received: " << message << std::endl;
                break;
            }
            case 'J':
            {
                OUCH::OrderRejectedMessage message;
                auto deserialized = message.deserialize(data, length);
                if (deserialized)
                    std::cout << "[OUCH] Message received: " << message << std::endl;
                break;
            }
            case 'U':
            {
                OUCH::OrderReplacedMessage message;
                auto deserialized = message.deserialize(data, length);
                if (deserialized)
                    std::cout << "[OUCH] Message received: " << message << std::endl;
                break;
            }
            case 'C':
            {
                OUCH::OrderCanceledMessage message;
                auto deserialized = message.deserialize(data, length);
                if (deserialized)
                    std::cout << "[OUCH] Message received: " << message << std::endl;
                break;
            }
            case 'E':
            {
                OUCH::OrderExecutedMessage message;
                auto deserialized = message.deserialize(data, length);
                if (deserialized)
                    std::cout << "[OUCH] Message received: " << message << std::endl;
                break;
            }
            case 'B':
            {
                OUCH::BrokenTradeMessage message;
                auto deserialized = message.deserialize(data, length);
                if (deserialized)
                    std::cout << "[OUCH] Message received: " << message << std::endl;
                break;
            }
        }
    });

    subscriber->setEndOfStreamHandler([](aeron::Image &image)
    {
        std::cout << "Handled end of stream in session  " << image.sessionId()
            << " with correlation " << image.correlationId()
            << " from " << image.sourceIdentity()
            << std::endl;
    });

    subscriber->start();

    // Block main thread until all publishers and subscribers are stopped

    subscriber->wait();

    return 0;
}

SubscriberSettings parseSubscriberSettings(int argc, char **argv)
{
    SubscriberSettings settings;

    try
    {
        CommandOptionParser parser;

        // Prepare command options parser
        parser.addOption(CommandOption("ouch.subscriber.dir",       1, 1, "Directory used by Aeron driver."));
        parser.addOption(CommandOption("ouch.subscriber.channel",   1, 1, "Channel endpoint to connect to."));
        parser.addOption(CommandOption("ouch.subscriber.stream",    1, 1, "Stream ID as number."));
        parser.addOption(CommandOption("ouch.subscriber.fragments", 1, 1, "Fragment count limit."));

        // Parse command arguments
        parser.parse(argc, argv);

        // Use specified options
        settings.directory = parser.getOption("ouch.subscriber.dir").getParam(0, settings.directory);
        settings.channel = parser.getOption("ouch.subscriber.channel").getParam(0, settings.channel);
        settings.streamId = parser.getOption("ouch.subscriber.stream").getParamAsInt(0, 1, INT32_MAX, settings.streamId);
        settings.fragments = static_cast<size_t>(parser.getOption("ouch.subscriber.fragments").getParamAsInt(0, 1, INT32_MAX, settings.fragments));
        settings.invalid = false;
    }
    catch (const aeron::util::SourcedException &e)
    {
        std::cerr << "[ERROR] " << e.what() << std::endl << std::endl;
    }

    return settings;
}
