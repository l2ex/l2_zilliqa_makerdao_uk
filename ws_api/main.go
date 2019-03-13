package main

import (
	"./InterExchange"
	"flag"
	"fmt"
	"github.com/gorilla/websocket"
	"github.com/lirm/aeron-go/aeron"
	"github.com/lirm/aeron-go/aeron/atomic"
	"github.com/lirm/aeron-go/aeron/idlestrategy"
	"github.com/lirm/aeron-go/aeron/logbuffer"
	"log"
	"net/http"
	"time"
)

var Addr = flag.String("addr", "localhost:2054", "http service address")

var upgrader = websocket.Upgrader{} // use default options

var sub = InterExchange.SUB{}
var pub = InterExchange.PUB{}

func apiStream(w http.ResponseWriter, r *http.Request) {
	fmt.Println("creating to %s", *Addr)
	c, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Print("upgrade:", err)
		return
	}
	defer c.Close()
	for {
		mt , message, err := c.ReadMessage()
		if err != nil {
			log.Println("read:", err)
			break
		}

		switch message[0] {
		// place order
		case 'O':
			go push2ME(message)

		}

		go push2WS(*c , mt)

	}
}

func main() {
	sub.Connect()
	pub.Connect()


	flag.Parse()
	log.SetFlags(0)
	http.HandleFunc("/", apiStream)
	log.Fatal(http.ListenAndServe(*Addr, nil))
}

func push2ME(msg []byte)  {
	ret := pub.Publication.Offer(pub.Buffer, 0, int32(len(msg)), nil)
	switch ret {
	case aeron.NotConnected:
		log.Printf("not connected yet")
	case aeron.BackPressured:
		log.Printf("back pressured")
	default:
		if ret < 0 {
			log.Printf("Unrecognized code: %d", ret)
		} else {
			log.Printf("success!")
		}
	}
	if !pub.Publication.IsConnected() {
		log.Printf("no subscribers detected")
	}
}

func push2WS(c websocket.Conn, mt int) {

		subscription := <-sub.Transport.AddSubscription(*InterExchange.Config.OrderBookChannel, int32(*InterExchange.Config.OrderBookStreamID))
		defer subscription.Close()
		log.Printf("Subscription found %v", subscription)

		counter := 1
		handler := func(buffer *atomic.Buffer, offset int32, length int32, header *logbuffer.Header) {
			bytes := buffer.GetBytesArray(offset, length)
			//fmt.Printf("%8.d: Gots me a fragment offset:%d length: %d payload: %s\n", counter, offset, length, string(bytes))
			sign( bytes )
			err := c.WriteMessage( mt, bytes)
			if err != nil {
				log.Println("feed:", err)
			}
			counter++
		}

		idleStrategy := idlestrategy.Sleeping{SleepFor: time.Millisecond}

		for {
			fragmentsRead := subscription.Poll(handler, 10)
			idleStrategy.Idle(fragmentsRead)
		}

		sub.Disconnect()

		}

func sign(feed []byte) {
	//inspect what here and how sign?
	t := feed[1]
	switch t {
	case 'S':
		log.Printf("ProcessSystemEventMessage")
	case 'R':
		log.Printf( "ProcessStockDirectoryMessage")
	case 'H':
		log.Printf( "ProcessStockTradingActionMessage")
	case 'Y':
		log.Printf( "ProcessRegSHOMessage")
	case 'L':
		log.Printf( "ProcessMarketParticipantPositionMessage")
	case 'V':
		log.Printf( "ProcessMWCBDeclineMessage")
	case 'W':
		log.Printf( "ProcessMWCBStatusMessage")
	case 'K':
		log.Printf( "ProcessIPOQuotingMessage")
	case 'A':
		log.Printf( "ProcessAddOrderMessage")
	case 'F':
		log.Printf( "ProcessAddOrderMPIDMessage")
	case 'E':
		log.Printf( "ProcessOrderExecutedMessage")
	case 'C':
		log.Printf( "ProcessOrderExecutedWithPriceMessage")
	case 'X':
		log.Printf( "ProcessOrderCancelMessage")
	case 'D':
		log.Printf( "ProcessOrderDeleteMessage")
	case 'U':
		log.Printf( "ProcessOrderReplaceMessage")
	case 'P':
		log.Printf( "ProcessTradeMessage")
	case 'Q':
		log.Printf( "ProcessCrossTradeMessage")
	case 'B':
		log.Printf( "ProcessBrokenTradeMessage")
	case 'I':
		log.Printf( "ProcessNOIIMessage")
	case 'N':
		log.Printf( "ProcessRPIIMessage")
	default:
		log.Printf( "ProcessUnknownMessage")

	}


}