package main

import (
	"fmt"
	"math/big"

	"github.com/l2ex/go-engine/crypto/blockchains"
	"github.com/l2ex/go-engine/crypto/blockchains/common"
	"github.com/l2ex/go-engine/crypto/blockchains/ethereum"
	"github.com/l2ex/go-engine/crypto/ecc"
	"github.com/l2ex/go-engine/utils"
)

func main() {
	testUpdateChannelMessageSerialization()
	testUpdateChannelMessageSignature()
}

func testUpdateChannelMessageSerialization() {
	etalon := "792db58835893e047189f4b6639eda85ac34113f33c21af6915e14f376a355c41def6610558d311bfffffffffffffffffffffffffffffffffffffffffffffffffffffffff75772800000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000"
	args := common.L2ArgsUpdateChannel{
		Owner: utils.HexToBytesUnsafe("792db58835893e047189f4b6639eda85ac34113f"),
		Token: utils.HexToBytesUnsafe("33c21af6915e14f376a355c41def6610558d311b"),
		Change: big.NewInt(-145264000),
		Nonce: big.NewInt(1),
		Apply: false,
		Free: big.NewInt(0),
	}
	l2 := new(ethereum.L2)
	data, _ := l2.SerializeMessageUC(args)
	args2, _ := l2.DeserializeMessageUC(data)
	fmt.Printf("L2 update channel arguments: %+v\n", args)
	fmt.Printf("L2 update channel serialized: %#x\n", data)
	fmt.Printf("L2 update channel deserialized: %+v\n", args2)
	if utils.HexFromBytes(data) != etalon {
		fmt.Printf("ERROR: Wrong result received during update channel message serialization\n")
	}
}

func testUpdateChannelMessageSignature() {
	etalon := "1cb74bb12e78a0dfae6d09d404637bc75abf51bbd5e92e0d52860fe3ca3b2c0e825069fdb6641571e436f1ab9ba329b00e7f3f02634b3856107f9cc87e3ce2ba2c"
	data := utils.HexToBytesUnsafe("792db58835893e047189f4b6639eda85ac34113f33c21af6915e14f376a355c41def6610558d311bfffffffffffffffffffffffffffffffffffffffffffffffffffffffff75772800000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000")
	privateKey, _ := ecc.NewPrivateKeyFromString("26655fe5ccb52d03c5f6d31b2676ad525a77ada04ffa33fa2878d0ed261bf2e4", blockchains.EthereumMain)
	signature, _ := ecc.SignData(data, privateKey, blockchains.EthereumMain)
	r, s, v := signature.GetNumbers()
	fmt.Printf("L2 update channel signing: %s\n", signature.GetString())
	fmt.Printf("L2 update channel signing (v): %d\n", v)
	fmt.Printf("L2 update channel signing (r): %#x\n", r)
	fmt.Printf("L2 update channel signing (s): %#x\n", s)
	if signature.GetString() != etalon {
		fmt.Printf("ERROR: Wrong result received during update channel message signing\n")
	}
}

func handleError(err error) {
	if err != nil {
		fmt.Printf("[ERROR] %s\n", err)
	}
}
