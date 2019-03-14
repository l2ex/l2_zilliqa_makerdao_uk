#include <csignal>

#include "command_option_parser.h"
#include "publisher.h"
#include "subscriber.h"

#include "trader/l2ex/itch_handler.h"
#include "trader/l2ex/ouch_handler.h"
#include "trader/l2ex/market_handler.h"

using namespace TradingPlatform;
using namespace TradingPlatform::Aeron;
using namespace TradingPlatform::L2ex;

static std::unique_ptr<Publisher> itchPublisher;
static std::unique_ptr<Publisher> ouchPublisher;
static std::unique_ptr<Subscriber> ouchSubscriber;

void handleSigInt(int)
{
    if (itchPublisher)
        itchPublisher->stop();
    if (ouchPublisher)
        ouchPublisher->stop();
    if (ouchSubscriber)
        ouchSubscriber->stop();
}

// Forward declaration
bool prepareMarketManager(Matching::MarketManager *market);
PublisherSettings parsePublisherSettingsForITCH(int argc, char **argv);
PublisherSettings parsePublisherSettingsForOUCH(int argc, char **argv);
SubscriberSettings parseSubscriberSettingsForOUCH(int argc, char **argv);

int main(int argc, char **argv)
{
    std::signal(SIGINT, handleSigInt);

    // Parse settings

    auto itchPublisherSettings = parsePublisherSettingsForITCH(argc, argv);
    auto ouchPublisherSettings = parsePublisherSettingsForOUCH(argc, argv);
    auto ouchSubscriberSettings = parseSubscriberSettingsForOUCH(argc, argv);
    if (itchPublisherSettings.invalid || ouchPublisherSettings.invalid || ouchSubscriberSettings.invalid)
        return -1;

    std::cout << "Publishing ITCH to channel " << itchPublisherSettings.channel << " on stream " << itchPublisherSettings.streamId << std::endl;
    std::cout << "Publishing OUCH to channel " << ouchPublisherSettings.channel << " on stream " << ouchPublisherSettings.streamId << std::endl;
    std::cout << "Subscribing OUCH to channel " << ouchSubscriberSettings.channel << " on stream " << ouchSubscriberSettings.streamId << std::endl;

    // Create and start ITCH publisher

    itchPublisher = std::make_unique<Publisher>(itchPublisherSettings);
    if (!itchPublisher || itchPublisher->isFailed())
        return -1;

    itchPublisher->start();

    // Create and start OUCH publisher

    ouchPublisher = std::make_unique<Publisher>(ouchPublisherSettings);
    if (!ouchPublisher || ouchPublisher->isFailed())
        return -1;

    ouchPublisher->start();

    // Create handlers and market manager

    auto marketHandler = std::make_shared<MarketHandler>(nullptr, nullptr);
    auto market = std::make_shared<Matching::MarketManager>(*marketHandler);
    auto itchHandler = std::make_shared<ITCHHandler>(*market, &(*itchPublisher));
    auto ouchHandler = std::make_shared<OUCHHandler>(*market, &(*ouchPublisher));
    auto marketPrepared = prepareMarketManager(&(*market));
    if (!marketPrepared)
        return -1;
    market->EnableMatching();

    // Create and start OUCH subscriber
    
    ouchSubscriber = std::make_unique<Subscriber>(ouchSubscriberSettings);
    if (!ouchSubscriber || ouchSubscriber->isFailed())
        return -1;
    
    ouchSubscriber->setDataHandler([ouchHandler](const aeron::AtomicBuffer &buffer, aeron::index_t offset, aeron::index_t length, const aeron::Header &header)
    {
        if (length == 0)
            return;

        // Process the buffer
        bool processed = ouchHandler->Process(buffer.buffer() + offset, length);

        // Print some logs
        std::cout << "Handled message on stream " << header.streamId()
            << " in session " << header.sessionId()
            << " [" << offset << ":" << offset + length << "]"
            << (processed ? " and processed successfully" : " and is not processed")
            << std::endl;
    });

    ouchSubscriber->setEndOfStreamHandler([](aeron::Image &image)
    {
        std::cout << "Handled end of stream in session  " << image.sessionId()
            << " with correlation " << image.correlationId()
            << " from " << image.sourceIdentity()
            << std::endl;
    });

    ouchSubscriber->start();

    // Block main thread until all publishers and subscribers are stopped

    itchPublisher->wait();
    ouchPublisher->wait();
    ouchSubscriber->wait();

    return 0;
}

bool prepareMarketManager(Matching::MarketManager *market)
{
    auto reg = [&](uint32_t id, const char name[8])
    {
        Matching::ErrorCode error;
        auto symbol = Matching::Symbol(id, name);

        error = market->AddSymbol(symbol);
        if (error != Matching::ErrorCode::OK)
            return false;
        
        error = market->AddOrderBook(symbol);
        if (error != Matching::ErrorCode::OK)
            return false;

        return true;
    };

    bool result = true;

    result = result && reg(1, "BYN_RUB");
    result = result && reg(2, "CHF_RUB");
    result = result && reg(3, "CNY_RUB");
    result = result && reg(4, "EUR_RUB");
    result = result && reg(5, "EUR_RUB1");
    result = result && reg(6, "EUR_RUB2");
    result = result && reg(7, "EUR_USD");
    result = result && reg(8, "GBP_RUB");
    result = result && reg(9, "GLD_RUB");
    result = result && reg(10, "HKD_RUB");
    result = result && reg(11, "SLV_RUB");
    result = result && reg(12, "USD_EUR");
    result = result && reg(13, "USD_RUB");
    result = result && reg(14, "USD_RUB1");
    result = result && reg(777, "ZIL_DAI");
    
    return result;
}

PublisherSettings parsePublisherSettingsForITCH(int argc, char **argv)
{
    PublisherSettings settings;

    try
    {
        CommandOptionParser parser;

        // Prepare command options parser
        parser.addOption(CommandOption("itch.publisher.dir",     1, 1, "Directory used by Aeron driver."));
        parser.addOption(CommandOption("itch.publisher.channel", 1, 1, "Channel endpoint to connect to."));
        parser.addOption(CommandOption("itch.publisher.stream",  1, 1, "Stream ID as number."));
        parser.addOption(CommandOption("itch.publisher.buffer",  1, 1, "Size of buffer used to store cached data before sending to Aeron driver (in bytes)."));
        parser.addOption(CommandOption("itch.publisher.message", 1, 1, "Maximal size of message allowed to send to Aeron driver (in bytes)."));

        // Parse command arguments
        parser.parse(argc, argv);

        // Use specified options
        settings.directory = parser.getOption("itch.publisher.dir").getParam(0, settings.directory);
        settings.channel = parser.getOption("itch.publisher.channel").getParam(0, settings.channel);
        settings.streamId = parser.getOption("itch.publisher.stream").getParamAsInt(0, 1, INT32_MAX, settings.streamId);
        settings.bufferSize = static_cast<size_t>(parser.getOption("itch.publisher.buffer").getParamAsInt(0, 1024, INT32_MAX, static_cast<int>(settings.bufferSize)));
        settings.messageSize = static_cast<size_t>(parser.getOption("itch.publisher.message").getParamAsInt(0, 128, INT32_MAX, static_cast<int>(settings.messageSize)));
        settings.invalid = false;
    }
    catch (const aeron::util::SourcedException &e)
    {
        std::cerr << "[ERROR] " << e.what() << std::endl << std::endl;
    }

    return settings;
}

PublisherSettings parsePublisherSettingsForOUCH(int argc, char **argv)
{
    PublisherSettings settings;

    try
    {
        CommandOptionParser parser;

        // Prepare command options parser
        parser.addOption(CommandOption("ouch.publisher.dir",     1, 1, "Directory used by Aeron driver."));
        parser.addOption(CommandOption("ouch.publisher.channel", 1, 1, "Channel endpoint to connect to."));
        parser.addOption(CommandOption("ouch.publisher.stream",  1, 1, "Stream ID as number."));
        parser.addOption(CommandOption("ouch.publisher.buffer",  1, 1, "Size of buffer used to store cached data before sending to Aeron driver (in bytes)."));
        parser.addOption(CommandOption("ouch.publisher.message", 1, 1, "Maximal size of message allowed to send to Aeron driver (in bytes)."));

        // Parse command arguments
        parser.parse(argc, argv);

        // Use specified options
        settings.directory = parser.getOption("ouch.publisher.dir").getParam(0, settings.directory);
        settings.channel = parser.getOption("ouch.publisher.channel").getParam(0, settings.channel);
        settings.streamId = parser.getOption("ouch.publisher.stream").getParamAsInt(0, 1, INT32_MAX, settings.streamId);
        settings.bufferSize = static_cast<size_t>(parser.getOption("ouch.publisher.buffer").getParamAsInt(0, 1024, INT32_MAX, static_cast<int>(settings.bufferSize)));
        settings.messageSize = static_cast<size_t>(parser.getOption("ouch.publisher.message").getParamAsInt(0, 128, INT32_MAX, static_cast<int>(settings.messageSize)));
        settings.invalid = false;
    }
    catch (const aeron::util::SourcedException &e)
    {
        std::cerr << "[ERROR] " << e.what() << std::endl << std::endl;
    }

    return settings;
}

SubscriberSettings parseSubscriberSettingsForOUCH(int argc, char **argv)
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
