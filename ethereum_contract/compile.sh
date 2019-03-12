#!/bin/bash

OUTPUT_PATH="./bin"
OUTPUT_COMPONENTS="--abi --bin --devdoc"
OPTIMIZATION="--optimize --optimize-runs 200"
ALLOWED_PATHS="--allow-paths ."
OTHER_FLAGS="--overwrite"

EVM_VERSION_ETH=""
EVM_VERSION_QTUM="--evm-version homestead"

SOLC_OPTIONS_ETH="$OUTPUT_COMPONENTS $OPTIMIZATION $EVM_VERSION_ETH -o $OUTPUT_PATH $ALLOWED_PATHS $OTHER_FLAGS"
SOLC_OPTIONS_QTUM="$OUTPUT_COMPONENTS $OPTIMIZATION $EVM_VERSION_QTUM -o $OUTPUT_PATH $ALLOWED_PATHS $OTHER_FLAGS"

solc $SOLC_OPTIONS_ETH contracts/common/ERC20Token.sol
solc $SOLC_OPTIONS_ETH contracts/L2DexEthereum.sol

solc $SOLC_OPTIONS_QTUM contracts/common/QRC20Token.sol
solc $SOLC_OPTIONS_QTUM contracts/L2DexQTUM.sol

mkdir -p ${OUTPUT_PATH}/go/ethereum/erc20
mkdir -p ${OUTPUT_PATH}/go/ethereum/l2dex
mkdir -p ${OUTPUT_PATH}/go/qtum/qrc20
mkdir -p ${OUTPUT_PATH}/go/qtum/l2dex

abigen \
    --abi ${OUTPUT_PATH}/ERC20Token.abi \
    --bin ${OUTPUT_PATH}/ERC20Token.bin \
    --pkg erc20 \
    --type ERC20Token \
    --out ${OUTPUT_PATH}/go/ethereum/erc20/token.go
abigen \
    --abi ${OUTPUT_PATH}/QRC20Token.abi \
    --bin ${OUTPUT_PATH}/QRC20Token.bin \
    --pkg qrc20 \
    --type QRC20Token \
    --out ${OUTPUT_PATH}/go/qtum/qrc20/token.go
    
abigen \
    --abi ${OUTPUT_PATH}/L2DexEthereum.abi \
    --bin ${OUTPUT_PATH}/L2DexEthereum.bin \
    --pkg l2dex \
    --type L2DexEthereum \
    --out ${OUTPUT_PATH}/go/ethereum/l2dex/ethereum.go
abigen \
    --abi ${OUTPUT_PATH}/L2DexQTUM.abi \
    --bin ${OUTPUT_PATH}/L2DexQTUM.bin \
    --pkg l2dex \
    --type L2DexQTUM \
    --out ${OUTPUT_PATH}/go/qtum/l2dex/l2dex.go

cp ${OUTPUT_PATH}/ERC20Token.abi ${OUTPUT_PATH}/go/ethereum/erc20/token.abi
cp ${OUTPUT_PATH}/ERC20Token.bin ${OUTPUT_PATH}/go/ethereum/erc20/token.bin
cp ${OUTPUT_PATH}/QRC20Token.abi ${OUTPUT_PATH}/go/qtum/qrc20/token.abi
cp ${OUTPUT_PATH}/QRC20Token.bin ${OUTPUT_PATH}/go/qtum/qrc20/token.bin

cp ${OUTPUT_PATH}/L2DexEthereum.abi ${OUTPUT_PATH}/go/ethereum/l2dex/ethereum.abi
cp ${OUTPUT_PATH}/L2DexEthereum.bin ${OUTPUT_PATH}/go/ethereum/l2dex/ethereum.bin
cp ${OUTPUT_PATH}/L2DexQTUM.abi ${OUTPUT_PATH}/go/qtum/l2dex/l2dex.abi
cp ${OUTPUT_PATH}/L2DexQTUM.bin ${OUTPUT_PATH}/go/qtum/l2dex/l2dex.bin