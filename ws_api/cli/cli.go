package main

import (
	"encoding/binary"
	"flag"
	"io/ioutil"
	"log"
	"math"
	"net/url"
	"os"
	"strconv"

	"github.com/gorilla/websocket"

	"../messages"
)

// Web socket related stuff
var wsEndpoint = flag.String("addr", "localhost:2054", "web socket address")
var wsURL = url.URL{Scheme: "ws", Host: *wsEndpoint, Path: "/"}

// Commands
var cliCommand = os.Args[1]   // "makeOrder"
var cliAccount = os.Args[2]   // "alice", "bob", "charlie"
var cliSymbol = os.Args[3]    // "ZIL_DAI", "ETH_DAI", "ZIL_ETH"
var cliOrderType = os.Args[4] // "limit", "market"
var cliOrderSide = os.Args[5] // "buy", "sell"
var cliOrderQuantity = os.Args[6]
var cliOrderPrice = os.Args[7]

func main() {

	var err error

	message := messages.EnterOrderMessage{}

	switch cliCommand {
	case "makeOrder":
	default:
		log.Println("[ERROR] Unsupported command specified:", cliCommand)
		return
	}

	switch cliAccount {
	case "alice":
		message.AccountID = 1
	case "bob":
		message.AccountID = 2
	case "charlie":
		message.AccountID = 3
	default:
		log.Println("[ERROR] Unsupported account specified:", cliAccount)
		return
	}

	switch cliSymbol {
	case "ZIL_DAI":
		message.OrderBookID = messages.OrderBookID_ZIL_DAI
	case "ETH_DAI":
		message.OrderBookID = messages.OrderBookID_ETH_DAI
	case "ZIL_ETH":
		message.OrderBookID = messages.OrderBookID_ZIL_ETH
	default:
		log.Println("[ERROR] Unsupported symbol specified:", cliSymbol)
		return
	}

	switch cliOrderType {
	case "limit":
		message.OrderType = messages.OrderTypeLimit
	case "market":
		message.OrderType = messages.OrderTypeMarket
	default:
		log.Println("[ERROR] Unsupported order type specified:", cliOrderType)
		return
	}

	switch cliOrderSide {
	case "buy":
		message.OrderSide = messages.OrderSideBuy
	case "sell":
		message.OrderSide = messages.OrderSideSell
	default:
		log.Println("[ERROR] Unsupported order side specified:", cliOrderSide)
		return
	}

	message.Quantity, err = strconv.ParseUint(cliOrderQuantity, 10, 64)
	if err != nil {
		log.Println("[ERROR] Invalid order quantity specified:", cliOrderQuantity)
		return
	}

	if message.OrderType == messages.OrderTypeLimit {
		price, err := strconv.ParseFloat(cliOrderPrice, 64)
		if err != nil {
			log.Println("[ERROR] Invalid order price specified:", cliOrderPrice)
			return
		}
		message.Price = uint32(math.Round(price * 1000))
	} else {
		message.Price = 0x7FFFFFFF
	}

	// TODO: Improve it somehow
	var orderID uint32 = 1
	lastOrderIDBytes, _ := ioutil.ReadFile("order_counter")
	lastOrderID, _ := strconv.ParseUint(string(lastOrderIDBytes), 10, 32)
	if lastOrderID > 0 {
		orderID = uint32(lastOrderID) + 1
	}
	ioutil.WriteFile("order_counter", ([]byte)(strconv.FormatUint(uint64(orderID), 10)), 0644)
	message.OrderID = orderID

	c, _, err := websocket.DefaultDialer.Dial(wsURL.String(), nil)
	if err != nil {
		log.Println("[WS] Failed to connect to web socket:", err)
		return
	}
	defer c.Close()

	messageBytes, err := message.Serialize()
	if err != nil {
		log.Println("[ERROR] Unable to serialize message:", message)
		return
	}

	messagePrefixed := make([]byte, len(messageBytes)+2)
	binary.BigEndian.PutUint16(messagePrefixed[:2], uint16(len(messageBytes)))
	copy(messagePrefixed[2:], messageBytes)

	err = c.WriteMessage(websocket.BinaryMessage, messagePrefixed)
	if err != nil {
		log.Println("[WS] Failed to write close message to web socket:", err)
		return
	}
}
