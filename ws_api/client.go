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

var server = flag.String("addr", "localhost:2054", "http service address")

func main() {
	flag.Parse()
	log.SetFlags(0)

	interrupt := make(chan os.Signal, 1)
	signal.Notify(interrupt, os.Interrupt)

	u := url.URL{Scheme: "ws", Host: *server, Path: "/"}
	log.Printf("connecting to %s", u.String())

	c, _, err := websocket.DefaultDialer.Dial(u.String(), nil)
	if err != nil {
		log.Fatal("dial:", err)
	}
	defer c.Close()


	log.Printf("60 second demo")
	log.Printf("Starting trading betwen 3 actors : Alice, Bob and MarketMaker")
	log.Printf("Ethereum Rinkebuy channel contract: ")
	log.Printf("Zilica testnet channel contract: " )
	log.Printf("Initial Alice balance: ")
	log.Printf("Initial Bob balance: " )


	//trading demo
	go Alice(*c)
	go Bob(*c)
	go MarketMaker(*c)
	go OrderLog(*c)

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
				log.Println("write close:", err)
				return
			}
		case t := <-ticker.C:
			err := c.WriteMessage(websocket.TextMessage, []byte(t.String()))
			if err != nil {
				log.Println("write:", err)
				return
			}
		case <-interrupt:
			log.Println("interrupt")

			// Cleanly close the connection by sending a close message and then
			// waiting (with timeout) for the server to close the connection.
			err := c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, ""))
			if err != nil {
				log.Println("write close:", err)
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

func Alice(c websocket.Conn)  {
	Msg := "OAlice Order"
	c.WriteMessage(websocket.TextMessage, []byte(Msg))
	// TODO
	// open data file aliceorders.ouch
	// send all orders with random delay


}

func Bob(c websocket.Conn) {
	Msg := "OBob Order"
	c.WriteMessage(websocket.TextMessage, []byte(Msg))
	// TODO
	// open data file bobrders.ouch
	// send with random delay

}

func MarketMaker(c websocket.Conn)  {
	Msg := "OMarket maker order"
	c.WriteMessage(websocket.TextMessage, []byte(Msg))
	// post orders buy
	// post orders sell
	// send 60 times
}

func OrderLog(c websocket.Conn) {
//	go func() {

		for {
			_, message, err := c.ReadMessage()
			if err != nil {
				log.Println("read:", err)
				return
			}
			// TODO
			// create ordersLog.csv
			// save all
			log.Printf("recv: %s", message)
		}
//	}()
}

