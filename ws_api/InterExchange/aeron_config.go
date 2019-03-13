package InterExchange

import (
	"flag"
	"github.com/op/go-logging"
)


var logger = logging.MustGetLogger("basic_publisher")


var Config = struct {
	OrderBookStreamID *int
	ApiStreamID *int
	OrderBookChannel  *string
	APIChannel  *string

}{

	OrderBookStreamID: flag.Int("S", 1025, "streamId to use for orderbook"),
	ApiStreamID:       flag.Int("s", 1024, "streamId to use for api"),
	OrderBookChannel:  flag.String("C", "aeron:udp?endpoint=10.10.0.122:40570", "default channel to subscribe to orderbook"),
	APIChannel:        flag.String("c", "aeron:udp?endpoint=10.10.0.122:40570", "default channel to subscribe to api"),
}

const OrderBookAdr = "10.10.0.122:40570"
