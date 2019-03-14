package messages

import (
	"encoding/binary"
)

// OrderBookID is identifier of order book.
type OrderBookID uint32

const (
	OrderBookID_ZIL_DAI OrderBookID = 777
	OrderBookID_ETH_DAI OrderBookID = 778
	OrderBookID_ZIL_ETH OrderBookID = 779
)

// OrderType is type of order (limit/market).
type OrderType byte

const (
	OrderTypeLimit  OrderType = 'L'
	OrderTypeMarket OrderType = 'M'
)

// OrderSide is side of order (buy/sell).
type OrderSide byte

const (
	OrderSideBuy  OrderSide = 'B'
	OrderSideSell OrderSide = 'S'
)

// MakeOrderRequest contains make order request information.
type MakeOrderRequest struct {
	AccountID   uint32
	OrderBookID OrderBookID
	OrderID     uint32
	OrderType   OrderType
	OrderSide   OrderSide
	Quantity    uint64
	Price       uint32
}

func (message *MakeOrderRequest) Serialize() ([]byte, error) {
	result := make([]byte, 43)
	result[0] = 'O'                                                        // type
	binary.BigEndian.PutUint32(result[1:5], uint32(message.OrderID))       // order ID
	result[5] = 'C'                                                        // account type
	binary.BigEndian.PutUint32(result[6:10], uint32(message.AccountID))    // account ID
	result[10] = byte(message.OrderSide)                                   // order side (buy/sell)
	binary.BigEndian.PutUint64(result[11:19], message.Quantity)            // quantity
	binary.BigEndian.PutUint32(result[19:23], uint32(message.OrderBookID)) // orderbook ID
	binary.BigEndian.PutUint32(result[23:27], message.Price)               // price
	binary.BigEndian.PutUint32(result[27:31], 0)                           // time in force
	binary.BigEndian.PutUint32(result[31:35], 0)                           // client ID
	binary.BigEndian.PutUint64(result[35:43], 0)                           // minimum quantity
	return result, nil
}
