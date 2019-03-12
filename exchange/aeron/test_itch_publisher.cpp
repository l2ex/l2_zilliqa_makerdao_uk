#include <csignal>

#include "command_option_parser.h"
#include "publisher.h"
#include "subscriber.h"

#include "benchmark/reporter_console.h"
#include "time/timestamp.h"

#include "trader/l2ex/itch_handler.h"
#include "trader/l2ex/ouch_handler.h"
#include "trader/l2ex/market_handler.h"

using namespace TradingPlatform;
using namespace TradingPlatform::Aeron;
using namespace TradingPlatform::L2ex;

static std::unique_ptr<Publisher> publisher;

void handleSigInt(int)
{
    if (publisher)
        publisher->stop();
}

// Forward declaration
PublisherSettings parsePublisherSettings(int argc, char **argv);

int main(int argc, char **argv)
{
    std::signal(SIGINT, handleSigInt);

    // Parse settings

    auto publisherSettings = parsePublisherSettings(argc, argv);
    if (publisherSettings.invalid)
        return -1;

    std::cout << "Publishing ITCH to channel " << publisherSettings.channel << " on stream " << publisherSettings.streamId << std::endl;

    // Create and start publisher

    publisher = std::make_unique<Publisher>(publisherSettings);
    if (!publisher || publisher->isFailed())
        return -1;

    publisher->start();

    // Perform input
    std::unique_ptr<CppCommon::Reader> input(new CppCommon::StdInput());
    size_t size;
    uint8_t buffer[8192];
    std::cout << "ITCH processing...";
    uint64_t timestamp_start = CppCommon::Timestamp::nano();
    while ((size = input->Read(buffer, sizeof(buffer))) > 0)
    {
        publisher->publish(buffer, size);
    }
    uint64_t timestamp_stop = CppCommon::Timestamp::nano();
    std::cout << "Done! Forwarding time: " << CppBenchmark::ReporterConsole::GenerateTimePeriod(timestamp_stop - timestamp_start) << std::endl;
    publisher->stop();

    // Block main thread until publisher is stopped

    publisher->wait();

    return 0;
}

PublisherSettings parsePublisherSettings(int argc, char **argv)
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
