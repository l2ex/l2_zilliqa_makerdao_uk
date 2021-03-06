package main

import (
	"log"
	"math/big"

	"github.com/l2ex/go-engine/crypto/blockchains"
	"github.com/l2ex/go-engine/crypto/blockchains/common"
	"github.com/l2ex/go-engine/crypto/blockchains/ethereum"
	"github.com/l2ex/go-engine/crypto/ecc"
	"github.com/l2ex/go-engine/utils"
)

func main() {
	exampleUpdateChannelMessageSerialization()
	exampleUpdateChannelMessageSignature()
}

func exampleUpdateChannelMessageSerialization() {
	etalon := "792db58835893e047189f4b6639eda85ac34113f33c21af6915e14f376a355c41def6610558d311bfffffffffffffffffffffffffffffffffffffffffffffffffffffffff75772800000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000"
	args := common.L2ArgsUpdateChannel{
		Owner:  utils.HexToBytesUnsafe("792db58835893e047189f4b6639eda85ac34113f"),
		Token:  utils.HexToBytesUnsafe("33c21af6915e14f376a355c41def6610558d311b"),
		Change: big.NewInt(-145264000),
		Nonce:  big.NewInt(1),
		Apply:  false,
		Free:   big.NewInt(0),
	}
	l2 := new(ethereum.L2)
	data, err := l2.SerializeMessageUC(args)
	handleError(err)
	args2, err := l2.DeserializeMessageUC(data)
	handleError(err)
	log.Printf("L2 update channel arguments: %+v\n", args)
	log.Printf("L2 update channel serialized: %#x\n", data)
	log.Printf("L2 update channel deserialized: %+v\n", args2)
	if utils.HexFromBytes(data) != etalon {
		log.Printf("ERROR: Wrong result received during update channel message serialization\n")
	}
}

func exampleUpdateChannelMessageSignature() {
	etalon := "1cb74bb12e78a0dfae6d09d404637bc75abf51bbd5e92e0d52860fe3ca3b2c0e825069fdb6641571e436f1ab9ba329b00e7f3f02634b3856107f9cc87e3ce2ba2c"
	data := utils.HexToBytesUnsafe("792db58835893e047189f4b6639eda85ac34113f33c21af6915e14f376a355c41def6610558d311bfffffffffffffffffffffffffffffffffffffffffffffffffffffffff75772800000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000")
	privateKey, err := ecc.NewPrivateKeyFromString("26655fe5ccb52d03c5f6d31b2676ad525a77ada04ffa33fa2878d0ed261bf2e4", blockchains.EthereumMain)
	handleError(err)
	signature, err := ecc.SignData(data, privateKey, blockchains.EthereumMain)
	handleError(err)
	r, s, v := signature.GetNumbers()
	log.Printf("L2 update channel signing: %s\n", signature.GetString())
	log.Printf("L2 update channel signing (v): %d\n", v)
	log.Printf("L2 update channel signing (r): %#x\n", r)
	log.Printf("L2 update channel signing (s): %#x\n", s)
	if signature.GetString() != etalon {
		log.Printf("ERROR: Wrong result received during update channel message signing\n")
	}
}

func handleError(err error) {
	if err != nil {
		log.Printf("[ERROR] %s\n", err)
	}
}
