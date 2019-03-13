package InterExchange

import (
	"flag"
	"github.com/lirm/aeron-go/aeron"
	"github.com/op/go-logging"
)


var logger = logging.MustGetLogger("basic_publisher")


var Config = struct {
	AeronPrefix     *string
	ProfilerEnabled *bool
	DriverTo        *int64
	StreamID        *int
	Channel         *string
	Messages        *int
	Size            *int
	LoggingOn       *bool
	OrderBookStreamID *int
	ApiStreamID *int
	OrderBookChannel  *string
	APIChannel  *string

}{
	AeronPrefix:       flag.String("p", aeron.DefaultAeronDir, "root directory for aeron driver file"),
	ProfilerEnabled:   flag.Bool("prof", false, "enable CPU profiling"),
	DriverTo:          flag.Int64("to", 1000000, "driver liveliness timeout in ms"),
	StreamID:          flag.Int("sid", 10, "default streamId to use"),
	Channel:           flag.String("chan", "aeron:udp?endpoint=10.10.0.124:40123", "default channel to subscribe to"),
	Messages:          flag.Int("m", 1000000, "number of messages to send"),
	Size:              flag.Int("len", 256, "messages size"),
	LoggingOn:         flag.Bool("l", false, "enable logging"),
	OrderBookStreamID: flag.Int("S", 11, "streamId to use for orderbook"),
	ApiStreamID:       flag.Int("s", 10, "streamId to use for api"),
	OrderBookChannel:  flag.String("C", "aeron:udp?endpoint="+OrderBookAdr, "default channel to subscribe to orderbook"),
	APIChannel:        flag.String("c", "aeron:udp?endpoint=0.0.0.0:40123", "default channel to subscribe to api"),
}

const OrderBookAdr = "10.10.10.124:40123"
