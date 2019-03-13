package InterExchange

import (
	"flag"
	"github.com/lirm/aeron-go/aeron"
	"github.com/lirm/aeron-go/examples"
	"github.com/op/go-logging"
	"time"
)

//L2 Data Feed from exchange Core to API service based on Aeron protocol


type SUB struct {
	Transport *aeron.Aeron
	Flow chan *aeron.Subscription
}

var l = logging.MustGetLogger("basic_subscriber")

var localConfig = struct{
	AeronPrefix     *string
	ProfilerEnabled *bool
	DriverTo        *int64
	StreamID        *int
	Channel         *string
	Messages        *int
	Size            *int
	LoggingOn       *bool
}{
	AeronPrefix:       flag.String("p", aeron.DefaultAeronDir, "root directory for aeron driver file"),
	ProfilerEnabled:   flag.Bool("prof", false, "enable CPU profiling"),
	DriverTo:          flag.Int64("to", 1000000, "driver liveliness timeout in ms"),
	StreamID:          flag.Int("sid", 1023, "default streamId to use"),
	Channel:           flag.String("chan", "aeron:udp?endpoint=10.10.0.124:40123", "default channel to subscribe to"),
	Messages:          flag.Int("m", 1000000, "number of messages to send"),
	Size:              flag.Int("len", 256, "messages size"),
	LoggingOn:         flag.Bool("l", false, "enable logging"),
}
func (feed *SUB)Connect()  {
	flag.Parse()

	if !*localConfig.LoggingOn {
		logging.SetLevel(logging.INFO, "aeron")
		logging.SetLevel(logging.INFO, "memmap")
		logging.SetLevel(logging.DEBUG, "driver")
		logging.SetLevel(logging.INFO, "counters")
		logging.SetLevel(logging.INFO, "logbuffers")
		logging.SetLevel(logging.INFO, "buffer")
	}

	to := time.Duration(time.Millisecond.Nanoseconds() * *localConfig.DriverTo)
	ctx := aeron.NewContext().AeronDir(*examples.ExamplesConfig.AeronPrefix).MediaDriverTimeout(to)
	var err error
	feed.Transport, err = aeron.Connect(ctx)
	if err != nil {
		l.Fatalf("Failed to connect to media driver: %s\n", err.Error())
	}


}

func (feed *SUB)Disconnect() {
	feed.Transport.Close()
}


