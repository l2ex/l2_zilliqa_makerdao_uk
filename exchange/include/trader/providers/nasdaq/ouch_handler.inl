/*!
    \file ouch_handler.inl
    \brief NASDAQ OUCH handler inline implementation
    \author Ivan Shynkarenka
    \date 21.07.2017
    \copyright MIT License
*/

#include <cassert>

namespace TradingPlatform {
namespace OUCH {

inline size_t EnterOrderMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 43) && "Invalid size of the OUCH message type 'O'");
    if (size != 43)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderToken);
    *data++ = this->AccountType;
    data += CppCommon::Endian::WriteBigEndian(data, this->AccountId);
    *data++ = this->OrderVerb;
    data += CppCommon::Endian::WriteBigEndian(data, this->Shares);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderbookId);
    data += CppCommon::Endian::WriteBigEndian(data, this->Price);
    data += CppCommon::Endian::WriteBigEndian(data, this->TimeInForce);
    data += CppCommon::Endian::WriteBigEndian(data, this->ClientId);
    data += CppCommon::Endian::WriteBigEndian(data, this->MinimumQuantity);
    
    return size;
}

inline bool EnterOrderMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 43) && "Invalid size of the OUCH message type 'O'");
    if (size != 43)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderToken);
    this->AccountType = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->AccountId);
    this->OrderVerb = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->Shares);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderbookId);
    data += CppCommon::Endian::ReadBigEndian(data, this->Price);
    data += CppCommon::Endian::ReadBigEndian(data, this->TimeInForce);
    data += CppCommon::Endian::ReadBigEndian(data, this->ClientId);
    data += CppCommon::Endian::ReadBigEndian(data, this->MinimumQuantity);
    
    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const EnterOrderMessage& message)
{
    stream << "EnterOrderMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; OrderToken=" << message.OrderToken
        << "; AccountType=" << CppCommon::WriteChar(message.AccountType)
        << "; AccountId=" << message.AccountId
        << "; OrderVerb=" << CppCommon::WriteChar(message.OrderVerb)
        << "; Shares=" << message.Shares
        << "; OrderbookId=" << message.OrderbookId
        << "; Price=" << message.Price
        << "; TimeInForce=" << message.TimeInForce
        << "; ClientId=" << message.ClientId
        << "; MinimumQuantity=" << message.MinimumQuantity
        << ")";
    return stream;
}

inline size_t ReplaceOrderMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 21) && "Invalid size of the OUCH message type 'U'");
    if (size != 21)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->ExistingOrderToken);
    data += CppCommon::Endian::WriteBigEndian(data, this->ReplacementOrderToken);
    data += CppCommon::Endian::WriteBigEndian(data, this->Shares);
    data += CppCommon::Endian::WriteBigEndian(data, this->Price);
    
    return size;
}

inline bool ReplaceOrderMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 21) && "Invalid size of the OUCH message type 'U'");
    if (size != 21)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->ExistingOrderToken);
    data += CppCommon::Endian::ReadBigEndian(data, this->ReplacementOrderToken);
    data += CppCommon::Endian::ReadBigEndian(data, this->Shares);
    data += CppCommon::Endian::ReadBigEndian(data, this->Price);
    
    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const ReplaceOrderMessage& message)
{
    stream << "ReplaceOrderMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; ExistingOrderToken=" << message.ExistingOrderToken
        << "; ReplacementOrderToken=" << message.ReplacementOrderToken
        << "; Shares=" << message.Shares
        << "; Price=" << message.Price
        << ")";
    return stream;
}

inline size_t CancelOrderMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 5) && "Invalid size of the OUCH message type 'X'");
    if (size != 5)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderToken);
    
    return size;
}

inline bool CancelOrderMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 5) && "Invalid size of the OUCH message type 'X'");
    if (size != 5)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderToken);
    
    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const CancelOrderMessage& message)
{
    stream << "CancelOrderMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; OrderToken=" << message.OrderToken
        << ")";
    return stream;
}

inline size_t UnknownMessage::serialize(void *buffer, size_t size) const
{
    assert((size > 0) && "Invalid size of the unknown OUCH message!");
    if (size == 0)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    *data = this->Type;

    return size;
}

inline bool UnknownMessage::deserialize(void *buffer, size_t size)
{
    assert((size > 0) && "Invalid size of the unknown OUCH message!");
    if (size == 0)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data;

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const UnknownMessage& message)
{
    stream << "UnknownMessage(Type=" << CppCommon::WriteChar(message.Type) << ")";
    return stream;
}

inline size_t SystemEventMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 10) && "Invalid size of the OUCH message type 'S'");
    if (size != 10)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->Timestamp);
    *data++ = this->EventCode;
    
    return size;
}

inline bool SystemEventMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 10) && "Invalid size of the OUCH message type 'S'");
    if (size != 10)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->Timestamp);
    this->EventCode = *data++;
    
    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const SystemEventMessage& message)
{
    stream << "SystemEventMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; Timestamp=" << message.Timestamp
        << "; AccountType=" << CppCommon::WriteChar(message.EventCode)
        << ")";
    return stream;
}

inline size_t OrderAcceptedMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 59) && "Invalid size of the OUCH message type 'A'");
    if (size != 59)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderToken);
    *data++ = this->AccountType;
    data += CppCommon::Endian::WriteBigEndian(data, this->AccountId);
    *data++ = this->OrderVerb;
    data += CppCommon::Endian::WriteBigEndian(data, this->Shares);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderbookId);
    data += CppCommon::Endian::WriteBigEndian(data, this->Price);
    data += CppCommon::Endian::WriteBigEndian(data, this->TimeInForce);
    data += CppCommon::Endian::WriteBigEndian(data, this->ClientId);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderReferenceNumber);
    data += CppCommon::Endian::WriteBigEndian(data, this->MinimumQuantity);
    *data++ = this->OrderState;
    
    return size;
}

inline bool OrderAcceptedMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 59) && "Invalid size of the OUCH message type 'A'");
    if (size != 59)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::WriteBigEndian(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderToken);
    this->AccountType = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->AccountId);
    this->OrderVerb = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->Shares);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderbookId);
    data += CppCommon::Endian::ReadBigEndian(data, this->Price);
    data += CppCommon::Endian::ReadBigEndian(data, this->TimeInForce);
    data += CppCommon::Endian::ReadBigEndian(data, this->ClientId);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderReferenceNumber);
    data += CppCommon::Endian::ReadBigEndian(data, this->MinimumQuantity);
    this->OrderState = *data++;
    
    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const OrderAcceptedMessage& message)
{
    stream << "OrderAcceptedMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; Timestamp=" << message.Timestamp
        << "; OrderToken=" << message.OrderToken
        << "; AccountType=" << CppCommon::WriteChar(message.AccountType)
        << "; AccountId=" << message.AccountId
        << "; OrderVerb=" << CppCommon::WriteChar(message.OrderVerb)
        << "; Shares=" << message.Shares
        << "; OrderbookId=" << message.OrderbookId
        << "; Price=" << message.Price
        << "; TimeInForce=" << message.TimeInForce
        << "; ClientId=" << message.ClientId
        << "; OrderReferenceNumber=" << message.OrderReferenceNumber
        << "; MinimumQuantity=" << message.MinimumQuantity
        << "; OrderState=" << CppCommon::WriteChar(message.OrderState)
        << ")";
    return stream;
}

inline size_t OrderRejectedMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 14) && "Invalid size of the OUCH message type 'J'");
    if (size != 14)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderToken);
    *data++ = this->Reason;
    
    return size;
}

inline bool OrderRejectedMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 14) && "Invalid size of the OUCH message type 'J'");
    if (size != 14)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderToken);
    this->Reason = *data++;
    
    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const OrderRejectedMessage& message)
{
    stream << "OrderRejectedMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; Timestamp=" << message.Timestamp
        << "; OrderToken=" << message.OrderToken
        << "; Reason=" << CppCommon::WriteChar(message.Reason)
        << ")";
    return stream;
}

inline size_t OrderReplacedMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 43) && "Invalid size of the OUCH message type 'U'");
    if (size != 43)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->ReplacementOrderToken);
    *data++ = this->OrderVerb;
    data += CppCommon::Endian::WriteBigEndian(data, this->Shares);
    data += CppCommon::Endian::WriteBigEndian(data, this->Price);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderReferenceNumber);
    *data++ = this->OrderState;
    data += CppCommon::Endian::WriteBigEndian(data, this->PreviousOrderToken);
    
    return size;
}

inline bool OrderReplacedMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 43) && "Invalid size of the OUCH message type 'U'");
    if (size != 43)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->ReplacementOrderToken);
    this->OrderVerb = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->Shares);
    data += CppCommon::Endian::ReadBigEndian(data, this->Price);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderReferenceNumber);
    this->OrderState = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->PreviousOrderToken);
    
    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const OrderReplacedMessage& message)
{
    stream << "OrderReplacedMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; Timestamp=" << message.Timestamp
        << "; ReplacementOrderToken=" << message.ReplacementOrderToken
        << "; OrderVerb=" << CppCommon::WriteChar(message.OrderVerb)
        << "; Shares=" << message.Shares
        << "; Price=" << message.Price
        << "; OrderReferenceNumber=" << message.OrderReferenceNumber
        << "; OrderState=" << CppCommon::WriteChar(message.OrderState)
        << "; PreviousOrderToken=" << message.PreviousOrderToken
        << ")";
    return stream;
}

inline size_t OrderCanceledMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 22) && "Invalid size of the OUCH message type 'C'");
    if (size != 22)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderToken);
    data += CppCommon::Endian::WriteBigEndian(data, this->Shares);
    *data++ = this->Reason;
    
    return size;
}

inline bool OrderCanceledMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 22) && "Invalid size of the OUCH message type 'C'");
    if (size != 22)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderToken);
    data += CppCommon::Endian::ReadBigEndian(data, this->Shares);
    this->Reason = *data++;
    
    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const OrderCanceledMessage& message)
{
    stream << "OrderCanceledMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; Timestamp=" << message.Timestamp
        << "; OrderToken=" << message.OrderToken
        << "; Shares=" << message.Shares
        << "; Reason=" << CppCommon::WriteChar(message.Reason)
        << ")";
    return stream;
}

inline size_t OrderExecutedMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 38) && "Invalid size of the OUCH message type 'E'");
    if (size != 38)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderToken);
    data += CppCommon::Endian::WriteBigEndian(data, this->ExecutedShares);
    data += CppCommon::Endian::WriteBigEndian(data, this->ExecutedPrice);
    *data++ = this->LiquidityFlag;
    data += CppCommon::Endian::WriteBigEndian(data, this->MatchNumber);
    data += CppCommon::Endian::WriteBigEndian(data, this->CounterPartyId);
    
    return size;
}

inline bool OrderExecutedMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 38) && "Invalid size of the OUCH message type 'E'");
    if (size != 38)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderToken);
    data += CppCommon::Endian::ReadBigEndian(data, this->ExecutedShares);
    data += CppCommon::Endian::ReadBigEndian(data, this->ExecutedPrice);
    this->LiquidityFlag = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->MatchNumber);
    data += CppCommon::Endian::ReadBigEndian(data, this->CounterPartyId);
    
    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const OrderExecutedMessage& message)
{
    stream << "OrderExecutedMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; Timestamp=" << message.Timestamp
        << "; OrderToken=" << message.OrderToken
        << "; ExecutedShares=" << message.ExecutedShares
        << "; ExecutedPrice=" << message.ExecutedPrice
        << "; LiquidityFlag=" << CppCommon::WriteChar(message.LiquidityFlag)
        << "; MatchNumber=" << message.MatchNumber
        << "; CounterPartyId=" << message.CounterPartyId
        << ")";
    return stream;
}

inline size_t BrokenTradeMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 22) && "Invalid size of the OUCH message type 'B'");
    if (size != 22)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderToken);
    data += CppCommon::Endian::WriteBigEndian(data, this->MatchNumber);
    *data++ = this->Reason;
    
    return size;
}

inline bool BrokenTradeMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 22) && "Invalid size of the OUCH message type 'B'");
    if (size != 22)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderToken);
    data += CppCommon::Endian::ReadBigEndian(data, this->MatchNumber);
    this->Reason = *data++;
    
    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const BrokenTradeMessage& message)
{
    stream << "BrokenTradeMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; Timestamp=" << message.Timestamp
        << "; OrderToken=" << message.OrderToken
        << "; MatchNumber=" << message.MatchNumber
        << "; Reason=" << CppCommon::WriteChar(message.Reason)
        << ")";
    return stream;
}

template <size_t N>
inline size_t SerializableMessage::ReadString(const void* buffer, char (&str)[N])
{
    std::memcpy(str, buffer, N);

    return N;
}

template <size_t N>
inline size_t SerializableMessage::WriteString(void* buffer, const char (&str)[N])
{
    std::memcpy(buffer, str, N);

    return N;
}

inline size_t SerializableMessage::ReadTimestamp(const void* buffer, uint64_t& value)
{
    if (CppCommon::Endian::IsBigEndian())
    {
        ((uint8_t*)&value)[0] = 0;
        ((uint8_t*)&value)[1] = 0;
        ((uint8_t*)&value)[2] = 0;
        ((uint8_t*)&value)[3] = 0;
        ((uint8_t*)&value)[4] = 0;
        ((uint8_t*)&value)[5] = ((const uint8_t*)buffer)[0];
        ((uint8_t*)&value)[6] = ((const uint8_t*)buffer)[1];
        ((uint8_t*)&value)[7] = ((const uint8_t*)buffer)[2];
    }
    else
    {
        ((uint8_t*)&value)[0] = ((const uint8_t*)buffer)[2];
        ((uint8_t*)&value)[1] = ((const uint8_t*)buffer)[1];
        ((uint8_t*)&value)[2] = ((const uint8_t*)buffer)[0];
        ((uint8_t*)&value)[3] = 0;
        ((uint8_t*)&value)[4] = 0;
        ((uint8_t*)&value)[5] = 0;
        ((uint8_t*)&value)[6] = 0;
        ((uint8_t*)&value)[7] = 0;
    }

    return 6;
}

inline size_t SerializableMessage::WriteTimestamp(void* buffer, const uint64_t& value)
{
    if (CppCommon::Endian::IsBigEndian())
    {
        ((uint8_t*)buffer)[0] = ((const uint8_t*)&value)[5];
        ((uint8_t*)buffer)[1] = ((const uint8_t*)&value)[6];
        ((uint8_t*)buffer)[2] = ((const uint8_t*)&value)[7];
    }
    else
    {
        ((uint8_t*)buffer)[2] = ((const uint8_t*)&value)[0];
        ((uint8_t*)buffer)[1] = ((const uint8_t*)&value)[1];
        ((uint8_t*)buffer)[0] = ((const uint8_t*)&value)[2];

    }

    return 6;
}

} // namespace OUCH
} // namespace TradingPlatform
