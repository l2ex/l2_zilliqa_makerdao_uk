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

func (feed *SUB)Connect()  {
	flag.Parse()

	if !*Config.LoggingOn {
		logging.SetLevel(logging.INFO, "aeron")
		logging.SetLevel(logging.INFO, "memmap")
		logging.SetLevel(logging.DEBUG, "driver")
		logging.SetLevel(logging.INFO, "counters")
		logging.SetLevel(logging.INFO, "logbuffers")
		logging.SetLevel(logging.INFO, "buffer")
	}

	to := time.Duration(time.Millisecond.Nanoseconds() * *Config.DriverTo)
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


