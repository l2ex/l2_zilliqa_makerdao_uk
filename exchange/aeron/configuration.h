#ifndef TRADING_PLATFORM_AERON_CONFIGURATION_H
#define TRADING_PLATFORM_AERON_CONFIGURATION_H

#include <string>

namespace TradingPlatform {
namespace Aeron {

const static std::string DEFAULT_CHANNEL = "aeron:udp?endpoint=localhost:40123";
const static std::int32_t DEFAULT_STREAM_ID = 10;
const static std::size_t DEFAULT_PUBLISHER_BUFFER_SIZE = 128 * 1024 * 1024;
const static std::size_t DEFAULT_PUBLISHER_MESSAGE_SIZE = 256 * 1024;
const static int DEFAULT_FRAGMENT_COUNT_LIMIT = 1024;

}}

#endif // TRADING_PLATFORM_AERON_CONFIGURATION_H
