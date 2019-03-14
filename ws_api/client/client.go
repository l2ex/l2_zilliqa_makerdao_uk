package main

import (
	"flag"
	"net/url"
	"os"
	"os/signal"
	"time"

	"github.com/gorilla/websocket"
	"github.com/op/go-logging"

	"../common"
)

var logger = logging.MustGetLogger("ws-client")

// Web socket related stuff
var wsEndpoint = flag.String("addr", "localhost:2054", "websocket address")
var wsURL = url.URL{Scheme: "ws", Host: *wsEndpoint, Path: "/"}

func main() {
	flag.Parse()

	interrupt := make(chan os.Signal, 1)
	signal.Notify(interrupt, os.Interrupt)

	logger.Infof("Connecting to websocket at address %s...\n", wsURL.String())

	conn, _, err := websocket.DefaultDialer.Dial(wsURL.String(), nil)
	if err != nil {
		logger.Errorf("[ERROR] Unable to connect to websocket: %s\n", err.Error())
		return
	}
	defer conn.Close()

	logger.Infof("60 second demo\n")
	logger.Infof("Starting trading betwen 3 actors : Alice, Bob and MarketMaker\n")
	logger.Infof("Ethereum Rinkeby channel contract: 0x59848c104012cE77cca6BAd9EF16d7278a1c112d\n")
	logger.Infof("Zilliqa testnet channel contract: \n")
	logger.Infof("Initial Alice balance: \n")
	logger.Infof("Initial Bob balance: \n")

	// Trading demo
	go alice(conn)
	go bob(conn)
	go marketMaker(conn)
	go orderLog(conn)

	ticker := time.NewTicker(time.Second)
	defer ticker.Stop()

	for {
		select {
		case <-time.After(60 * time.Second):
			common.CloseConnection(conn, logger)
			return
		case t := <-ticker.C:
			err := conn.WriteMessage(websocket.TextMessage, []byte(t.String()))
			if err != nil {
				logger.Errorf("[ERROR] Unable to write tick time message to websocket: %s\n", err.Error())
				return
			}
		case <-interrupt:
			logger.Infof("Interrupt signal handled\n")
			common.CloseConnection(conn, logger)
			return
		}
	}
}

func alice(conn *websocket.Conn) {
	// TODO
	// open data file aliceorders.ouch
	// send all orders with random delay

}

func bob(conn *websocket.Conn) {
	// TODO
	// open data file bobrders.ouch
	// send with random delay

}

func marketMaker(conn *websocket.Conn) {
	// post orders buy
	// post orders sell
	// send 60 times
}

func orderLog(conn *websocket.Conn) {
	for {
		_, message, err := conn.ReadMessage()
		if err != nil {
			logger.Errorf("[ERROR] Unable to read message from websocket: %s\n", err.Error())
			return
		}
		// TODO
		// create ordersLog.csv
		// save all
		logger.Infof("[WebSocket] Message received: %#x\n", message)
	}
}
