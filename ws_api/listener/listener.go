package main

import (
	"flag"
	"net/url"
	"os"
	"os/signal"

	"github.com/gorilla/websocket"
	"github.com/op/go-logging"

	"../common"
)

var logger = logging.MustGetLogger("ws-listener")

// Web socket related stuff
var wsEndpoint = flag.String("addr", "localhost:2054", "websocket address")
var wsURL = url.URL{Scheme: "ws", Host: *wsEndpoint, Path: "/subscribe"}

func main() {
	interrupt := make(chan os.Signal, 1)
	signal.Notify(interrupt, os.Interrupt)

	logger.Infof("Connecting to websocket at address %s...\n", wsURL.String())

	conn, _, err := websocket.DefaultDialer.Dial(wsURL.String(), nil)
	if err != nil {
		logger.Errorf("[ERROR] Unable to connect to websocket: %s\n", err.Error())
		return
	}
	defer conn.Close()

	for {
		select {
		case <-interrupt:
			logger.Infof("Interrupt signal handled\n")
			common.CloseConnection(conn, logger)
			return
		}
	}
}
