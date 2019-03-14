package common

import (
	"time"

	"github.com/gorilla/websocket"
)

const (
	// Time allowed to write a message to the peer.
	writeWait = 10 * time.Second
	// Time allowed to read the next pong message from the peer.
	pongWait = 60 * time.Second
	// Send pings to peer with this period. Must be less than pongWait.
	pingPeriod = 10 * time.Second
	// Websocket connection input buffer size in bytes.
	readBufferSize = 1024
	// Websocket connection output buffer size in bytes.
	writeBufferSize = 1024
)

// Listener is a middleman between the websocket connection and the hub.
type Listener struct {
	// The websocket hub.
	Hub *Hub
	// The websocket connection.
	Conn *websocket.Conn
	// Buffered channel of outbound messages.
	Send chan []byte
}

// ReadPump pumps messages from the websocket connection to the hub.
//
// The application runs readPump in a per-connection goroutine. The application
// ensures that there is at most one reader on a connection by executing all
// reads from this goroutine.
func (listener *Listener) ReadPump() {
	defer func() {
		listener.Hub.Unregister <- listener
		listener.Conn.Close()
	}()
	listener.Conn.SetReadDeadline(time.Now().Add(pongWait))
	listener.Conn.SetPongHandler(func(string) error {
		return listener.Conn.SetReadDeadline(time.Now().Add(pongWait))
	})
	for {
		messageType, message, err := listener.Conn.ReadMessage()
		if err != nil {
			if IsConnectionClosedUnexpectedly(err) {
				listener.Hub.logger.Errorf("[ERROR] Unable to read message from websocket: %s\n", err.Error())
			}
			break
		}
		if messageType == websocket.BinaryMessage {
			listener.Hub.Broadcast <- message
		}
	}
}

// WritePump pumps messages from the hub to the websocket connection.
//
// A goroutine running writePump is started for each connection. The
// application ensures that there is at most one writer to a connection by
// executing all writes from this goroutine.
func (listener *Listener) WritePump() {
	ticker := time.NewTicker(pingPeriod)
	defer func() {
		ticker.Stop()
		listener.Conn.Close()
	}()
	for {
		select {
		case message, ok := <-listener.Send:
			listener.Conn.SetWriteDeadline(time.Now().Add(writeWait))
			if !ok {
				// The hub closed the channel.
				err := listener.Conn.WriteMessage(websocket.CloseMessage, []byte{})
				if err != nil {
					if IsConnectionClosedUnexpectedly(err) {
						listener.Hub.logger.Errorf("[ERROR] Unable to read message from websocket: %s\n", err.Error())
					} else {
						listener.Hub.logger.Info("Websocket connection is closed\n")
					}
				}
				return
			}

			writer, err := listener.Conn.NextWriter(websocket.BinaryMessage)
			if err != nil {
				listener.Hub.logger.Errorf("[ERROR] Unable to create websocket writer: %s\n", err.Error())
				return
			}

			// Add queued messages to the current websocket message.
			_, err = writer.Write(message)
			if err != nil {
				if IsConnectionClosedUnexpectedly(err) {
					listener.Hub.logger.Errorf("[ERROR] Unable to write message to websocket writer: %s\n", err.Error())
				} else {
					listener.Hub.logger.Info("Websocket connection is closed\n")
				}
			}
			n := len(listener.Send)
			for i := 0; i < n; i++ {
				_, err = writer.Write(<-listener.Send)
				if err != nil {
					if IsConnectionClosedUnexpectedly(err) {
						listener.Hub.logger.Errorf("[ERROR] Unable to write message to websocket writer: %s\n", err.Error())
					} else {
						listener.Hub.logger.Info("Websocket connection is closed\n")
					}
				}
			}

			err = writer.Close()
			if err != nil {
				listener.Hub.logger.Errorf("[ERROR] Unable to close websocket writer: %s\n", err.Error())
				return
			}
		case <-ticker.C:
			listener.Conn.SetWriteDeadline(time.Now().Add(writeWait))
			err := listener.Conn.WriteMessage(websocket.PingMessage, nil)
			if err != nil {
				if IsConnectionClosedUnexpectedly(err) {
					listener.Hub.logger.Errorf("[ERROR] Unable to write message to websocket: %s\n", err.Error())
				} else {
					listener.Hub.logger.Info("Websocket connection is closed\n")
				}
				return
			}
		}
	}
}
