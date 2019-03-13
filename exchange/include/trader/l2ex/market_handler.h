#ifndef TRADING_PLATFORM_L2EX_MARKET_HANDLER_H
#define TRADING_PLATFORM_L2EX_MARKET_HANDLER_H

#include "trader/matching/market_manager.h"
#include "trader/providers/nasdaq/itch_handler.h"
#include "trader/providers/nasdaq/ouch_handler.h"

#define TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS 1

namespace TradingPlatform {
namespace L2ex {

class MarketHandler : public Matching::MarketHandler
{
public:

    MarketHandler(Aeron::Publisher *itchPublisher = nullptr, Aeron::Publisher *ouchPublisher = nullptr)
        : Matching::MarketHandler()
        , _itchPublisher(itchPublisher)
        , _ouchPublisher(ouchPublisher)
    {
    }

protected:

    /////////////////////////////////////////////
    // Symbol handlers
    /////////////////////////////////////////////

    void onAddSymbol(const Matching::Symbol &symbol) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Add symbol: " << symbol << std::endl;
#endif
        if (_itchPublisher)
        {
            ITCH::StockDirectoryMessage message = {};
            // TODO: Fill message properly
            publishMessageITCH(message);
        }
    }

    void onDeleteSymbol(const Matching::Symbol &symbol) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Delete symbol: " << symbol << std::endl;
#endif
        if (_itchPublisher)
        {
            ITCH::StockDirectoryMessage message = {};
            // TODO: Fill message properly
            publishMessageITCH(message);
        }
    }

    /////////////////////////////////////////////
    // Order book handlers
    /////////////////////////////////////////////

    void onAddOrderBook(const Matching::OrderBook &order_book) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Add order book: " << order_book << std::endl;
#endif
        if (_itchPublisher)
        {
            ITCH::StockDirectoryMessage message = {};
            // TODO: Fill message properly
            //publishMessageITCH(message);
        }
    }

    void onUpdateOrderBook(const Matching::OrderBook &order_book, bool top) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Update order book: " << order_book << (top ? " - Top of the book!" : "") << std::endl;
#endif
        if (_itchPublisher)
        {
            ITCH::StockDirectoryMessage message = {};
            // TODO: Fill message properly
            //publishMessageITCH(message);
        }
    }

    void onDeleteOrderBook(const Matching::OrderBook &order_book) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Delete order book: " << order_book << std::endl;
#endif
        if (_itchPublisher)
        {
            ITCH::StockDirectoryMessage message = {};
            // TODO: Fill message properly
            //publishMessageITCH(message);
        }
    }

    /////////////////////////////////////////////
    // Price level handlers
    /////////////////////////////////////////////

    void onAddLevel(const Matching::OrderBook &order_book, const Matching::Level &level, bool top) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Add level: " << level << (top ? " - Top of the book!" : "") << std::endl;
#endif
    }

    void onUpdateLevel(const Matching::OrderBook &order_book, const Matching::Level &level, bool top) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Update level: " << level << (top ? " - Top of the book!" : "") << std::endl;
#endif
    }

    void onDeleteLevel(const Matching::OrderBook &order_book, const Matching::Level &level, bool top) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Delete level: " << level << (top ? " - Top of the book!" : "") << std::endl;
#endif
    }

    /////////////////////////////////////////////
    // Order handlers
    /////////////////////////////////////////////

    void onAddOrder(const Matching::Order &order) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Add order: " << order << std::endl;
#endif
        if (_itchPublisher)
        {
            ITCH::AddOrderMessage message = {};
            message.Type = 'A';
            message.Timestamp = nanosecondsSinceMidnight();
            message.OrderReferenceNumber = order.Id;
            message.BuySellIndicator = (order.Side == Matching::OrderSide::BUY ? 'B' : 'S');
            message.Shares = static_cast<uint32_t>(order.Quantity);
            message.Price = static_cast<uint32_t>(order.Price);
            publishMessageITCH(message);
        }
        if (_ouchPublisher)
        {
            OUCH::OrderAcceptedMessage message = {};
            message.Type = 'A';
            message.Timestamp = nanosecondsSinceMidnight();
            message.OrderToken = static_cast<uint32_t>(order.Id);
            message.OrderVerb = (order.Side == Matching::OrderSide::BUY ? 'B' : 'S');
            message.Shares = order.Quantity;
            message.Price = static_cast<uint32_t>(order.Price);
            message.OrderReferenceNumber = order.Id;
            message.OrderState = 'L';
            publishMessageOUCH(message);
        }
    }

    void onUpdateOrder(const Matching::Order &order) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Update order: " << order << std::endl;
#endif
    }

    void onDeleteOrder(const Matching::Order &order) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Delete order: " << order << std::endl;
#endif
    }

    /////////////////////////////////////////////
    // Order execution handler
    /////////////////////////////////////////////

    void onExecuteOrder(const Matching::Order &order, uint64_t price, uint64_t quantity) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Execute order: " << order << " with price " << price << " and quantity " << quantity << std::endl;
#endif
        if (_itchPublisher)
        {
            ITCH::OrderExecutedMessage message = {};
            message.Type = 'P';
            message.Timestamp = nanosecondsSinceMidnight();
            message.OrderReferenceNumber = order.Id;
            message.ExecutedShares = static_cast<uint32_t>(quantity);
            publishMessageITCH(message);
        }
        if (_ouchPublisher)
        {
            OUCH::OrderExecutedMessage message = {};
            message.Type = 'E';
            message.Timestamp = nanosecondsSinceMidnight();
            message.OrderToken = static_cast<uint32_t>(order.Id);
            message.ExecutedShares = order.Quantity;
            message.ExecutedPrice = static_cast<uint32_t>(order.Price);
            publishMessageOUCH(message);
        }
    }

private:

    template <class Message>
    void publishMessageITCH(const Message &message)
    {
        if (!_itchPublisher)
            return;
        auto length = message.serialize(_messageSerialized, sizeof(_messageSerialized));
        if (length > 0)
            _itchPublisher->publish(_messageSerialized, length);
    }

    template <class Message>
    void publishMessageOUCH(const Message &message)
    {
        if (!_ouchPublisher)
            return;
        auto length = message.serialize(_messageSerialized, sizeof(_messageSerialized));
        if (length > 0)
            _ouchPublisher->publish(_messageSerialized, length);
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

    Aeron::Publisher *_itchPublisher;
    Aeron::Publisher *_ouchPublisher;
    uint8_t _messageSerialized[1024];
};

}}

#endif // TRADING_PLATFORM_L2EX_MARKET_HANDLER_H