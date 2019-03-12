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
            _market.AddOrder(order);
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
            _market.AddOrder(order);
        }
        publishMessage(message);
        return true;
    }

    bool onMessage(const OUCH::ReplaceOrderMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_OUCH_HANDLER_PRINT_LOGS
        std::cout << "[OUTH] Message received: " << message << std::endl;
#endif
        // TODO: Implement
        publishMessage(message);
        return true;
    }

    bool onMessage(const OUCH::CancelOrderMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_OUCH_HANDLER_PRINT_LOGS
        std::cout << "[OUTH] Message received: " << message << std::endl;
#endif
        // TODO: Implement
        publishMessage(message);
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
        auto length = message.serialize(_messageSerialized, sizeof(_messageSerialized));
        if (length > 0 && _publisher)
            _publisher->publish(_messageSerialized, length);
    }

private:

    Matching::MarketManager &_market;
    Aeron::Publisher *_publisher;
    uint8_t _messageSerialized[1024];
};

}}

#endif // TRADING_PLATFORM_L2EX_OUCH_HANDLER_H