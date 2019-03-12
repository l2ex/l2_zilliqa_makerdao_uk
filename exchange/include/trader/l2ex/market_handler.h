#ifndef TRADING_PLATFORM_L2EX_MARKET_HANDLER_H
#define TRADING_PLATFORM_L2EX_MARKET_HANDLER_H

#include "trader/matching/market_manager.h"
#include "trader/providers/nasdaq/itch_handler.h"

#define TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS 1

namespace TradingPlatform {
namespace L2ex {

class MarketHandler : public Matching::MarketHandler
{
protected:

    /////////////////////////////////////////////
    // Symbol handlers
    /////////////////////////////////////////////

    void onAddSymbol(const Matching::Symbol &symbol) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Add symbol: " << symbol << std::endl;
#endif
    }

    void onDeleteSymbol(const Matching::Symbol &symbol) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Delete symbol: " << symbol << std::endl;
#endif
    }

    /////////////////////////////////////////////
    // Order book handlers
    /////////////////////////////////////////////

    void onAddOrderBook(const Matching::OrderBook &order_book) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Add order book: " << order_book << std::endl;
#endif
    }

    void onUpdateOrderBook(const Matching::OrderBook &order_book, bool top) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Update order book: " << order_book << (top ? " - Top of the book!" : "") << std::endl;
#endif
    }

    void onDeleteOrderBook(const Matching::OrderBook &order_book) override
    {
#ifdef TRADING_PLATFORM_L2EX_MARKET_PRINT_LOGS
        std::cout << "[L2MM] Delete order book: " << order_book << std::endl;
#endif
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
    }
};

}}

#endif // TRADING_PLATFORM_L2EX_MARKET_HANDLER_H