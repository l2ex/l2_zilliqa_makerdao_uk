package common

import (
	"time"

	"github.com/lirm/aeron-go/aeron/atomic"
	"github.com/lirm/aeron-go/aeron/idlestrategy"
	"github.com/lirm/aeron-go/aeron/logbuffer"
	"github.com/op/go-logging"

	transport "../aeron"
)

// Hub maintains the set of active listeners and broadcasts messages to the listeners.
type Hub struct {
	// Registered listeners.
	Listeners map[*Listener]bool
	// Register requests from the listeners.
	Register chan *Listener
	// Unregister requests from the listeners.
	Unregister chan *Listener
	// Inbound messages from an outside.
	Broadcast chan []byte
	// Aeron subscriber used to listen events over Aeron protocol.
	subscriber *transport.Subscriber
	// Logger used for logging.
	logger *logging.Logger
}

func NewHub() *Hub {
	return &Hub{
		Listeners:  make(map[*Listener]bool),
		Register:   make(chan *Listener),
		Unregister: make(chan *Listener),
		Broadcast:  make(chan []byte),
		subscriber: &transport.Subscriber{},
		logger:     logging.MustGetLogger("ws-hub"),
	}
}

func (hub *Hub) Run() {
	hub.subscriber.Connect()
	defer hub.subscriber.Disconnect()

	go hub.listenAeron()

	for {
		select {
		case listener := <-hub.Register:
			hub.Listeners[listener] = true
		case listener := <-hub.Unregister:
			if _, ok := hub.Listeners[listener]; ok {
				delete(hub.Listeners, listener)
				close(listener.Send)
			}
		case message := <-hub.Broadcast:
			for listener := range hub.Listeners {
				select {
				case listener.Send <- message:
				default:
					close(listener.Send)
					delete(hub.Listeners, listener)
				}
			}
		}
	}
}

func (hub *Hub) listenAeron() {
	handler := func(buffer *atomic.Buffer, offset int32, length int32, header *logbuffer.Header) {
		message := buffer.GetBytesArray(offset, length)
		hub.logger.Infof("[AERON] %#x\n", message)
		if len(message) > 2 {
			switch message[2] {
			case 'A':
				// Accepted order
				hub.logger.Info("[AERON] Message received: OrderAcceptedMessage\n")
			case 'J':
				// Rejected order
				hub.logger.Info("[AERON] Message received: OrderRejectedMessage\n")
			case 'E':
				// Executed order
				hub.logger.Info("[AERON] Message received: OrderExecutedMessage\n")
			default:
				// Unknown message type
				hub.logger.Info("[AERON] Message received: unknown\n")
				return
			}
			hub.Broadcast <- message[2:]
		}
	}
	idleStrategy := idlestrategy.Sleeping{SleepFor: time.Millisecond}
	for {
		fragmentsRead := hub.subscriber.Subscription.Poll(handler, 10)
		idleStrategy.Idle(fragmentsRead)
	}
}
