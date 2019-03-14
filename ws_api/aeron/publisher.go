package aeron

import (
	"time"

	"github.com/lirm/aeron-go/aeron"
	"github.com/lirm/aeron-go/aeron/atomic"
	"github.com/op/go-logging"
)

// Publisher contains all Aeron specific objects needed to send data to Aeron channel.
type Publisher struct {
	Context      *aeron.Context
	Transport    *aeron.Aeron
	Publication  *aeron.Publication
	Buffer       []byte
	BufferAtomic *atomic.Buffer
	logger       *logging.Logger
}

// Connect connects publisher to Aeron channel.
func (publisher *Publisher) Connect() error {
	logging.SetLevel(logging.WARNING, "aeron")
	logging.SetLevel(logging.WARNING, "logbuffers")
	logging.SetLevel(logging.WARNING, "memmap")
	logging.SetLevel(logging.WARNING, "counters")
	logging.SetLevel(logging.WARNING, "driver")

	publisher.logger = logging.MustGetLogger("aeron-publisher")

	keepAliveTimeout := time.Hour * 24 * 7
	publisher.Context = aeron.NewContext().MediaDriverTimeout(keepAliveTimeout)

	var err error
	publisher.Transport, err = aeron.Connect(publisher.Context)
	if err != nil {
		publisher.logger.Errorf("Failed to connect to Aeron media driver: %s\n", err.Error())
		return err
	}

	publisher.Publication = <-publisher.Transport.AddPublication(*Config.OrderBookChannel, int32(*Config.OrderBookStreamID))
	publisher.logger.Infof("Publication created: %+v\n", publisher.Publication)

	publisher.Buffer = make([]byte, 1024)
	publisher.BufferAtomic = atomic.MakeBuffer(publisher.Buffer, len(publisher.Buffer))

	return nil
}

// Disconnect disconnects publisher from Aeron channel.
func (publisher *Publisher) Disconnect() error {
	publisher.Publication.Close()
	publisher.Transport.Close()
	return nil
}
