package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"

	"github.com/gorilla/websocket"
)

var Addr = flag.String("addr", "localhost:2054", "http service address")

var upgrader = websocket.Upgrader{} // use default options

func apiStream(w http.ResponseWriter, r *http.Request) {
	fmt.Println("creating to %s", *Addr)
	c, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Print("upgrade:", err)
		return
	}
	defer c.Close()
	for {
		mt , message, err := c.ReadMessage()
		if err != nil {
			log.Println("read:", err)
			break
		}

		switch message[0] {
		// place order
		case 'O':
			go placeOrder(string(message))
			// get hystory
		}

		// send history
		go feed(*c , mt)

	}
}

func main() {
	flag.Parse()
	log.SetFlags(0)
	http.HandleFunc("/", apiStream)
	log.Fatal(http.ListenAndServe(*Addr, nil))
}

func placeOrder(message string)  {
	fmt.Println(message)
}

func feed(c websocket.Conn, mt int) {
	err := c.WriteMessage( mt, []byte("dfsfsdfsd"))
	if err != nil {
		log.Println("read:", err)
		//break
	}
}