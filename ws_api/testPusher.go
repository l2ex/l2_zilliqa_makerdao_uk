package main

import (
	"bufio"
	"encoding/csv"
	"fmt"
	"io"
	"log"
	"os"
	"strings"
)

type MoEX struct {
	NO		string		`json:"no"`
	SECCODE string		`json:"seccode"`
	BUYSELL string		`json:"buysell"`
	TIME 	string		`json:"time"`
	ORDERNO string		`json:"orderno"`
	ACTION 	string		`json:"action"`
	PRICE 	string		`json:"price"`
	VOLUME 	string		`json:"volume"`
	TRADENO string		`json:"tradeno"`
	TRADEPRICE string	`json:"tradeprice"`
}



func parseO(raw []string) []byte {
	//func (bigEndian) String() string { return "BigEndian" }
	nmsgO := make([]byte, 43)
	nmsgO[0]	= 'O' //orderType
	nmsgO[1:4] 	= []byte(raw[4]) //orderID
	nmsgO[5:1]	= []byte("C")  //account type
	nmsgO[6:4]	= []byte("12")  //account ID
	nmsgO[10:1]	= []byte(raw[2])  //Buy/Sell
	nmsgO[12:8]	= []byte(raw[7]) //vol
	nmsgO[19:4]	= []byte("HUY") //orderbookID
	var replacer = strings.NewReplacer(" ", ",", "\t", ",")
	str := replacer.Replace(raw[6])
	nmsgO[23:4] = []byte(str) //price int
	//nmsgO[27:4]
	nmsgO[31:4]	= []byte("123") //clienID
	//nmsgO[35:8]	minQTY

	return nmsgO
}

func parseU(raw []string) []byte {
	nmsgO := make([]byte, 21)
	nmsgO[0]	= 'U' //orderType
	nmsgO[1:4] 	= []byte(raw[4]) //orderID
	nmsgO[5:4] 	= []byte(raw[4]) //orderID
	nmsgO[9:8]	= []byte(raw[7]) //QTY
	var replacer = strings.NewReplacer(" ", ",", "\t", ",")
	str := replacer.Replace(raw[6])
	nmsgO[17:4] = []byte(str) //price int
	return nmsgO
}

func parseX(raw []string) []byte {
	nmsgO := make([]byte, 5)
	nmsgO[0]	= 'X' //orderType
	nmsgO[1:4] 	= []byte(raw[4]) //orderID
	return nmsgO
}





var y = 0;
func main() {
	csvFile, _ := os.Open("OrderLog20181229-2.csv")
	reader := csv.NewReader(bufio.NewReader(csvFile))
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
				Order := parseX(ord)
			case "1": //post   = O
				Order := parseO(ord)
			case "2": //deal   = O
				Order := parseO(ord)
			}
			s := string(ord[5])
			if s[0:1] == "U" {





	}}}

	fmt.Println(y)

/*		orders = append(orders, MoEX{
			NO: line[0],
			SECCODE: line[1],
			BUYSELL: line[2],
			TIME: line[3],
			ORDERNO: line[4],
			ACTION: line[5],
			PRICE: line[6],
			VOLUME: line[7],
			TRADENO: line[8],
			TRADEPRICE:line[9],
		}
		)
*/

	}

