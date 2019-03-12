/*!
    \file itch_handler.cpp
    \brief NASDAQ ITCH handler implementation
    \author Ivan Shynkarenka
    \date 21.07.2017
    \copyright MIT License
*/

#include "trader/providers/nasdaq/itch_handler.h"

#include <cassert>

namespace TradingPlatform {
namespace ITCH {

bool ITCHHandler::Process(void* buffer, size_t size)
{
    size_t index = 0;
    uint8_t* data = (uint8_t*)buffer;

    while (index < size)
    {
        if (_size == 0)
        {
            size_t remaining = size - index;

            // Collect message size into the cache
            if (((_cache.size() == 0) && (remaining < 3)) || (_cache.size() == 1))
            {
                _cache.push_back(data[index++]);
                continue;
            }

            // Read a new message size
            uint16_t message_size;
            if (_cache.empty())
            {
                // Read the message size directly from the input buffer
                index += CppCommon::Endian::ReadBigEndian(&data[index], message_size);
            }
            else
            {
                // Read the message size from the cache
                CppCommon::Endian::ReadBigEndian(_cache.data(), message_size);

                // Clear the cache
                _cache.clear();
            }
            _size = message_size;
        }

        // Read a new message
        if (_size > 0)
        {
            size_t remaining = size - index;

            // Complete or place the message into the cache
            if (!_cache.empty())
            {
                size_t tail = _size - _cache.size();
                if (tail > remaining)
                    tail = remaining;
                _cache.insert(_cache.end(), &data[index], &data[index + tail]);
                index += tail;
                if (_cache.size() < _size)
                    continue;
            }
            else if (_size > remaining)
            {
                _cache.reserve(_size);
                _cache.insert(_cache.end(), &data[index], &data[index + remaining]);
                index += remaining;
                continue;
            }

            // Process the current message
            if (_cache.empty())
            {
                // Process the current message size directly from the input buffer
                if (!ProcessMessage(&data[index], _size))
                    return false;
                index += _size;
            }
            else
            {
                // Process the current message size directly from the cache
                if (!ProcessMessage(_cache.data(), _size))
                    return false;

                // Clear the cache
                _cache.clear();
            }

            // Process the next message
            _size = 0;
        }
    }

    return true;
}

bool ITCHHandler::ProcessMessage(void* buffer, size_t size)
{
    // Message is empty
    if (size == 0)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    switch (*data)
    {
        case 'S':
            return ProcessSystemEventMessage(data, size);
        case 'R':
            return ProcessStockDirectoryMessage(data, size);
        case 'H':
            return ProcessStockTradingActionMessage(data, size);
        case 'Y':
            return ProcessRegSHOMessage(data, size);
        case 'L':
            return ProcessMarketParticipantPositionMessage(data, size);
        case 'V':
            return ProcessMWCBDeclineMessage(data, size);
        case 'W':
            return ProcessMWCBStatusMessage(data, size);
        case 'K':
            return ProcessIPOQuotingMessage(data, size);
        case 'A':
            return ProcessAddOrderMessage(data, size);
        case 'F':
            return ProcessAddOrderMPIDMessage(data, size);
        case 'E':
            return ProcessOrderExecutedMessage(data, size);
        case 'C':
            return ProcessOrderExecutedWithPriceMessage(data, size);
        case 'X':
            return ProcessOrderCancelMessage(data, size);
        case 'D':
            return ProcessOrderDeleteMessage(data, size);
        case 'U':
            return ProcessOrderReplaceMessage(data, size);
        case 'P':
            return ProcessTradeMessage(data, size);
        case 'Q':
            return ProcessCrossTradeMessage(data, size);
        case 'B':
            return ProcessBrokenTradeMessage(data, size);
        case 'I':
            return ProcessNOIIMessage(data, size);
        case 'N':
            return ProcessRPIIMessage(data, size);
        default:
            return ProcessUnknownMessage(data, size);
    }
}

void ITCHHandler::Reset()
{
    _size = 0;
    _cache.clear();
}

bool ITCHHandler::ProcessSystemEventMessage(void* buffer, size_t size)
{
    SystemEventMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessStockDirectoryMessage(void* buffer, size_t size)
{
    StockDirectoryMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessStockTradingActionMessage(void* buffer, size_t size)
{
    StockTradingActionMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessRegSHOMessage(void* buffer, size_t size)
{
    RegSHOMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessMarketParticipantPositionMessage(void* buffer, size_t size)
{
    MarketParticipantPositionMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessMWCBDeclineMessage(void* buffer, size_t size)
{
    MWCBDeclineMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessMWCBStatusMessage(void* buffer, size_t size)
{
    MWCBStatusMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessIPOQuotingMessage(void* buffer, size_t size)
{
    IPOQuotingMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessAddOrderMessage(void* buffer, size_t size)
{
    AddOrderMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessAddOrderMPIDMessage(void* buffer, size_t size)
{
    AddOrderMPIDMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessOrderExecutedMessage(void* buffer, size_t size)
{
    OrderExecutedMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessOrderExecutedWithPriceMessage(void* buffer, size_t size)
{
    OrderExecutedWithPriceMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessOrderCancelMessage(void* buffer, size_t size)
{
    OrderCancelMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessOrderDeleteMessage(void* buffer, size_t size)
{
    OrderDeleteMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessOrderReplaceMessage(void* buffer, size_t size)
{
    OrderReplaceMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessTradeMessage(void* buffer, size_t size)
{
    TradeMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessCrossTradeMessage(void* buffer, size_t size)
{
    CrossTradeMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessBrokenTradeMessage(void* buffer, size_t size)
{
    BrokenTradeMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessNOIIMessage(void* buffer, size_t size)
{
    NOIIMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessRPIIMessage(void* buffer, size_t size)
{
    RPIIMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool ITCHHandler::ProcessUnknownMessage(void* buffer, size_t size)
{
    UnknownMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

} // namespace ITCH
} // namespace TradingPlatform
