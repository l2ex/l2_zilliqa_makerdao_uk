package InterExchange

import (
	"github.com/lirm/aeron-go/aeron"
	"github.com/lirm/aeron-go/aeron/atomic"
	"log"
	"time"
)

type streamPub chan *aeron.Publication

type PUB struct {
	Transport *aeron.Aeron
	Publication *aeron.Publication
	Buffer *atomic.Buffer
}



func check(e error) {
	if e != nil {
		panic(e)
	}
}

func (publisher *PUB)Connect()  {

	to := time.Duration(time.Millisecond.Nanoseconds() * 1000000)
	ctx := aeron.NewContext().AeronDir(aeron.DefaultAeronDir).MediaDriverTimeout(to)

	var err error
	publisher.Transport, err = aeron.Connect(ctx)
	if err != nil {
		logger.Fatalf("Failed to connect to media driver: %s\n", err.Error())
	}
	publisher.Publication = <-publisher.Transport.AddPublication(*Config.OrderBookChannel, int32(*Config.OrderBookStreamID))
	log.Printf("Publication found %v", publisher.Publication)
	bulkMSG := "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
	publisher.Buffer = atomic.MakeBuffer(([]byte)(bulkMSG))


	/*
		msgHeader := make([]byte, 2)
		var msgLength uint16
		var rMsgHeader *bytes.Reader
		var total uint64



		for {
			count, _ := fInput.Read(msgHeader)
			if count < 2 {
				// EOF
				fmt.Printf("=========== Parsing ITCH v5.0 ends   ===========\n")
				break
			}
			rMsgHeader = bytes.NewReader(msgHeader)
			err := binary.Read(rMsgHeader, binary.BigEndian, &msgLength)
			check(err)

			// message buffer
			// message buffer
			message := make([]byte, msgLength+2)
			message[0] = msgHeader[0]
			message[1] = msgHeader[1]
			count, _ = fInput.Read(message[2:])
			if count < int(msgLength) {
				panic("Error reading input file")
			}
			//message := fmt.Sprintf("this is a message %d", count)
			srcBuffer := atomic.MakeBuffer(([]byte)(message))
			log.Print(message)
			t := message[2]
			switch t {
			case 'S':
				log.Printf("ProcessSystemEventMessage")
			case 'R':
				log.Printf( "ProcessStockDirectoryMessage")
			case 'H':
				log.Printf( "ProcessStockTradingActionMessage")
			case 'Y':
				log.Printf( "ProcessRegSHOMessage")
			case 'L':
				log.Printf( "ProcessMarketParticipantPositionMessage")
			case 'V':
				log.Printf( "ProcessMWCBDeclineMessage")
			case 'W':
				log.Printf( "ProcessMWCBStatusMessage")
			case 'K':
				log.Printf( "ProcessIPOQuotingMessage")
			case 'A':
				log.Printf( "ProcessAddOrderMessage")
			case 'F':
				log.Printf( "ProcessAddOrderMPIDMessage")
			case 'E':
				log.Printf( "ProcessOrderExecutedMessage")
			case 'C':
				log.Printf( "ProcessOrderExecutedWithPriceMessage")
			case 'X':
				log.Printf( "ProcessOrderCancelMessage")
			case 'D':
				log.Printf( "ProcessOrderDeleteMessage")
			case 'U':
				log.Printf( "ProcessOrderReplaceMessage")
			case 'P':
				log.Printf( "ProcessTradeMessage")
			case 'Q':
				log.Printf( "ProcessCrossTradeMessage")
			case 'B':
				log.Printf( "ProcessBrokenTradeMessage")
			case 'I':
				log.Printf( "ProcessNOIIMessage")
			case 'N':
				log.Printf( "ProcessRPIIMessage")
			default:
				log.Printf( "ProcessUnknownMessage")

			}
			//log.Printf(string(t))
			ret := publication.Offer(srcBuffer, 0, int32(len(message)), nil)
			switch ret {
			case aeron.NotConnected:
				log.Printf("%d: not connected yet", total)
			case aeron.BackPressured:
				log.Printf("%d: back pressured", total)
			default:
				if ret < 0 {
					log.Printf("%d: Unrecognized code: %d", total, ret)
				} else {
					log.Printf("%d: success!", total)
				}
			}
			if !publication.IsConnected() {
				log.Printf("no subscribers detected")
			}
			//time.Sleep(time.Second)
		}
	*/
	//	fmt.Printf("Total number of all messages parsed: %d\n", total)
	//	for t, n := range totalType {
	//		fmt.Printf("Total number of %c messages parsed: %d\n", t, n)
	//	}
	//fmt.Printf("Time spent: %d seconds\n", time.Since(start)/1000000000)

	/**
	for counter := 0; counter < *examples.ExamplesConfig.Messages; counter++ {
		message := fmt.Sprintf("this is a message %d", counter)
		srcBuffer := atomic.MakeBuffer(([]byte)(message))
		ret := publication.Offer(srcBuffer, 0, int32(len(message)), nil)
		switch ret {
		case aeron.NotConnected:
			log.Printf("%d: not connected yet", counter)
		case aeron.BackPressured:
			log.Printf("%d: back pressured", counter)
		default:
			if ret < 0 {
				log.Printf("%d: Unrecognized code: %d", counter, ret)
			} else {
				log.Printf("%d: success!", counter)
			}
		}

		if !publication.IsConnected() {
			log.Printf("no subscribers detected")
		}
		time.Sleep(time.Second)
	}
	**/
	
}

func (publisher *PUB)Disconnect() {
	 publisher.Transport.Close()
	 publisher.Publication.Close()
}