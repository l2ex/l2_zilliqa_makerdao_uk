package aeron

import (
	"flag"
)

// Config contains default configuration for Aeron publisher and subscriber.
var Config = struct {
	OrderBookChannel  *string
	APIChannel        *string
	OrderBookStreamID *int
	APIStreamID       *int
}{
	OrderBookChannel:  flag.String("C", "aeron:ipc", "aeron channel endpoint used by orderbook subscriber"),
	APIChannel:        flag.String("c", "aeron:ipc", "aeron channel endpoint used by API subscriber"),
	OrderBookStreamID: flag.Int("S", 1025, "aeron stream ID used by orderbook subscriber"),
	APIStreamID:       flag.Int("s", 1024, "aeron stream ID used by API subscriber"),
}
