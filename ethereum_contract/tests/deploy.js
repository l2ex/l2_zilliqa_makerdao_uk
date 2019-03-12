const HelperEthereum = require('./helper-ethereum')
const HelperQtum = require('./helper-qtum')

const config = require('./config')
const connectionEthereum = config.ethereum.connection
const configQtum = config.qtum.connection
const connectionQtum = configQtum.username && configQtum.password ?
    `http://${configQtum.username}:${configQtum.password}@${configQtum.address}:${configQtum.port}` :
    `http://${configQtum.address}:${configQtum.port}`

const helperEthereum = new HelperEthereum(connectionEthereum)
const helperQtum = new HelperQtum(connectionQtum)

function sleepPromise(ms) {
    return new Promise(resolve => {
        if (ms > 0) {
            setTimeout(() => {
                resolve()
            }, ms)
        } else {
            resolve()
        }
    })
}

async function waitForTransactionQtum(transactionId) {
    while (true) {
        const transactionReceipt = await helperQtum.requestTransactionReceipt(transactionId)
        if (transactionReceipt.length > 0) {
            return transactionReceipt
        }
        await sleepPromise(1000)
    }
}

async function deployERC20(name, symbol, decimals) {
    const creatorAddress = config.ethereum.accounts.tokenOwner.address
    const creatorPrivateKey = config.ethereum.accounts.tokenOwner.privateKey
    console.log(`[ETHEREUM] Deploying ERC20 token '${name}' (${symbol}) with ${decimals} decimals...`)
    const result = await helperEthereum.deployToken(name, symbol, decimals, creatorAddress, creatorPrivateKey)
    if (!result.status) {
        throw new Error('[ETHEREUM] Unable to deploy ERC20 token')
    }
    console.log(`[ETHEREUM] ERC20 token '${name}' (${symbol}) deployed at address: ${result.contractAddress} (${result.gasUsed} gas used)`)
    return result
}

async function deployQRC20(name, symbol, decimals) {
    const creatorAddress = config.qtum.accounts.tokenOwner.address
    console.log(`[QTUM] Deploying QRC20 token '${name}' (${symbol}) with ${decimals} decimals...`)
    const result = await helperQtum.deployToken(name, symbol, decimals, creatorAddress)
    console.log()
    console.log(result)
    const transactionReceipt = await waitForTransactionQtum(result.txid)
    if (transactionReceipt[0].excepted != 'None') {
        console.log()
        console.log(transactionReceipt)
        throw new Error('[QTUM] Unable to deploy QRC20 token')
    }
    console.log(`[QTUM] QRC20 token '${name}' (${symbol}) deployed at address: 0x${result.address} (${transactionReceipt[0].gasUsed} gas used)`)
    return result
}

async function deployECRecoverPublicKey() {
    const creatorAddress = config.qtum.accounts.l2Owner.address
    console.log('[QTUM] Deploying ECRecoverPublicKey contract...')
    const result = await helperQtum.deployECRecoverPublicKey(creatorAddress)
    const transactionReceipt = await waitForTransactionQtum(result.txid)
    console.log()
    console.log(result)
    if (transactionReceipt[0].excepted != 'None') {
        console.log()
        console.log(transactionReceipt)
        throw new Error('[QTUM] Unable to deploy ECRecoverPublicKey contract')
    }
    console.log(`[QTUM] ECRecoverPublicKey contract deployed at address: 0x${result.address} (${transactionReceipt[0].gasUsed} gas used)`)
    return result
}

async function deployL2Ethereum() {
    const oracleAddress = config.ethereum.accounts.l2Oracle.address
    const creatorAddress = config.ethereum.accounts.l2Owner.address
    const creatorPrivateKey = config.ethereum.accounts.l2Owner.privateKey
    console.log(`[ETHEREUM] Deploying L2 contract with oracle ${oracleAddress}...`)
    const result = await helperEthereum.deployL2(oracleAddress, creatorAddress, creatorPrivateKey)
    if (!result.status) {
        throw new Error('[ETHEREUM] Unable to deploy L2 contract')
    }
    console.log(`[ETHEREUM] L2 contract deployed at address: ${result.contractAddress} (${result.gasUsed} gas used)`)
    return result
}

async function deployL2Qtum(ecrpkAddress) {
    const oracleAddress = helperQtum.addressQtumToAddressEthereum(config.qtum.accounts.l2Oracle.address)
    const creatorAddress = config.qtum.accounts.l2Owner.address
    console.log(`[QTUM] Deploying L2 contract with oracle ${oracleAddress} and ECRecoverPublicKey contract ${ecrpkAddress}...`)
    const result = await helperQtum.deployL2(oracleAddress, ecrpkAddress, creatorAddress)
    console.log()
    console.log(result)
    const transactionReceipt = await waitForTransactionQtum(result.txid)
    if (transactionReceipt[0].excepted != 'None') {
        console.log()
        console.log(transactionReceipt)
        throw new Error('[QTUM] Unable to deploy L2 contract')
    }
    console.log(`[QTUM] L2 contract deployed at address: 0x${result.address} (${transactionReceipt[0].gasUsed} gas used)`)
    return result
}

async function deployTestTokens() {

    for (const symbol in config.ethereum.tokens) {
        const token = config.ethereum.tokens[symbol]
        const result = await deployERC20(token.name, token.symbol, token.decimals)
    }

    for (const symbol in config.qtum.tokens) {
        const token = config.qtum.tokens[symbol]
        const result = await deployQRC20(token.name, token.symbol, token.decimals)
    }
}

async function deployL2() {

    //const l2Ethereum = await deployL2Ethereum()

    const ecrpk = await deployECRecoverPublicKey()
    const l2Qtum = await deployL2Qtum(ecrpk.address)
}

async function prepareEthereum() {

    const amount = '3000000000000000000'
    const etherHolderPrivateKey = config.ethereum.accounts.l2Owner.privateKey

    await helperEthereum.transferEther(amount, config.ethereum.accounts.tokenOwner.address, etherHolderPrivateKey)
    await helperEthereum.transferEther(amount, config.ethereum.accounts.l2Oracle.address, etherHolderPrivateKey)
    await helperEthereum.transferEther(amount, config.ethereum.accounts.userA.address, etherHolderPrivateKey)
    await helperEthereum.transferEther(amount, config.ethereum.accounts.userB.address, etherHolderPrivateKey)
    await helperEthereum.transferEther(amount, config.ethereum.accounts.userC.address, etherHolderPrivateKey)
}

async function prepareQtum() {

    const amountQtum = 10
    
    await helperQtum.transferQtum(config.qtum.accounts.default.address, amountQtum)
    await helperQtum.transferQtum(config.qtum.accounts.tokenOwner.address, amountQtum)
    await helperQtum.transferQtum(config.qtum.accounts.l2Owner.address, amountQtum)
    await helperQtum.transferQtum(config.qtum.accounts.l2Oracle.address, amountQtum)
    await helperQtum.transferQtum(config.qtum.accounts.userA.address, amountQtum)
    await helperQtum.transferQtum(config.qtum.accounts.userB.address, amountQtum)
    await helperQtum.transferQtum(config.qtum.accounts.userC.address, amountQtum)
}

async function prepare() {
    await prepareEthereum()
    await prepareQtum()
}

async function deploy() {
    await deployTestTokens()
    await deployL2()
}

//prepare()
deploy()