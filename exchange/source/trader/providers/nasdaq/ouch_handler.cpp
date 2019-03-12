/*!
    \file ouch_handler.cpp
    \brief NASDAQ OUCH handler implementation
    \author Igor Sokolov
    \date 12.03.2019
    \copyright MIT License
*/

#include "trader/providers/nasdaq/ouch_handler.h"

#include <cassert>

namespace TradingPlatform {
namespace OUCH {

bool OUCHHandler::Process(void* buffer, size_t size)
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

bool OUCHHandler::ProcessMessage(void* buffer, size_t size)
{
    // Message is empty
    if (size == 0)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    switch (*data)
    {
        case 'O':
            return ProcessEnterOrderMessage(data, size);
        case 'U':
            return ProcessReplaceOrderMessage(data, size);
        case 'X':
            return ProcessCancelOrderMessage(data, size);
        default:
            return ProcessUnknownMessage(data, size);
    }
}

void OUCHHandler::Reset()
{
    _size = 0;
    _cache.clear();
}

bool OUCHHandler::ProcessEnterOrderMessage(void* buffer, size_t size)
{
    EnterOrderMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool OUCHHandler::ProcessReplaceOrderMessage(void* buffer, size_t size)
{
    ReplaceOrderMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool OUCHHandler::ProcessCancelOrderMessage(void* buffer, size_t size)
{
    CancelOrderMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

bool OUCHHandler::ProcessUnknownMessage(void* buffer, size_t size)
{
    UnknownMessage message;
    if (!message.deserialize(buffer, size))
        return false;
    return onMessage(message);
}

} // namespace OUCH
} // namespace TradingPlatform
