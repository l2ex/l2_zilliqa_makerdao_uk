package main

import (
	"flag"
	"log"
	"net/url"
	"os"
	"os/signal"
	"time"

	"github.com/gorilla/websocket"
)

// Web socket related stuff
var wsEndpoint = flag.String("addr", "localhost:2054", "web socket address")
var wsURL = url.URL{Scheme: "ws", Host: *wsEndpoint, Path: "/"}

func main() {
	flag.Parse()
	log.SetFlags(0)

	interrupt := make(chan os.Signal, 1)
	signal.Notify(interrupt, os.Interrupt)

	log.Println("[WS] Connecting to", wsURL.String(), "...")

	c, _, err := websocket.DefaultDialer.Dial(wsURL.String(), nil)
	if err != nil {
		log.Println("[WS] Failed to connect to web socket:", err)
		return
	}
	defer c.Close()

	log.Println("60 second demo")
	log.Println("Starting trading betwen 3 actors : Alice, Bob and MarketMaker")
	log.Println("Ethereum Rinkeby channel contract: 0x59848c104012cE77cca6BAd9EF16d7278a1c112d")
	log.Println("Zilliqa testnet channel contract: ")
	log.Println("Initial Alice balance: ")
	log.Println("Initial Bob balance: ")

	// Trading demo
	go alice(c)
	go bob(c)
	go marketMaker(c)
	go orderLog(c)

	done := make(chan struct{})
	defer close(done)

	ticker := time.NewTicker(time.Second)
	defer ticker.Stop()

	for {
		select {
		case <-done:
			return
		case <-time.After(60 * time.Second):
			err := c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, ""))
			if err != nil {
				log.Println("[WS] Failed to write close message to web socket:", err)
				return
			}
		case t := <-ticker.C:
			err := c.WriteMessage(websocket.TextMessage, []byte(t.String()))
			if err != nil {
				log.Println("[WS] Failed to write tick time message to web socket:", err)
				return
			}
		case <-interrupt:
			log.Println("[WS] Interrupt signal handled")

			// Cleanly close the connection by sending a close message and then
			// waiting (with timeout) for the server to close the connection.
			err := c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, ""))
			if err != nil {
				log.Println("[WS] Failed to write close message to web socket:", err)
				return
			}
			select {
			case <-done:
			case <-time.After(time.Second):
			}
			return

		}
	}
}

func alice(c *websocket.Conn) {
	// TODO
	// open data file aliceorders.ouch
	// send all orders with random delay

}

func bob(c *websocket.Conn) {
	// TODO
	// open data file bobrders.ouch
	// send with random delay

}

func marketMaker(c *websocket.Conn) {
	// post orders buy
	// post orders sell
	// send 60 times
}

func orderLog(c *websocket.Conn) {
	for {
		_, message, err := c.ReadMessage()
		if err != nil {
			log.Println("[WS] Failed to read message from web socket:", err)
			return
		}
		// TODO
		// create ordersLog.csv
		// save all
		log.Println("[WS] Message received:", message)
	}
}
