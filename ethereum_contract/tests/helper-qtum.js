const fs = require('fs')
const Web3 = require('web3')
const { QtumRPC } = require('qtumjs')
const bs58check = require('bs58check')


class HelperQtum {

    constructor(connection) {
        this.web3 = new Web3()
        this.rpc = new QtumRPC(connection)
        this.contracts = {
            ecrpk: {
                bin: fs.readFileSync('./ECRecoverPublicKey.bin').toString()
            },
            token: {
                abi: JSON.parse(fs.readFileSync('./bin/QRC20Token.abi')),
                bin: fs.readFileSync('./bin/QRC20Token.bin').toString()
            },
            l2: {
                abi: JSON.parse(fs.readFileSync('./bin/L2DexQTUM.abi')),
                bin: fs.readFileSync('./bin/L2DexQTUM.bin').toString()
            }
        }
        this.gasLimit = 2500000
        this.gasPrice = 0.0000005
    }

    requestAddressesByAccount(accountName = '') {
        return this.rpc.rawCall('getaddressesbyaccount', [ accountName ])
    }

    requestAccountBalance(accountName = '') {
        return this.rpc.rawCall('getbalance', [ accountName ])
    }

    requestReceivedByAccount(accountName = '') {
        return this.rpc.rawCall('getreceivedbyaccount', [ accountName ])
    }

    requestReceivedByAddress(address) {
        return this.rpc.rawCall('getreceivedbyaddress', [ address ])
    }

    requestNewAddress(accountName = '') {
        return this.rpc.rawCall('getnewaddress', [ accountName ])
    }

    requestTransaction(transactionId) {
        return this.rpc.rawCall('gettransaction', [ transactionId ])
    }

    requestTransactionReceipt(transactionId) {
        return this.rpc.rawCall('gettransactionreceipt', [ transactionId ])
    }

    // DEPLOY CONTRACTS

    deployECRecoverPublicKey(creatorAddress) {
        return this.rpc.rawCall('createcontract', [ this.contracts.ecrpk.bin, this.gasLimit, this.gasPrice, creatorAddress ])
    }

    deployToken(name, symbol, decimals, creatorAddress) {
        const parametersTypes = this.getContractConstructorParameters(this.contracts.token.abi)
        if (parametersTypes.length == 3) {
            const parametersBin = this.web3.eth.abi.encodeParameters(parametersTypes, [ name, symbol, decimals ])
            const bytecode = `${this.contracts.token.bin}${parametersBin.slice(2)}`
            return this.rpc.rawCall('createcontract', [ bytecode, this.gasLimit, this.gasPrice, creatorAddress ])
        }
    }

    deployL2(oracle, ecrpkAddress, creatorAddress) {
        const parametersTypes = this.getContractConstructorParameters(this.contracts.l2.abi)
        if (parametersTypes.length == 2) {
            const parametersBin = this.web3.eth.abi.encodeParameters(parametersTypes, [ oracle, ecrpkAddress ])
            const bytecode = `${this.contracts.l2.bin}${parametersBin.slice(2)}`
            return this.rpc.rawCall('createcontract', [ bytecode, this.gasLimit, this.gasPrice, creatorAddress ])
        }
    }

    // HELPER FUNCTIONS

    transferQtum(receiverAddress, amount) {
        return this.rpc.rawCall('sendtoaddress', [ receiverAddress, amount ])
    }

    addressQtumToAddressEthereum(addressQtum) {
        const addressEthereum = bs58check.decode(addressQtum).slice(1)
        return `0x${addressEthereum.toString('hex')}`
    }

    getContractConstructorParameters(abi) {
        let parametersTypes = []
        abi.forEach(object => {
            if (object.type == 'constructor') {
                object.inputs.forEach(parameter => {
                    parametersTypes.push(parameter.type)
                })
                return parametersTypes
            }
        })
        return parametersTypes
    }
}

module.exports = HelperQtum