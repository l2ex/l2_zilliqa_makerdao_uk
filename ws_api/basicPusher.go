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
	"encoding/binary"
	"encoding/csv"
	"flag"
	"fmt"
	"io"
	"log"
	"math"
	"os"
	"strconv"
	"strings"
	"time"

	"github.com/lirm/aeron-go/aeron"
	"github.com/lirm/aeron-go/aeron/atomic"
	"github.com/op/go-logging"
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
	flag.String("chan", "aeron:udp?endpoint=10.0.1.5:40570", "default channel to subscribe to"),
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

	quantity, _ := strconv.ParseUint(raw[7], 10, 64)
	price, _ := strconv.ParseFloat(raw[6], 64)
	orderID, _ := strconv.ParseUint(raw[4], 10, 32)
	accountID, _ := strconv.ParseUint("12", 10, 32)
	orderbookID := uint32(0)
	switch raw[1] {
	case "BYN_RUB":
		orderbookID = 1
	case "CHF_RUB":
		orderbookID = 2
	case "CNY_RUB":
		orderbookID = 3
	case "EUR_RUB":
		orderbookID = 4
	case "EUR_RUB1":
		orderbookID = 5
	case "EUR_RUB2":
		orderbookID = 6
	case "EUR_USD":
		orderbookID = 7
	case "GBP_RUB":
		orderbookID = 8
	case "GLD_RUB":
		orderbookID = 9
	case "HKD_RUB":
		orderbookID = 10
	case "SLV_RUB":
		orderbookID = 11
	case "USD_EUR":
		orderbookID = 12
	case "USD_RUB":
		orderbookID = 13
	case "USD_RUB1":
		orderbookID = 14
	}

	nmsgO := make([]byte, 45)
	binary.BigEndian.PutUint16(nmsgO[:2], 43)                                // message size
	nmsgO[2] = 'O'                                                           // type
	binary.BigEndian.PutUint32(nmsgO[3:7], uint32(orderID))                  // order ID
	nmsgO[7] = 'C'                                                           // account type
	binary.BigEndian.PutUint32(nmsgO[8:12], uint32(accountID))               // account ID
	nmsgO[12] = raw[2][0]                                                    // order side (buy/sell)
	binary.BigEndian.PutUint64(nmsgO[13:21], quantity)                       // quantity
	binary.BigEndian.PutUint32(nmsgO[21:25], orderbookID)                    // orderbook ID
	binary.BigEndian.PutUint32(nmsgO[25:29], uint32(math.Round(price*1000))) // price
	binary.BigEndian.PutUint32(nmsgO[29:33], 0)                              // time in force
	binary.BigEndian.PutUint32(nmsgO[33:37], 0)                              // client ID
	binary.BigEndian.PutUint64(nmsgO[37:45], 0)                              // minimum quantity

	return nmsgO
}

func parseX(raw []string) []byte {

	orderID, _ := strconv.ParseUint(raw[4], 10, 32)

	nmsgO := make([]byte, 7)
	binary.BigEndian.PutUint16(nmsgO[:2], 5)                // message size
	nmsgO[2] = 'X'                                          // type
	binary.BigEndian.PutUint32(nmsgO[3:7], uint32(orderID)) // order ID

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

	fmt.Printf("streaming to aeron udp : %s\n", *ExamplesConfig.Channel)

	// total number of all messages
	var total uint64
	start := time.Now()

	csvFile, _ := os.Open("/Users/im/github/l2_scylla_makerdao_uk/ws_api/OrderLog20181229-2.csv")
	reader := csv.NewReader(bufio.NewReader(csvFile))
	srcBuffer := make([]byte, 1024)
	srcBufferAtomic := atomic.MakeBuffer(srcBuffer, len(srcBuffer))
	firstLine := true
	for {
		line, error := reader.Read()
		if error == io.EOF {
			break
		} else if error != nil {
			log.Fatal(error)
		}
		if firstLine {
			firstLine = false
			continue
		}
		time.Sleep(time.Millisecond * 20)
		for i := 0; i < len(line); i++ {
			ord := strings.Split(line[i], ";")
			switch ord[5] {
			case "0": // delete = X
				message := parseX(ord)
				copy(srcBuffer, message)
				ret := publication.Offer(srcBufferAtomic, 0, int32(len(message)), nil)
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
				copy(srcBuffer, message)
				ret := publication.Offer(srcBufferAtomic, 0, int32(len(message)), nil)
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
				copy(srcBuffer, message)
				ret := publication.Offer(srcBufferAtomic, 0, int32(len(message)), nil)
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

		fmt.Printf("Time spent: %d seconds\n", time.Since(start)/1000000000)
	}
}
