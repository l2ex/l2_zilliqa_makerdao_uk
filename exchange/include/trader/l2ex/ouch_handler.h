#ifndef TRADING_PLATFORM_L2EX_OUCH_HANDLER_H
#define TRADING_PLATFORM_L2EX_OUCH_HANDLER_H

#include "trader/matching/market_manager.h"
#include "trader/providers/nasdaq/ouch_handler.h"
#include "../../../aeron/publisher.h"

#define TRADING_PLATFORM_L2EX_OUCH_HANDLER_PRINT_LOGS 1

namespace TradingPlatform {
namespace L2ex {

class OUCHHandler : public OUCH::OUCHHandler
{
public:

    OUCHHandler(Matching::MarketManager &market, Aeron::Publisher *publisher = nullptr)
        : _market(market)
        , _publisher(publisher)
    {
    }

protected:

    bool onMessage(const OUCH::EnterOrderMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_OUCH_HANDLER_PRINT_LOGS
        std::cout << "[OUTH] Message received: " << message << std::endl;
#endif
        uint64_t orderId = message.OrderToken;
        if (message.Price == 0x7fffffff)
        {
            Matching::Order order = Matching::Order::Market(
                orderId,
                message.OrderbookId,
                message.OrderVerb == 'B' ? Matching::OrderSide::BUY : Matching::OrderSide::SELL,
                message.Shares
            );
            auto error = _market.AddOrder(order);
            if (error != Matching::ErrorCode::OK)
            {
                OUCH::OrderRejectedMessage rejected = {};
                rejected.Type = 'J';
                rejected.Timestamp = nanosecondsSinceMidnight();
                rejected.OrderToken = message.OrderToken;
                rejected.Reason = 'W'; // TODO
                publishMessage(rejected);
                return false;
            }
        }
        else
        {
            Matching::Order order = Matching::Order::Limit(
                orderId,
                message.OrderbookId,
                message.OrderVerb == 'B' ? Matching::OrderSide::BUY : Matching::OrderSide::SELL,
                message.Price,
                message.Shares
            );
            auto error = _market.AddOrder(order);
            if (error != Matching::ErrorCode::OK)
            {
                OUCH::OrderRejectedMessage rejected = {};
                rejected.Type = 'J';
                rejected.Timestamp = nanosecondsSinceMidnight();
                rejected.OrderToken = message.OrderToken;
                rejected.Reason = 'W'; // TODO
                publishMessage(rejected);
                return false;
            }
        }
        return true;
    }

    bool onMessage(const OUCH::ReplaceOrderMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_OUCH_HANDLER_PRINT_LOGS
        std::cout << "[OUTH] Message received: " << message << std::endl;
#endif
        uint64_t existingOrderId = message.ExistingOrderToken;
        uint64_t replacementOrderId = message.ReplacementOrderToken;
        auto error = _market.ReplaceOrder(existingOrderId, replacementOrderId, message.Price, message.Shares);
        if (error != Matching::ErrorCode::OK)
            return false;
        return true;
    }

    bool onMessage(const OUCH::CancelOrderMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_OUCH_HANDLER_PRINT_LOGS
        std::cout << "[OUTH] Message received: " << message << std::endl;
#endif
        uint64_t orderId = message.OrderToken;
        auto error = _market.DeleteOrder(orderId);
        if (error != Matching::ErrorCode::OK)
            return false;
        return true;
    }

    bool onMessage(const OUCH::UnknownMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_OUCH_HANDLER_PRINT_LOGS
        std::cerr << "[OUTH] Unknown message received: " << message << std::endl;
#endif
        return true;
    }

private:

    template <class Message>
    void publishMessage(const Message &message)
    {
        if (_publisher)
        {
            auto length = message.serialize(_messageSerialized + 2, sizeof(_messageSerialized) - 2);
            if (length > 0)
            {
                CppCommon::Endian::WriteBigEndian(_messageSerialized, static_cast<uint16_t>(length));
                _publisher->publish(_messageSerialized, length + 2);
            }
        }
    }

    std::chrono::system_clock::duration durationSinceMidnight() {
        auto now = std::chrono::system_clock::now();
        time_t tnow = std::chrono::system_clock::to_time_t(now);
        tm *date = std::localtime(&tnow);
        date->tm_hour = 0;
        date->tm_min = 0;
        date->tm_sec = 0;
        auto midnight = std::chrono::system_clock::from_time_t(std::mktime(date));
        return now - midnight;
    }

    uint64_t nanosecondsSinceMidnight() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(durationSinceMidnight()).count();
    }

private:

    Matching::MarketManager &_market;
    Aeron::Publisher *_publisher;
    uint8_t _messageSerialized[1024];
};

}}

#endif // TRADING_PLATFORM_L2EX_OUCH_HANDLER_H