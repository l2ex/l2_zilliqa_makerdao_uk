# Ethereum and QTUM one-2-n payment channel smart contracts

This repository contains necessary infrasctucture for compiling and deploying one-2-n payment channel smart contracts. Implementation one-2-n payment channel smart contract can be found in file [contracts/L2Dex.sol](contracts/L2Dex.sol).

# Compiling

To compile all presented smart contracts for both Ethereum and QTUM just run [compile.sh](compile.sh) script:

```sh
compile.sh
```

Note that `solidity` version **0.4.25** should be installed and available with command `solc`.

Another tool which is required is `abigen`. It can be installed with following commands:

```sh
go get -u github.com/ethereum/go-ethereum
cd $GOPATH/src/github.com/ethereum/go-ethereum
godep go install ./cmd/abigen
```