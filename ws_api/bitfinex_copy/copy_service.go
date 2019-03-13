package bitfinex_copy



/*
const ws = require('ws')
const w = new ws('wss://api-pub.bitfinex.com/ws/2')

w.on('message', (msg) => console.log(msg))

let msg = JSON.stringify({
event: 'subscribe',
channel: 'book',
prec: 'R0',
symbol: 'tBTCUSD'
})

w.on('open', () => w.send(msg))

*/

const ws = require('ws')
const w = new ws('wss://api-pub.bitfinex.com/ws/2')

w.on('message', (msg) => console.log(msg))

let msg = JSON.stringify({
event: 'subscribe',
channel: 'book',
prec: 'R0',
symbol: 'tETHUSD'
})

w.on('open', () => w.send(msg))
