/*!
    \file itch_handler.h
    \brief NASDAQ ITCH handler definition
    \author Ivan Shynkarenka
    \date 21.07.2017
    \copyright MIT License
*/

#ifndef TRADING_PLATFORM_ITCH_HANDLER_H
#define TRADING_PLATFORM_ITCH_HANDLER_H

#include "utility/endian.h"
#include "utility/iostream.h"

#include <vector>

namespace TradingPlatform {

/*!
    \namespace TradingPlatform::ITCH
    \brief ITCH protocol definitions
*/
namespace ITCH {

//! Base class for all ITCH messages
class SerializableMessage
{
public:
    virtual size_t serialize(void *buffer, size_t size) const = 0;
    virtual bool deserialize(void *buffer, size_t size) = 0;

protected:
    template <size_t N>
    static size_t ReadString(const void* buffer, char (&str)[N]);
    template <size_t N>
    static size_t WriteString(void* buffer, const char (&str)[N]);

    static size_t ReadTimestamp(const void* buffer, uint64_t& value);
    static size_t WriteTimestamp(void* buffer, const uint64_t& value);
};

//! System Event Message
struct SystemEventMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    char EventCode;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const SystemEventMessage& message);
};

//! Stock Directory Message
struct StockDirectoryMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    char Stock[8];
    char MarketCategory;
    char FinancialStatusIndicator;
    uint32_t RoundLotSize;
    char RoundLotsOnly;
    char IssueClassification;
    char IssueSubType[2];
    char Authenticity;
    char ShortSaleThresholdIndicator;
    char IPOFlag;
    char LULDReferencePriceTier;
    char ETPFlag;
    uint32_t ETPLeverageFactor;
    char InverseIndicator;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const StockDirectoryMessage& message);
};

//! Stock Trading Action Message
struct StockTradingActionMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    char Stock[8];
    char TradingState;
    char Reserved;
    char Reason;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const StockTradingActionMessage& message);
};

//! Reg SHO Short Sale Price Test Restricted Indicator Message
struct RegSHOMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    char Stock[8];
    char RegSHOAction;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const RegSHOMessage& message);
};

//! Market Participant Position Message
struct MarketParticipantPositionMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    char MPID[4];
    char Stock[8];
    char PrimaryMarketMaker;
    char MarketMakerMode;
    char MarketParticipantState;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const MarketParticipantPositionMessage& message);
};

//! MWCB Decline Level Message
struct MWCBDeclineMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    uint64_t Level1;
    uint64_t Level2;
    uint64_t Level3;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const MWCBDeclineMessage& message);
};

//! MWCB Status Message
struct MWCBStatusMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    char BreachedLevel;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const MWCBStatusMessage& message);
};

//! IPO Quoting Period Update Message
struct IPOQuotingMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    char Stock[8];
    uint32_t IPOReleaseTime;
    char IPOReleaseQualifier;
    uint32_t IPOPrice;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const IPOQuotingMessage& message);
};

//! Add Order Message
struct AddOrderMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    uint64_t OrderReferenceNumber;
    char BuySellIndicator;
    uint32_t Shares;
    char Stock[8];
    uint32_t Price;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const AddOrderMessage& message);
};

//! Add Order with MPID Attribution Message
struct AddOrderMPIDMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    uint64_t OrderReferenceNumber;
    char BuySellIndicator;
    uint32_t Shares;
    char Stock[8];
    uint32_t Price;
    char Attribution;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const AddOrderMPIDMessage& message);
};

//! Order Executed Message
struct OrderExecutedMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    uint64_t OrderReferenceNumber;
    uint32_t ExecutedShares;
    uint64_t MatchNumber;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const OrderExecutedMessage& message);
};

//! Order Executed With Price Message
struct OrderExecutedWithPriceMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    uint64_t OrderReferenceNumber;
    uint32_t ExecutedShares;
    uint64_t MatchNumber;
    char Printable;
    uint32_t ExecutionPrice;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const OrderExecutedWithPriceMessage& message);
};

//! Order Cancel Message
struct OrderCancelMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    uint64_t OrderReferenceNumber;
    uint32_t CanceledShares;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const OrderCancelMessage& message);
};

//! Order Delete Message
struct OrderDeleteMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    uint64_t OrderReferenceNumber;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const OrderDeleteMessage& message);
};

//! Order Replace Message
struct OrderReplaceMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    uint64_t OriginalOrderReferenceNumber;
    uint64_t NewOrderReferenceNumber;
    uint32_t Shares;
    uint32_t Price;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const OrderReplaceMessage& message);
};

//! Trade Message
struct TradeMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    uint64_t OrderReferenceNumber;
    char BuySellIndicator;
    uint32_t Shares;
    char Stock[8];
    uint32_t Price;
    uint64_t MatchNumber;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const TradeMessage& message);
};

//! Cross Trade Message
struct CrossTradeMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    uint64_t Shares;
    char Stock[8];
    uint32_t CrossPrice;
    uint64_t MatchNumber;
    char CrossType;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const CrossTradeMessage& message);
};

//! Broken Trade Message
struct BrokenTradeMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    uint64_t MatchNumber;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const BrokenTradeMessage& message);
};

//! Net Order Imbalance Indicator (NOII) Message
struct NOIIMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    uint64_t PairedShares;
    uint64_t ImbalanceShares;
    char ImbalanceDirection;
    char Stock[8];
    uint32_t FarPrice;
    uint32_t NearPrice;
    uint32_t CurrentReferencePrice;
    char CrossType;
    char PriceVariationIndicator;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const NOIIMessage& message);
};

//! Retail Price Improvement Indicator (RPII) Messsage
struct RPIIMessage : public SerializableMessage
{
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    char Stock[8];
    char InterestFlag;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const RPIIMessage& message);
};

//! Unknown message
struct UnknownMessage : public SerializableMessage
{
    char Type;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const UnknownMessage& message);
};

//! NASDAQ ITCH handler class
/*!
    NASDAQ ITCH handler is used to parse NASDAQ ITCH protocol and handle its
    messages in special handlers.

    NASDAQ ITCH protocol specification:
    http://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHSpecification.pdf

    NASDAQ ITCH protocol examples:
    ftp://emi.nasdaq.com/ITCH

    Not thread-safe.
*/
class ITCHHandler
{
public:
    ITCHHandler() { Reset(); }
    ITCHHandler(const ITCHHandler&) = delete;
    ITCHHandler(ITCHHandler&&) noexcept = default;
    virtual ~ITCHHandler() = default;

    ITCHHandler& operator=(const ITCHHandler&) = delete;
    ITCHHandler& operator=(ITCHHandler&&) noexcept = default;

    //! Process all messages from the given buffer in ITCH format and call corresponding handlers
    /*!
        \param buffer - Buffer to process
        \param size - Buffer size
        \return 'true' if the given buffer was successfully processed, 'false' if the given buffer process was failed
    */
    bool Process(void* buffer, size_t size);
    //! Process a single message from the given buffer in ITCH format and call corresponding handlers
    /*!
        \param buffer - Buffer to process
        \param size - Buffer size
        \return 'true' if the given buffer was successfully processed, 'false' if the given buffer process was failed
    */
    bool ProcessMessage(void* buffer, size_t size);

    //! Reset ITCH handler
    void Reset();

protected:
    // Message handlers
    virtual bool onMessage(const SystemEventMessage& message) { return true; }
    virtual bool onMessage(const StockDirectoryMessage& message) { return true; }
    virtual bool onMessage(const StockTradingActionMessage& message) { return true; }
    virtual bool onMessage(const RegSHOMessage& message) { return true; }
    virtual bool onMessage(const MarketParticipantPositionMessage& message) { return true; }
    virtual bool onMessage(const MWCBDeclineMessage& message) { return true; }
    virtual bool onMessage(const MWCBStatusMessage& message) { return true; }
    virtual bool onMessage(const IPOQuotingMessage& message) { return true; }
    virtual bool onMessage(const AddOrderMessage& message) { return true; }
    virtual bool onMessage(const AddOrderMPIDMessage& message) { return true; }
    virtual bool onMessage(const OrderExecutedMessage& message) { return true; }
    virtual bool onMessage(const OrderExecutedWithPriceMessage& message) { return true; }
    virtual bool onMessage(const OrderCancelMessage& message) { return true; }
    virtual bool onMessage(const OrderDeleteMessage& message) { return true; }
    virtual bool onMessage(const OrderReplaceMessage& message) { return true; }
    virtual bool onMessage(const TradeMessage& message) { return true; }
    virtual bool onMessage(const CrossTradeMessage& message) { return true; }
    virtual bool onMessage(const BrokenTradeMessage& message) { return true; }
    virtual bool onMessage(const NOIIMessage& message) { return true; }
    virtual bool onMessage(const RPIIMessage& message) { return true; }
    virtual bool onMessage(const UnknownMessage& message) { return true; }

private:
    bool ProcessSystemEventMessage(void* buffer, size_t size);
    bool ProcessStockDirectoryMessage(void* buffer, size_t size);
    bool ProcessStockTradingActionMessage(void* buffer, size_t size);
    bool ProcessRegSHOMessage(void* buffer, size_t size);
    bool ProcessMarketParticipantPositionMessage(void* buffer, size_t size);
    bool ProcessMWCBDeclineMessage(void* buffer, size_t size);
    bool ProcessMWCBStatusMessage(void* buffer, size_t size);
    bool ProcessIPOQuotingMessage(void* buffer, size_t size);
    bool ProcessAddOrderMessage(void* buffer, size_t size);
    bool ProcessAddOrderMPIDMessage(void* buffer, size_t size);
    bool ProcessOrderExecutedMessage(void* buffer, size_t size);
    bool ProcessOrderExecutedWithPriceMessage(void* buffer, size_t size);
    bool ProcessOrderCancelMessage(void* buffer, size_t size);
    bool ProcessOrderDeleteMessage(void* buffer, size_t size);
    bool ProcessOrderReplaceMessage(void* buffer, size_t size);
    bool ProcessTradeMessage(void* buffer, size_t size);
    bool ProcessCrossTradeMessage(void* buffer, size_t size);
    bool ProcessBrokenTradeMessage(void* buffer, size_t size);
    bool ProcessNOIIMessage(void* buffer, size_t size);
    bool ProcessRPIIMessage(void* buffer, size_t size);
    bool ProcessUnknownMessage(void* buffer, size_t size);

private:
    size_t _size;
    std::vector<uint8_t> _cache;
};

/*! \example itch_handler.cpp NASDAQ ITCH handler example */

} // namespace ITCH
} // namespace TradingPlatform

#include "itch_handler.inl"

#endif // TRADING_PLATFORM_ITCH_HANDLER_H
