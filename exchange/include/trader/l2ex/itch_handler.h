#ifndef TRADING_PLATFORM_L2EX_ITCH_HANDLER_H
#define TRADING_PLATFORM_L2EX_ITCH_HANDLER_H

#include "trader/matching/market_manager.h"
#include "trader/providers/nasdaq/itch_handler.h"
#include "../../../aeron/publisher.h"

#define TRADING_PLATFORM_L2EX_ITCH_HANDLER_PRINT_LOGS 1

namespace TradingPlatform {
namespace L2ex {

class ITCHHandler : public ITCH::ITCHHandler
{
public:

    ITCHHandler(Matching::MarketManager &market, Aeron::Publisher *publisher = nullptr)
        : _market(market)
        , _publisher(publisher)
    {
    }

protected:

    bool onMessage(const ITCH::StockDirectoryMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_ITCH_HANDLER_PRINT_LOGS
        std::cout << "[ITCH] Message received: " << message << std::endl;
#endif
        Matching::Symbol symbol(message.StockLocate, message.Stock);
        _market.AddSymbol(symbol);
        _market.AddOrderBook(symbol);
        publishMessage(message);
        return true;
    }

    bool onMessage(const ITCH::AddOrderMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_ITCH_HANDLER_PRINT_LOGS
        std::cout << "[ITCH] Message received: " << message << std::endl;
#endif
        Matching::Order order = Matching::Order::Limit(
            message.OrderReferenceNumber,
            message.StockLocate,
            message.BuySellIndicator == 'B' ? Matching::OrderSide::BUY : Matching::OrderSide::SELL,
            message.Price,
            message.Shares
        );
        _market.AddOrder(order);
        publishMessage(message);
        return true;
    }

    bool onMessage(const ITCH::AddOrderMPIDMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_ITCH_HANDLER_PRINT_LOGS
        std::cout << "[ITCH] Message received: " << message << std::endl;
#endif
        Matching::Order order = Matching::Order::Limit(
            message.OrderReferenceNumber,
            message.StockLocate,
            message.BuySellIndicator == 'B' ? Matching::OrderSide::BUY : Matching::OrderSide::SELL,
            message.Price,
            message.Shares
        );
        _market.AddOrder(order);
        publishMessage(message);
        return true;
    }

    bool onMessage(const ITCH::OrderExecutedMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_ITCH_HANDLER_PRINT_LOGS
        std::cout << "[ITCH] Message received: " << message << std::endl;
#endif
        _market.ExecuteOrder(message.OrderReferenceNumber, message.ExecutedShares);
        publishMessage(message);
        return true;
    }

    bool onMessage(const ITCH::OrderExecutedWithPriceMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_ITCH_HANDLER_PRINT_LOGS
        std::cout << "[ITCH] Message received: " << message << std::endl;
#endif
        _market.ExecuteOrder(message.OrderReferenceNumber, message.ExecutionPrice, message.ExecutedShares);
        publishMessage(message);
        return true;
    }

    bool onMessage(const ITCH::OrderCancelMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_ITCH_HANDLER_PRINT_LOGS
        std::cout << "[ITCH] Message received: " << message << std::endl;
#endif
        _market.ReduceOrder(message.OrderReferenceNumber, message.CanceledShares);
        publishMessage(message);
        return true;
    }

    bool onMessage(const ITCH::OrderDeleteMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_ITCH_HANDLER_PRINT_LOGS
        std::cout << "[ITCH] Message received: " << message << std::endl;
#endif
        _market.DeleteOrder(message.OrderReferenceNumber);
        publishMessage(message);
        return true;
    }

    bool onMessage(const ITCH::OrderReplaceMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_ITCH_HANDLER_PRINT_LOGS
        std::cout << "[ITCH] Message received: " << message << std::endl;
#endif
        _market.ReplaceOrder(message.OriginalOrderReferenceNumber, message.NewOrderReferenceNumber, message.Price, message.Shares);
        publishMessage(message);
        return true;
    }

    bool onMessage(const ITCH::UnknownMessage &message) override
    {
#ifdef TRADING_PLATFORM_L2EX_ITCH_HANDLER_PRINT_LOGS
        std::cerr << "[ITCH] Unknown message received: " << message << std::endl;
#endif
        return true;
    }

private:

    template <class Message>
    void publishMessage(const Message &message)
    {
        if (_publisher)
        {
            auto length = message.serialize(_messageSerialized, sizeof(_messageSerialized));
            if (length > 0)
                _publisher->publish(_messageSerialized, length);
        }
    }

private:

    Matching::MarketManager &_market;
    Aeron::Publisher *_publisher;
    uint8_t _messageSerialized[1024];
};

}}

#endif // TRADING_PLATFORM_L2EX_ITCH_HANDLER_H