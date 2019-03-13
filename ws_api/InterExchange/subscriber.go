package InterExchange

import (
	"github.com/lirm/aeron-go/aeron"
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


	to := time.Duration(time.Millisecond.Nanoseconds() * 1000000)
	ctx := aeron.NewContext().AeronDir(aeron.DefaultAeronDir).MediaDriverTimeout(to)
	var err error
	feed.Transport, err = aeron.Connect(ctx)
	if err != nil {
		l.Fatalf("Failed to connect to media driver: %s\n", err.Error())
	}


}

func (feed *SUB)Disconnect() {
	feed.Transport.Close()
}


