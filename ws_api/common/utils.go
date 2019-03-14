package common

import (
	"github.com/gorilla/websocket"
	"github.com/op/go-logging"
)

// IsConnectionClosedUnexpectedly returns true if websocket connection is closed unexpectedly.
func IsConnectionClosedUnexpectedly(err error) bool {
	return websocket.IsUnexpectedCloseError(
		err,
		websocket.CloseGoingAway,
		websocket.CloseNormalClosure,
		websocket.CloseNoStatusReceived,
	)
}

// CloseConnection closes websocket connection carefully (should be used only by client).
func CloseConnection(conn *websocket.Conn, logger *logging.Logger) {
	err := conn.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, ""))
	if err != nil {
		if logger != nil {
			if IsConnectionClosedUnexpectedly(err) {
				logger.Errorf("[ERROR] Unable to write close message to websocket: %s\n", err.Error())
			} else {
				logger.Info("Websocket connection is closed\n")
			}
		}
		return
	}

	for {
		messageType, _, err := conn.ReadMessage()
		if err != nil {
			if logger != nil {
				if IsConnectionClosedUnexpectedly(err) {
					logger.Errorf("[ERROR] Unable to read message from websocket: %s\n", err.Error())
				} else {
					logger.Info("Websocket connection is closed\n")
				}
			}
			return
		}
		switch messageType {
		case websocket.CloseMessage:
			return
		}
	}
}
