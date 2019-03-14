package aeron

import (
	"time"

	"github.com/lirm/aeron-go/aeron"
	"github.com/op/go-logging"
)

// Subscriber contains all Aeron specific objects needed to receive data from Aeron channel.
type Subscriber struct {
	Context      *aeron.Context
	Transport    *aeron.Aeron
	Subscription *aeron.Subscription
	logger       *logging.Logger
}

// Connect connects subscriber to Aeron channel.
func (subscriber *Subscriber) Connect() error {

	subscriber.logger = logging.MustGetLogger("aeron-subscriber")

	keepAliveTimeout := time.Hour * 24 * 7
	subscriber.Context = aeron.NewContext().MediaDriverTimeout(keepAliveTimeout)

	var err error
	subscriber.Transport, err = aeron.Connect(subscriber.Context)
	if err != nil {
		subscriber.logger.Errorf("Failed to connect to Aeron media driver: %s\n", err.Error())
		return err
	}

	subscriber.Subscription = <-subscriber.Transport.AddSubscription(*Config.APIChannel, int32(*Config.APIStreamID))
	subscriber.logger.Infof("Subscription created: %+v\n", subscriber.Subscription)

	return nil
}

// Disconnect disconnects subscriber from Aeron channel.
func (subscriber *Subscriber) Disconnect() error {
	subscriber.Subscription.Close()
	subscriber.Transport.Close()
	return nil
}
