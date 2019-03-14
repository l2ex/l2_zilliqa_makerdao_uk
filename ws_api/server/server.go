package main

import (
	"encoding/binary"
	"flag"
	"net/http"

	"github.com/gorilla/websocket"
	"github.com/lirm/aeron-go/aeron"
	"github.com/op/go-logging"

	transport "../aeron"
	"../common"
	"../messages"
)

var logger = logging.MustGetLogger("ws-server")

// Web socket related stuff
var wsEndpoint = flag.String("addr", "0.0.0.0:2054", "websocket address")
var wsReadBufferSize = 1024
var wsWriteBufferSize = 1024

var hub *common.Hub
var publisher = &transport.Publisher{}

func main() {
	flag.Parse()

	hub = common.NewHub()
	go hub.Run()

	publisher.Connect()
	defer publisher.Disconnect()

	http.HandleFunc("/", wsHandler)
	http.HandleFunc("/subscribe", func(w http.ResponseWriter, r *http.Request) {
		wsListen(hub, w, r)
	})
	err := http.ListenAndServe(*wsEndpoint, nil)
	if err != nil {
		logger.Errorf("[ERROR] ListenAndServe failed with error: %s\n", err.Error())
	}
}

func wsHandler(w http.ResponseWriter, r *http.Request) {

	logger.Infof("Handling incoming websocket connection from %s...\n", r.RemoteAddr)

	conn, err := websocket.Upgrade(w, r, nil, wsReadBufferSize, wsWriteBufferSize)
	if err != nil {
		logger.Errorf("[ERROR] Unable to set up websocket connection from %s: %s\n", r.RemoteAddr, err.Error())
		return
	}

	for {
		messageType, message, err := conn.ReadMessage()
		if err != nil {
			if common.IsConnectionClosedUnexpectedly(err) {
				logger.Errorf("[ERROR] Unable to read message from websocket: %s\n", err.Error())
			} else {
				logger.Infof("Websocket connection from %s is closed\n", r.RemoteAddr)
			}
			return
		}
		switch messageType {
		case websocket.BinaryMessage:
			if len(message) > 0 {
				switch message[0] {
				case 'O':
					// Enter order message
					m := &messages.EnterOrderMessage{}
					err = m.Deserialize(message)
					if err != nil {
						logger.Errorf("[ERROR] Unable to deserialize received message EnterOrderMessage = %#x\n", message)
						break
					}
					logger.Infof("[WebSocket] Message received: EnterOrderMessage = %+v\n", m)
					sendToAeron(message)
				}
			}
		}
	}
}

func wsListen(hub *common.Hub, w http.ResponseWriter, r *http.Request) {

	logger.Infof("Handling incoming websocket subscribe connection from %s...\n", r.RemoteAddr)

	conn, err := websocket.Upgrade(w, r, nil, wsReadBufferSize, wsWriteBufferSize)
	if err != nil {
		logger.Errorf("[ERROR] Unable to set up websocket subscribe connection from %s: %s\n", r.RemoteAddr, err.Error())
		return
	}

	listener := &common.Listener{Hub: hub, Conn: conn, Send: make(chan []byte, 128)}
	listener.Hub.Register <- listener

	// Allow collection of memory referenced by the caller by doing all work in new goroutines
	go listener.WritePump()
	go listener.ReadPump()
}

func sendToAeron(message []byte) {
	binary.BigEndian.PutUint16(publisher.Buffer[:2], uint16(len(message)))
	copy(publisher.Buffer[2:], message)
	result := publisher.Publication.Offer(publisher.BufferAtomic, 0, int32(len(message)+2), nil)
	switch result {
	case aeron.NotConnected:
		logger.Error("[AERON] Publication failed: not connected yet\n")
	case aeron.BackPressured:
		logger.Error("[AERON] Publication failed: back pressured\n")
	default:
		if result < 0 {
			logger.Errorf("[AERON] Publication failed: unrecognized result code: %d\n", result)
		} else {
			logger.Error("[AERON] Publication successful!")
		}
	}
}
