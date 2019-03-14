package main

import (
	"flag"
	"io/ioutil"
	"math"
	"net/url"
	"os"
	"strconv"

	"github.com/gorilla/websocket"
	"github.com/op/go-logging"

	"../common"
	"../messages"
)

var logger = logging.MustGetLogger("ws-cli")

// Web socket related stuff
var wsEndpoint = flag.String("addr", "localhost:2054", "websocket address")
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
		logger.Errorf("[ERROR] Unsupported command specified: %s\n", cliCommand)
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
		logger.Errorf("[ERROR] Unsupported account specified: %s\n", cliAccount)
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
		logger.Errorf("[ERROR] Unsupported symbol specified: %s\n", cliSymbol)
		return
	}

	switch cliOrderType {
	case "limit":
		message.OrderType = messages.OrderTypeLimit
	case "market":
		message.OrderType = messages.OrderTypeMarket
	default:
		logger.Errorf("[ERROR] Unsupported order type specified: %s\n", cliOrderType)
		return
	}

	switch cliOrderSide {
	case "buy":
		message.OrderSide = messages.OrderSideBuy
	case "sell":
		message.OrderSide = messages.OrderSideSell
	default:
		logger.Errorf("[ERROR] Unsupported order side specified: %s\n", cliOrderSide)
		return
	}

	message.Quantity, err = strconv.ParseUint(cliOrderQuantity, 10, 64)
	if err != nil {
		logger.Errorf("[ERROR] Invalid order quantity specified: %s\n", cliOrderQuantity)
		return
	}

	if message.OrderType == messages.OrderTypeLimit {
		price, err := strconv.ParseFloat(cliOrderPrice, 64)
		if err != nil {
			logger.Errorf("[ERROR] Invalid order price specified: %s\n", cliOrderPrice)
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

	messageBytes, err := message.Serialize()
	if err != nil {
		logger.Errorf("[ERROR] Unable to serialize message: %s\n", message)
		return
	}

	conn, _, err := websocket.DefaultDialer.Dial(wsURL.String(), nil)
	if err != nil {
		logger.Errorf("[ERROR] Unable to connect to websocket: %s\n", err.Error())
		return
	}
	defer conn.Close()

	err = conn.WriteMessage(websocket.BinaryMessage, messageBytes)
	if err != nil {
		logger.Errorf("[ERROR] Unable to write message to websocket: %s\n", err.Error())
		return
	}

	common.CloseConnection(conn, nil)
}
