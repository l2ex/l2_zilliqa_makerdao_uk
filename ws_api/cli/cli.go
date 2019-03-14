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
var wsEndpoint = flag.String("addr", "137.116.73.73:2054", "web socket address")
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

	makeOrderRequest := messages.MakeOrderRequest{}

	switch cliCommand {
	case "makeOrder":
	default:
		log.Println("[ERROR] Unknown command specified:", cliCommand)
		return
	}

	switch cliAccount {
	case "alice":
		makeOrderRequest.AccountID = 1
	case "bob":
		makeOrderRequest.AccountID = 2
	case "charlie":
		makeOrderRequest.AccountID = 3
	default:
		log.Println("[ERROR] Unknown account specified:", cliAccount)
		return
	}

	switch cliSymbol {
	case "ZIL_DAI":
		makeOrderRequest.OrderBookID = messages.OrderBookID_ZIL_DAI
	case "ETH_DAI":
		makeOrderRequest.OrderBookID = messages.OrderBookID_ETH_DAI
	case "ZIL_ETH":
		makeOrderRequest.OrderBookID = messages.OrderBookID_ZIL_ETH
	default:
		log.Println("[ERROR] Unknown symbol specified:", cliSymbol)
		return
	}

	switch cliOrderType {
	case "limit":
		makeOrderRequest.OrderType = messages.OrderTypeLimit
	case "market":
		makeOrderRequest.OrderType = messages.OrderTypeMarket
	default:
		log.Println("[ERROR] Unknown order type specified:", cliOrderType)
		return
	}

	switch cliOrderSide {
	case "buy":
		makeOrderRequest.OrderSide = messages.OrderSideBuy
	case "sell":
		makeOrderRequest.OrderSide = messages.OrderSideSell
	default:
		log.Println("[ERROR] Unknown order side specified:", cliOrderSide)
		return
	}

	makeOrderRequest.Quantity, err = strconv.ParseUint(cliOrderQuantity, 10, 64)
	if err != nil {
		log.Println("[ERROR] Invalid order quantity specified:", cliOrderQuantity)
		return
	}

	if makeOrderRequest.OrderType == messages.OrderTypeLimit {
		price, err := strconv.ParseFloat(cliOrderPrice, 64)
		if err != nil {
			log.Println("[ERROR] Invalid order price specified:", cliOrderPrice)
			return
		}
		makeOrderRequest.Price = uint32(math.Round(price * 1000))
	} else {
		makeOrderRequest.Price = 0x7FFFFFFF
	}

	// TODO: Improve it somehow
	var orderID uint32 = 1
	lastOrderIDBytes, _ := ioutil.ReadFile("order_counter")
	lastOrderID, _ := strconv.ParseUint(string(lastOrderIDBytes), 10, 32)
	if lastOrderID > 0 {
		orderID = uint32(lastOrderID) + 1
	}
	ioutil.WriteFile("order_counter", ([]byte)(strconv.FormatUint(uint64(orderID), 10)), 0644)
	makeOrderRequest.OrderID = orderID

	serialized, err := makeOrderRequest.Serialize()
	if err != nil {
		log.Println("[ERROR] Unable to serialize message:", makeOrderRequest)
		return
	}

	message := make([]byte, len(serialized)+2)
	binary.BigEndian.PutUint16(message[:2], uint16(len(serialized)))
	copy(message[2:], serialized)

	c, _, err := websocket.DefaultDialer.Dial(wsURL.String(), nil)
	if err != nil {
		log.Println("[WS] Failed to connect to web socket:", err)
		return
	}
	defer c.Close()

	err = c.WriteMessage(websocket.BinaryMessage, message)
	if err != nil {
		log.Println("[WS] Failed to write close message to web socket:", err)
		return
	}
}
