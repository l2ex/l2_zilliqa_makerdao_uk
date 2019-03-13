/*
Copyright 2016 Stanislav Liberman

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

package main

import (
	"bufio"
	//"encoding/binary"
	"encoding/csv"
	"flag"
	"fmt"
	"github.com/lirm/aeron-go/aeron"
	"github.com/lirm/aeron-go/aeron/atomic"
	"io"
	"strings"

	//"github.com/lirm/aeron-go/examples"

	//"github.com/lirm/aeron-go/examples"
	"github.com/op/go-logging"
	"log"
	"os"
	"time"
)

var logger = logging.MustGetLogger("basic_publisher")


var ExamplesConfig = struct {
	AeronPrefix     *string
	ProfilerEnabled *bool
	DriverTo        *int64
	StreamID        *int
	Channel         *string
	Messages        *int
	Size            *int
	LoggingOn       *bool
}{
	flag.String("p", aeron.DefaultAeronDir, "root directory for aeron driver file"),
	flag.Bool("prof", false, "enable CPU profiling"),
	flag.Int64("to", 1000000, "driver liveliness timeout in ms"),
	flag.Int("sid", 1025, "default streamId to use"),
	flag.String("chan", "aeron:udp?endpoint=10.10.0.122:40570", "default channel to subscribe to"),
	flag.Int("m", 1000000, "number of messages to send"),
	flag.Int("len", 256, "messages size"),
	flag.Bool("l", false, "enable logging"),
}



func check(e error) {
	if e != nil {
		panic(e)
	}
}

func parseO(raw []string) []byte {
	//func (bigEndian) String() string { return "BigEndian" }
	nmsgO := make([]byte, 43)
	nmsgO[0]	= 'O' //orderType
	copy(nmsgO[1:4], []byte(raw[3])) //orderID
	copy(nmsgO[5:6], []byte("C")) //account type
	copy(nmsgO[6:10], []byte("12"))  //account ID
	copy(nmsgO[10:11], []byte(raw[2])) //Buy/Sell
	copy(nmsgO[12:20], []byte(raw[7])) //vol
	copy(nmsgO[19:23], []byte("HUY")) //orderbookID
	var replacer = strings.NewReplacer(" ", ",", "\t", ",")
	str := replacer.Replace(raw[6])
	copy(nmsgO[23:27] , []byte(str)) //price int
	//nmsgO[27:4]
	copy(nmsgO[31:35],  []byte("123")) //clienID
	//nmsgO[35:8]	minQTY

	return nmsgO
}

func parseU(raw []string) []byte {
	nmsgO := make([]byte, 21)
	nmsgO[0]	= 'U' //orderType
	copy(nmsgO[1:4] , []byte(raw[4])) //orderID
	copy(nmsgO[5:9] , []byte(raw[4])) //orderID
	copy(nmsgO[9:17]	, []byte(raw[7])) //QTY
	var replacer = strings.NewReplacer(" ", ",", "\t", ",")
	str := replacer.Replace(raw[6])
	copy(nmsgO[17:21] , []byte(str)) //price int
	return nmsgO
}

func parseX(raw []string) []byte {
	nmsgO := make([]byte, 5)
	nmsgO[0]	= 'X' //orderType
	copy(nmsgO[1:4] , []byte(raw[4])) //orderID
	return nmsgO
}


func main() {
	flag.Parse()

	if !*ExamplesConfig.LoggingOn {
		logging.SetLevel(logging.INFO, "aeron")
		logging.SetLevel(logging.INFO, "memmap")
		logging.SetLevel(logging.INFO, "driver")
		logging.SetLevel(logging.INFO, "counters")
		logging.SetLevel(logging.INFO, "logbuffers")
		logging.SetLevel(logging.INFO, "buffer")
		logging.SetLevel(logging.INFO, "rb")
	}

	to := time.Duration(time.Millisecond.Nanoseconds() * *ExamplesConfig.DriverTo)
	ctx := aeron.NewContext().AeronDir(*ExamplesConfig.AeronPrefix).MediaDriverTimeout(to)

	a, err := aeron.Connect(ctx)
	if err != nil {
		logger.Fatalf("Failed to connect to media driver: %s\n", err.Error())
	}
	defer a.Close()

	publication := <-a.AddPublication(*ExamplesConfig.Channel, int32(*ExamplesConfig.StreamID))
	defer publication.Close()
	log.Printf("Publication found %v", publication)

	fInput, err := os.Open("/Users/im/gitlab/itch5parser/sample.itch")
	if err != nil {
		logger.Fatalf("Cant open message file")
	}
	defer fInput.Close()

	//msgHeader := make([]byte, 2)
	//var msgLength uint16
	//var rMsgHeader *bytes.Reader
	//msgNum := 0
	//parseFlag := make(map[byte]bool)
	// total number of all messages
	var total uint64
	// total number of messages for each message type
	//totalType := make(map[byte]uint64)
	start := time.Now()
	fmt.Printf("=========== Parsing ITCH v5.0 starts ===========\n")
	fmt.Printf("streaming to aeron udp : %s\n", *ExamplesConfig.Channel)

	csvFile, _ := os.Open("/Users/im/github/l2_scylla_makerdao_uk/ws_api/OrderLog20181229-2.csv")
	reader := csv.NewReader(bufio.NewReader(csvFile))
	srcBuffer := atomic.MakeBuffer(([]byte)("01234567890123456789012345678901234567890123456789"))
	for {
		line, error := reader.Read()
		if error == io.EOF {
			break
		} else if error != nil {
			log.Fatal(error)
		}
		for i := 0; i < len(line); i++ {
			ord := strings.Split(line[i], ";")
			switch ord[5] {
			case "0": //delete = X
				message := parseX(ord)
				//srcBuffer := atomic.MakeBuffer(([]byte)(message))
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
			case "1": //post   = O
				message := parseO(ord)

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
			case "2": //deal   = O
				message := parseO(ord)

				//srcBuffer := atomic.MakeBuffer(([]byte)(message))
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
			}
		}

		/*	for {
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
		fmt.Printf("Time spent: %d seconds\n", time.Since(start)/1000000000)

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
}