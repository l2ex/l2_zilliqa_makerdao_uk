/*!
    \file ouch_handler.h
    \brief NASDAQ OUCH handler definition
    \author Igor Sokolov
    \date 12.03.2019
    \copyright MIT License
*/

#ifndef TRADING_PLATFORM_OUCH_HANDLER_H
#define TRADING_PLATFORM_OUCH_HANDLER_H

#include "utility/endian.h"
#include "utility/iostream.h"

#include <vector>

namespace TradingPlatform {

/*!
    \namespace TradingPlatform::OUCH
    \brief OUCH protocol definitions
*/
namespace OUCH {

//! Base class for all OUCH messages
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

/////////////////////////////////////////////
// Inbound message
/////////////////////////////////////////////

//! Enter Order Message
struct EnterOrderMessage : public SerializableMessage
{
    char Type;
    uint32_t OrderToken;
    char AccountType;
    uint32_t AccountId;
    char OrderVerb;
    uint64_t Shares;
    uint32_t OrderbookId;
    uint32_t Price;
    uint32_t TimeInForce;
    uint32_t ClientId;
    uint64_t MinimumQuantity;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const EnterOrderMessage& message);
};

//! Replace Order Message
struct ReplaceOrderMessage : public SerializableMessage
{
    char Type;
    uint32_t ExistingOrderToken;
    uint32_t ReplacementOrderToken;
    uint64_t Shares;
    uint32_t Price;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const ReplaceOrderMessage& message);
};

//! Cancel Order Message
struct CancelOrderMessage : public SerializableMessage
{
    char Type;
    uint32_t OrderToken;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const CancelOrderMessage& message);
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

/////////////////////////////////////////////
// Outbound message
/////////////////////////////////////////////

//! System Event Message
struct SystemEventMessage : public SerializableMessage
{
    char Type;
    uint64_t Timestamp;
    char EventCode;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const SystemEventMessage& message);
};

//! Order Accepted Message
struct OrderAcceptedMessage : public SerializableMessage
{
    char Type;
    uint64_t Timestamp;
    uint32_t OrderToken;
    char AccountType;
    uint32_t AccountId;
    char OrderVerb;
    uint64_t Shares;
    uint32_t OrderbookId;
    uint32_t Price;
    uint32_t TimeInForce;
    uint32_t ClientId;
    uint64_t OrderReferenceNumber;
    uint64_t MinimumQuantity;
    char OrderState;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const OrderAcceptedMessage& message);
};

//! Order Rejected Message
struct OrderRejectedMessage : public SerializableMessage
{
    char Type;
    uint64_t Timestamp;
    uint32_t OrderToken;
    char Reason;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const OrderRejectedMessage& message);
};

//! Order Replaced Message
struct OrderReplacedMessage : public SerializableMessage
{
    char Type;
    uint64_t Timestamp;
    uint32_t ReplacementOrderToken;
    char OrderVerb;
    uint64_t Shares;
    uint32_t OrderbookId;
    uint32_t Price;
    uint64_t OrderReferenceNumber;
    char OrderState;
    uint32_t PreviousOrderToken;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const OrderReplacedMessage& message);
};

//! Order Canceled Message
struct OrderCanceledMessage : public SerializableMessage
{
    char Type;
    uint64_t Timestamp;
    uint32_t OrderToken;
    uint64_t Shares;
    char Reason;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const OrderCanceledMessage& message);
};

//! Order Executed Message
struct OrderExecutedMessage : public SerializableMessage
{
    char Type;
    uint64_t Timestamp;
    uint32_t OrderToken;
    uint64_t ExecutedShares;
    uint32_t ExecutedPrice;
    char LiquidityFlag;
    uint64_t MatchNumber;
    uint32_t CounterPartyId;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const OrderExecutedMessage& message);
};

//! Broken Trade Message
struct BrokenTradeMessage : public SerializableMessage
{
    char Type;
    uint64_t Timestamp;
    uint32_t OrderToken;
    uint64_t MatchNumber;
    char Reason;

    size_t serialize(void *buffer, size_t size) const override;
    bool deserialize(void *buffer, size_t size) override;

    template <class TOutputStream>
    friend TOutputStream& operator<<(TOutputStream& stream, const BrokenTradeMessage& message);
};

/////////////////////////////////////////////
// Handler
/////////////////////////////////////////////

//! NASDAQ OUCH handler class
/*!
    NASDAQ OUCH handler is used to parse NASDAQ OUCH protocol and handle its
    messages in special handlers.

    NASDAQ OUCH protocol specification:
    http://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVOUCHSpecification.pdf

    NASDAQ OUCH protocol examples:
    ftp://emi.nasdaq.com/OUCH

    Not thread-safe.
*/
class OUCHHandler
{
public:
    OUCHHandler() { Reset(); }
    OUCHHandler(const OUCHHandler&) = delete;
    OUCHHandler(OUCHHandler&&) noexcept = default;
    virtual ~OUCHHandler() = default;

    OUCHHandler& operator=(const OUCHHandler&) = delete;
    OUCHHandler& operator=(OUCHHandler&&) noexcept = default;

    //! Process all messages from the given buffer in OUCH format and call corresponding handlers
    /*!
        \param buffer - Buffer to process
        \param size - Buffer size
        \return 'true' if the given buffer was successfully processed, 'false' if the given buffer process was failed
    */
    bool Process(void* buffer, size_t size);
    //! Process a single message from the given buffer in OUCH format and call corresponding handlers
    /*!
        \param buffer - Buffer to process
        \param size - Buffer size
        \return 'true' if the given buffer was successfully processed, 'false' if the given buffer process was failed
    */
    bool ProcessMessage(void* buffer, size_t size);

    //! Reset OUCH handler
    void Reset();

protected:
    // Message handlers
    virtual bool onMessage(const EnterOrderMessage& message) { return true; }
    virtual bool onMessage(const ReplaceOrderMessage& message) { return true; }
    virtual bool onMessage(const CancelOrderMessage& message) { return true; }
    virtual bool onMessage(const UnknownMessage& message) { return true; }

private:
    bool ProcessEnterOrderMessage(void* buffer, size_t size);
    bool ProcessReplaceOrderMessage(void* buffer, size_t size);
    bool ProcessCancelOrderMessage(void* buffer, size_t size);
    bool ProcessUnknownMessage(void* buffer, size_t size);

private:
    size_t _size;
    std::vector<uint8_t> _cache;
};

/*! \example ouch_handler.cpp NASDAQ OUCH handler example */

} // namespace OUCH
} // namespace TradingPlatform

#include "ouch_handler.inl"

#endif // TRADING_PLATFORM_OUCH_HANDLER_H
