/*!
    \file itch_handler.inl
    \brief NASDAQ ITCH handler inline implementation
    \author Ivan Shynkarenka
    \date 21.07.2017
    \copyright MIT License
*/

namespace TradingPlatform {
namespace ITCH {

inline size_t SystemEventMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 12) && "Invalid size of the ITCH message type 'S'");
    if (size != 12)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    *data++ = this->EventCode;
    
    return size;
}

inline bool SystemEventMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 12) && "Invalid size of the ITCH message type 'S'");
    if (size != 12)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    this->EventCode = *data++;
    
    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const SystemEventMessage& message)
{
    stream << "SystemEventMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; EventCode=" << CppCommon::WriteChar(message.EventCode)
        << ")";
    return stream;
}

inline size_t StockDirectoryMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 39) && "Invalid size of the ITCH message type 'R'");
    if (size != 39)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += WriteString(data, this->Stock);
    *data++ = this->MarketCategory;
    *data++ = this->FinancialStatusIndicator;
    data += CppCommon::Endian::WriteBigEndian(data, this->RoundLotSize);
    *data++ = this->RoundLotsOnly;
    *data++ = this->IssueClassification;
    data += WriteString(data, this->IssueSubType);
    *data++ = this->Authenticity;
    *data++ = this->ShortSaleThresholdIndicator;
    *data++ = this->IPOFlag;
    *data++ = this->LULDReferencePriceTier;
    *data++ = this->ETPFlag;
    data += CppCommon::Endian::WriteBigEndian(data, this->ETPLeverageFactor);
    *data++ = this->InverseIndicator;

    return size;
}

inline bool StockDirectoryMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 39) && "Invalid size of the ITCH message type 'R'");
    if (size != 39)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += ReadString(data, this->Stock);
    this->MarketCategory = *data++;
    this->FinancialStatusIndicator = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->RoundLotSize);
    this->RoundLotsOnly = *data++;
    this->IssueClassification = *data++;
    data += ReadString(data, this->IssueSubType);
    this->Authenticity = *data++;
    this->ShortSaleThresholdIndicator = *data++;
    this->IPOFlag = *data++;
    this->LULDReferencePriceTier = *data++;
    this->ETPFlag = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->ETPLeverageFactor);
    this->InverseIndicator = *data++;

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const StockDirectoryMessage& message)
{
    stream << "StockDirectoryMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; Stock=" << CppCommon::WriteString(message.Stock)
        << "; MarketCategory=" << CppCommon::WriteChar(message.MarketCategory)
        << "; FinancialStatusIndicator=" << CppCommon::WriteChar(message.FinancialStatusIndicator)
        << "; RoundLotSize=" << message.RoundLotSize
        << "; RoundLotsOnly=" << CppCommon::WriteChar(message.RoundLotsOnly)
        << "; IssueClassification=" << CppCommon::WriteChar(message.IssueClassification)
        << "; IssueSubType=" << CppCommon::WriteString(message.IssueSubType)
        << "; Authenticity=" << CppCommon::WriteChar(message.Authenticity)
        << "; ShortSaleThresholdIndicator=" << CppCommon::WriteChar(message.ShortSaleThresholdIndicator)
        << "; IPOFlag=" << CppCommon::WriteChar(message.IPOFlag)
        << "; LULDReferencePriceTier=" << CppCommon::WriteChar(message.LULDReferencePriceTier)
        << "; ETPFlag=" << CppCommon::WriteChar(message.ETPFlag)
        << "; ETPLeverageFactor=" << message.ETPLeverageFactor
        << "; InverseIndicator=" << CppCommon::WriteChar(message.InverseIndicator)
        << ")";
    return stream;
}

inline size_t StockTradingActionMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 25) && "Invalid size of the ITCH message type 'H'");
    if (size != 25)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += WriteString(data, this->Stock);
    *data++ = this->TradingState;
    *data++ = this->Reserved;
    *data++ = this->Reason;

    return size;
}

inline bool StockTradingActionMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 25) && "Invalid size of the ITCH message type 'H'");
    if (size != 25)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += ReadString(data, this->Stock);
    this->TradingState = *data++;
    this->Reserved = *data++;
    this->Reason = *data++;

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const StockTradingActionMessage& message)
{
    stream << "StockTradingActionMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; Stock=" << CppCommon::WriteString(message.Stock)
        << "; TradingState=" << CppCommon::WriteChar(message.TradingState)
        << "; Reserved=" << CppCommon::WriteChar(message.Reserved)
        << "; Reason=" << CppCommon::WriteChar(message.Reason)
        << ")";
    return stream;
}

inline size_t RegSHOMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 20) && "Invalid size of the ITCH message type 'Y'");
    if (size != 20)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += WriteString(data, this->Stock);
    *data++ = this->RegSHOAction;

    return size;
}

inline bool RegSHOMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 20) && "Invalid size of the ITCH message type 'Y'");
    if (size != 20)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += ReadString(data, this->Stock);
    this->RegSHOAction = *data++;

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const RegSHOMessage& message)
{
    stream << "RegSHOMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; Stock=" << CppCommon::WriteString(message.Stock)
        << "; RegSHOAction=" << CppCommon::WriteChar(message.RegSHOAction)
        << ")";
    return stream;
}

inline size_t MarketParticipantPositionMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 26) && "Invalid size of the ITCH message type 'L'");
    if (size != 26)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += WriteString(data, this->MPID);
    data += WriteString(data, this->Stock);
    *data++ = this->PrimaryMarketMaker;
    *data++ = this->MarketMakerMode;
    *data++ = this->MarketParticipantState;

    return size;
}

inline bool MarketParticipantPositionMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 26) && "Invalid size of the ITCH message type 'L'");
    if (size != 26)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += ReadString(data, this->MPID);
    data += ReadString(data, this->Stock);
    this->PrimaryMarketMaker = *data++;
    this->MarketMakerMode = *data++;
    this->MarketParticipantState = *data++;

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const MarketParticipantPositionMessage& message)
{
    stream << "MarketParticipantPositionMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; MPID=" << CppCommon::WriteString(message.MPID)
        << "; Stock=" << CppCommon::WriteString(message.Stock)
        << "; PrimaryMarketMaker=" << CppCommon::WriteChar(message.PrimaryMarketMaker)
        << "; MarketMakerMode=" << CppCommon::WriteChar(message.MarketMakerMode)
        << "; MarketParticipantState=" << CppCommon::WriteChar(message.MarketParticipantState)
        << ")";
    return stream;
}

inline size_t MWCBDeclineMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 35) && "Invalid size of the ITCH message type 'V'");
    if (size != 35)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->Level1);
    data += CppCommon::Endian::WriteBigEndian(data, this->Level2);
    data += CppCommon::Endian::WriteBigEndian(data, this->Level3);

    return size;
}

inline bool MWCBDeclineMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 35) && "Invalid size of the ITCH message type 'V'");
    if (size != 35)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->Level1);
    data += CppCommon::Endian::ReadBigEndian(data, this->Level2);
    data += CppCommon::Endian::ReadBigEndian(data, this->Level3);

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const MWCBDeclineMessage& message)
{
    stream << "MWCBDeclineMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; Level1=" << message.Level1
        << "; Level2=" << message.Level2
        << "; Level3=" << message.Level3
        << ")";
    return stream;
}

inline size_t MWCBStatusMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 12) && "Invalid size of the ITCH message type 'W'");
    if (size != 12)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    *data++ = this->BreachedLevel;

    return size;
}

inline bool MWCBStatusMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 12) && "Invalid size of the ITCH message type 'W'");
    if (size != 12)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    this->BreachedLevel = *data++;

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const MWCBStatusMessage& message)
{
    stream << "MWCBStatusMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; BreachedLevel=" << message.BreachedLevel
        << ")";
    return stream;
}

inline size_t IPOQuotingMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 28) && "Invalid size of the ITCH message type 'W'");
    if (size != 28)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += WriteString(data, this->Stock);
    data += CppCommon::Endian::WriteBigEndian(data, this->IPOReleaseTime);
    *data++ = this->IPOReleaseQualifier;
    data += CppCommon::Endian::WriteBigEndian(data, this->IPOPrice);

    return size;
}

inline bool IPOQuotingMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 28) && "Invalid size of the ITCH message type 'W'");
    if (size != 28)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += ReadString(data, this->Stock);
    data += CppCommon::Endian::ReadBigEndian(data, this->IPOReleaseTime);
    this->IPOReleaseQualifier = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->IPOPrice);

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const IPOQuotingMessage& message)
{
    stream << "IPOQuotingMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; Stock=" << CppCommon::WriteString(message.Stock)
        << "; IPOReleaseTime=" << message.IPOReleaseTime
        << "; IPOReleaseQualifier=" << message.IPOReleaseQualifier
        << "; IPOPrice=" << message.IPOPrice
        << ")";
    return stream;
}

inline size_t AddOrderMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 36) && "Invalid size of the ITCH message type 'A'");
    if (size != 36)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderReferenceNumber);
    *data++ = this->BuySellIndicator;
    data += CppCommon::Endian::WriteBigEndian(data, this->Shares);
    data += WriteString(data, this->Stock);
    data += CppCommon::Endian::WriteBigEndian(data, this->Price);
    
    return size;
}

inline bool AddOrderMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 36) && "Invalid size of the ITCH message type 'A'");
    if (size != 36)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderReferenceNumber);
    this->BuySellIndicator = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->Shares);
    data += ReadString(data, this->Stock);
    data += CppCommon::Endian::ReadBigEndian(data, this->Price);

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const AddOrderMessage& message)
{
    stream << "AddOrderMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; OrderReferenceNumber=" << message.OrderReferenceNumber
        << "; BuySellIndicator=" << CppCommon::WriteChar(message.BuySellIndicator)
        << "; Shares=" << message.Shares
        << "; Stock=" << CppCommon::WriteString(message.Stock)
        << "; Price=" << message.Price
        << ")";
    return stream;
}

inline size_t AddOrderMPIDMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 40) && "Invalid size of the ITCH message type 'F'");
    if (size != 40)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderReferenceNumber);
    *data++ = this->BuySellIndicator;
    data += CppCommon::Endian::WriteBigEndian(data, this->Shares);
    data += WriteString(data, this->Stock);
    data += CppCommon::Endian::WriteBigEndian(data, this->Price);
    *data++ = this->Attribution;

    return size;
}

inline bool AddOrderMPIDMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 40) && "Invalid size of the ITCH message type 'F'");
    if (size != 40)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderReferenceNumber);
    this->BuySellIndicator = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->Shares);
    data += ReadString(data, this->Stock);
    data += CppCommon::Endian::ReadBigEndian(data, this->Price);
    this->Attribution = *data++;

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const AddOrderMPIDMessage& message)
{
    stream << "AddOrderMPIDMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; OrderReferenceNumber=" << message.OrderReferenceNumber
        << "; BuySellIndicator=" << CppCommon::WriteChar(message.BuySellIndicator)
        << "; Shares=" << message.Shares
        << "; Stock=" << CppCommon::WriteString(message.Stock)
        << "; Price=" << message.Price
        << "; Attribution=" << CppCommon::WriteChar(message.Attribution)
        << ")";
    return stream;
}

inline size_t OrderExecutedMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 31) && "Invalid size of the ITCH message type 'E'");
    if (size != 31)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderReferenceNumber);
    data += CppCommon::Endian::WriteBigEndian(data, this->ExecutedShares);
    data += CppCommon::Endian::WriteBigEndian(data, this->MatchNumber);

    return size;
}

inline bool OrderExecutedMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 31) && "Invalid size of the ITCH message type 'E'");
    if (size != 31)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderReferenceNumber);
    data += CppCommon::Endian::ReadBigEndian(data, this->ExecutedShares);
    data += CppCommon::Endian::ReadBigEndian(data, this->MatchNumber);

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const OrderExecutedMessage& message)
{
    stream << "OrderExecutedMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; OrderReferenceNumber=" << message.OrderReferenceNumber
        << "; ExecutedShares=" << message.ExecutedShares
        << "; MatchNumber=" << message.MatchNumber
        << ")";
    return stream;
}

inline size_t OrderExecutedWithPriceMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 36) && "Invalid size of the ITCH message type 'C'");
    if (size != 36)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderReferenceNumber);
    data += CppCommon::Endian::WriteBigEndian(data, this->ExecutedShares);
    data += CppCommon::Endian::WriteBigEndian(data, this->MatchNumber);
    *data++ = this->Printable;
    data += CppCommon::Endian::WriteBigEndian(data, this->ExecutionPrice);

    return size;
}

inline bool OrderExecutedWithPriceMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 36) && "Invalid size of the ITCH message type 'C'");
    if (size != 36)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderReferenceNumber);
    data += CppCommon::Endian::ReadBigEndian(data, this->ExecutedShares);
    data += CppCommon::Endian::ReadBigEndian(data, this->MatchNumber);
    this->Printable = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->ExecutionPrice);

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const OrderExecutedWithPriceMessage& message)
{
    stream << "OrderExecutedWithPriceMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; OrderReferenceNumber=" << message.OrderReferenceNumber
        << "; ExecutedShares=" << message.ExecutedShares
        << "; MatchNumber=" << message.MatchNumber
        << "; Printable=" << CppCommon::WriteChar(message.Printable)
        << "; ExecutionPrice=" << message.ExecutionPrice
        << ")";
    return stream;
}

inline size_t OrderCancelMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 23) && "Invalid size of the ITCH message type 'X'");
    if (size != 23)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderReferenceNumber);
    data += CppCommon::Endian::WriteBigEndian(data, this->CanceledShares);

    return size;
}

inline bool OrderCancelMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 23) && "Invalid size of the ITCH message type 'X'");
    if (size != 23)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderReferenceNumber);
    data += CppCommon::Endian::ReadBigEndian(data, this->CanceledShares);

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const OrderCancelMessage& message)
{
    stream << "OrderCancelMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; OrderReferenceNumber=" << message.OrderReferenceNumber
        << "; CanceledShares=" << message.CanceledShares
        << ")";
    return stream;
}

inline size_t OrderDeleteMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 19) && "Invalid size of the ITCH message type 'D'");
    if (size != 19)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderReferenceNumber);

    return size;
}

inline bool OrderDeleteMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 19) && "Invalid size of the ITCH message type 'D'");
    if (size != 19)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderReferenceNumber);

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const OrderDeleteMessage& message)
{
    stream << "OrderDeleteMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; OrderReferenceNumber=" << message.OrderReferenceNumber
        << ")";
    return stream;
}

inline size_t OrderReplaceMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 35) && "Invalid size of the ITCH message type 'U'");
    if (size != 35)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OriginalOrderReferenceNumber);
    data += CppCommon::Endian::WriteBigEndian(data, this->NewOrderReferenceNumber);
    data += CppCommon::Endian::WriteBigEndian(data, this->Shares);
    data += CppCommon::Endian::WriteBigEndian(data, this->Price);

    return size;
}

inline bool OrderReplaceMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 35) && "Invalid size of the ITCH message type 'U'");
    if (size != 35)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OriginalOrderReferenceNumber);
    data += CppCommon::Endian::ReadBigEndian(data, this->NewOrderReferenceNumber);
    data += CppCommon::Endian::ReadBigEndian(data, this->Shares);
    data += CppCommon::Endian::ReadBigEndian(data, this->Price);

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const OrderReplaceMessage& message)
{
    stream << "OrderReplaceMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; OriginalOrderReferenceNumber=" << message.OriginalOrderReferenceNumber
        << "; NewOrderReferenceNumber=" << message.NewOrderReferenceNumber
        << "; Shares=" << message.Shares
        << "; Price=" << message.Price
        << ")";
    return stream;
}

inline size_t TradeMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 44) && "Invalid size of the ITCH message type 'P'");
    if (size != 44)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->OrderReferenceNumber);
    *data++ = this->BuySellIndicator;
    data += CppCommon::Endian::WriteBigEndian(data, this->Shares);
    data += WriteString(data, this->Stock);
    data += CppCommon::Endian::WriteBigEndian(data, this->Price);
    data += CppCommon::Endian::WriteBigEndian(data, this->MatchNumber);

    return size;
}

inline bool TradeMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 44) && "Invalid size of the ITCH message type 'P'");
    if (size != 44)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->OrderReferenceNumber);
    this->BuySellIndicator = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->Shares);
    data += ReadString(data, this->Stock);
    data += CppCommon::Endian::ReadBigEndian(data, this->Price);
    data += CppCommon::Endian::ReadBigEndian(data, this->MatchNumber);

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const TradeMessage& message)
{
    stream << "TradeMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; OrderReferenceNumber=" << message.OrderReferenceNumber
        << "; BuySellIndicator=" << CppCommon::WriteChar(message.BuySellIndicator)
        << "; Shares=" << message.Shares
        << "; Stock=" << CppCommon::WriteString(message.Stock)
        << "; Price=" << message.Price
        << "; MatchNumber=" << message.MatchNumber
        << ")";
    return stream;
}

inline size_t CrossTradeMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 40) && "Invalid size of the ITCH message type 'Q'");
    if (size != 40)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->Shares);
    data += WriteString(data, this->Stock);
    data += CppCommon::Endian::WriteBigEndian(data, this->CrossPrice);
    data += CppCommon::Endian::WriteBigEndian(data, this->MatchNumber);
    *data++ = this->CrossType;
    
    return size;
}

inline bool CrossTradeMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 40) && "Invalid size of the ITCH message type 'Q'");
    if (size != 40)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->Shares);
    data += ReadString(data, this->Stock);
    data += CppCommon::Endian::ReadBigEndian(data, this->CrossPrice);
    data += CppCommon::Endian::ReadBigEndian(data, this->MatchNumber);
    this->CrossType = *data++;

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const CrossTradeMessage& message)
{
    stream << "CrossTradeMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; Shares=" << message.Shares
        << "; Stock=" << CppCommon::WriteString(message.Stock)
        << "; CrossPrice=" << message.CrossPrice
        << "; MatchNumber=" << message.MatchNumber
        << "; CrossType=" << CppCommon::WriteChar(message.CrossType)
        << ")";
    return stream;
}

inline size_t BrokenTradeMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 19) && "Invalid size of the ITCH message type 'B'");
    if (size != 19)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->MatchNumber);

    return size;
}

inline bool BrokenTradeMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 19) && "Invalid size of the ITCH message type 'B'");
    if (size != 19)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->MatchNumber);

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const BrokenTradeMessage& message)
{
    stream << "BrokenTradeMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; MatchNumber=" << message.MatchNumber
        << ")";
    return stream;
}

inline size_t NOIIMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 50) && "Invalid size of the ITCH message type 'I'");
    if (size != 50)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::WriteBigEndian(data, this->PairedShares);
    data += CppCommon::Endian::WriteBigEndian(data, this->ImbalanceShares);
    *data++ = this->ImbalanceDirection;
    data += WriteString(data, this->Stock);
    data += CppCommon::Endian::WriteBigEndian(data, this->FarPrice);
    data += CppCommon::Endian::WriteBigEndian(data, this->NearPrice);
    data += CppCommon::Endian::WriteBigEndian(data, this->CurrentReferencePrice);
    *data++ = this->CrossType;
    *data++ = this->PriceVariationIndicator;

    return size;
}

inline bool NOIIMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 50) && "Invalid size of the ITCH message type 'I'");
    if (size != 50)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += CppCommon::Endian::ReadBigEndian(data, this->PairedShares);
    data += CppCommon::Endian::ReadBigEndian(data, this->ImbalanceShares);
    this->ImbalanceDirection = *data++;
    data += ReadString(data, this->Stock);
    data += CppCommon::Endian::ReadBigEndian(data, this->FarPrice);
    data += CppCommon::Endian::ReadBigEndian(data, this->NearPrice);
    data += CppCommon::Endian::ReadBigEndian(data, this->CurrentReferencePrice);
    this->CrossType = *data++;
    this->PriceVariationIndicator = *data++;

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const NOIIMessage& message)
{
    stream << "NOIIMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; PairedShares=" << message.PairedShares
        << "; ImbalanceShares=" << message.ImbalanceShares
        << "; ImbalanceDirection=" << CppCommon::WriteChar(message.ImbalanceDirection)
        << "; Stock=" << CppCommon::WriteString(message.Stock)
        << "; FarPrice=" << message.FarPrice
        << "; NearPrice=" << message.NearPrice
        << "; CurrentReferencePrice=" << message.CurrentReferencePrice
        << "; CrossType=" << CppCommon::WriteChar(message.CrossType)
        << "; PriceVariationIndicator=" << CppCommon::WriteChar(message.PriceVariationIndicator)
        << ")";
    return stream;
}

inline size_t RPIIMessage::serialize(void *buffer, size_t size) const
{
    assert((size == 20) && "Invalid size of the ITCH message type 'N'");
    if (size != 20)
        return 0;

    uint8_t* data = (uint8_t*)buffer;

    *data++ = this->Type;
    data += CppCommon::Endian::WriteBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::WriteBigEndian(data, this->TrackingNumber);
    data += WriteTimestamp(data, this->Timestamp);
    data += WriteString(data, this->Stock);
    *data++ = this->InterestFlag;

    return size;
}

inline bool RPIIMessage::deserialize(void *buffer, size_t size)
{
    assert((size == 20) && "Invalid size of the ITCH message type 'N'");
    if (size != 20)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    this->Type = *data++;
    data += CppCommon::Endian::ReadBigEndian(data, this->StockLocate);
    data += CppCommon::Endian::ReadBigEndian(data, this->TrackingNumber);
    data += ReadTimestamp(data, this->Timestamp);
    data += ReadString(data, this->Stock);
    this->InterestFlag = *data++;

    return true;
}

template <class TOutputStream>
inline TOutputStream& operator<<(TOutputStream& stream, const RPIIMessage& message)
{
    stream << "RPIIMessage(Type=" << CppCommon::WriteChar(message.Type)
        << "; StockLocate=" << message.StockLocate
        << "; TrackingNumber=" << message.TrackingNumber
        << "; Timestamp=" << message.Timestamp
        << "; Stock=" << CppCommon::WriteString(message.Stock)
        << "; InterestFlag=" << CppCommon::WriteChar(message.InterestFlag)
        << ")";
    return stream;
}

inline size_t UnknownMessage::serialize(void *buffer, size_t size) const
{
    assert((size > 0) && "Invalid size of the unknown ITCH message!");
    if (size == 0)
        return false;

    uint8_t* data = (uint8_t*)buffer;

    *data = this->Type;

    return size;
}

inline bool UnknownMessage::deserialize(void *buffer, size_t size)
{
    assert((size > 0) && "Invalid size of the unknown ITCH message!");
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

} // namespace ITCH
} // namespace TradingPlatform
