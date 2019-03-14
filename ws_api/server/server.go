package main

import (
	"flag"
	"log"
	"net/http"
	"time"

	"github.com/gorilla/websocket"
	"github.com/lirm/aeron-go/aeron"
	"github.com/lirm/aeron-go/aeron/atomic"
	"github.com/lirm/aeron-go/aeron/idlestrategy"
	"github.com/lirm/aeron-go/aeron/logbuffer"

	transport "../aeron"
)

// Web socket related stuff
var wsEndpoint = flag.String("addr", "0.0.0.0:2054", "web socket address")
var wsReadBufferSize = 1024
var wsWriteBufferSize = 1024

// Internal transport over Aeron protocol
var aeronSub = transport.Subscriber{}
var aeronPub = transport.Publisher{}

func main() {
	flag.Parse()
	log.SetFlags(0)

	aeronSub.Connect()
	aeronPub.Connect()
	defer aeronSub.Disconnect()
	defer aeronPub.Disconnect()

	http.HandleFunc("/", httpHandler)
	log.Fatal(http.ListenAndServe(*wsEndpoint, nil))
}

func httpHandler(w http.ResponseWriter, r *http.Request) {

	log.Println("[WS] Creating web socket at", *wsEndpoint, "...")

	c, err := websocket.Upgrade(w, r, nil, wsReadBufferSize, wsWriteBufferSize)
	if err != nil {
		log.Println("[WS] Failed to connect to web socket:", err)
		return
	}
	defer c.Close()

	go pullFromAeron(c)

	for {
		_, message, err := c.ReadMessage()
		if err != nil {
			log.Println("[WS] Failed to read message from web socket:", err)
			return
		}

		if len(message) > 2 {
			switch message[2] {
			case 'O':
				// Make order
				pushToAeron(message)
			}
		}
	}
}

func pushToAeron(message []byte) {
	// if !aeronPub.Publication.IsConnected() {
	// 	log.Println("[AERON] No subscribers detected")
	// 	return
	// }
	copy(aeronPub.Buffer, message)
	result := aeronPub.Publication.Offer(aeronPub.BufferAtomic, 0, int32(len(message)), nil)
	switch result {
	case aeron.NotConnected:
		log.Println("[AERON] Not connected yet")
	case aeron.BackPressured:
		log.Println("[AERON] Back pressured")
	default:
		if result < 0 {
			log.Printf("[AERON] Unrecognized result code: %d\n", result)
		} else {
			log.Println("[AERON] Publishing message successful!")
		}
	}
}

func pullFromAeron(c *websocket.Conn) {
	idleStrategy := idlestrategy.Sleeping{SleepFor: time.Millisecond}
	handler := func(buffer *atomic.Buffer, offset int32, length int32, header *logbuffer.Header) {
		message := buffer.GetBytesArray(offset, length)
		if len(message) > 2 {
			switch message[2] {
			case 'A':
				// Accepted order
				log.Println("[AERON] Received \"order accepted\" message!")
			case 'J':
				// Executed order
				log.Println("[AERON] Received \"order rejected\" message!")
			case 'E':
				// Executed order
				log.Println("[AERON] Received \"order executed\" message!")
			}
		}
		err := c.WriteMessage(websocket.BinaryMessage, message)
		if err != nil {
			log.Println("[WS] Failed to send message:", err)
		}
	}
	for {
		fragmentsRead := aeronSub.Subscription.Poll(handler, 10)
		idleStrategy.Idle(fragmentsRead)
	}
}
