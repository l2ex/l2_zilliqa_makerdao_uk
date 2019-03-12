module.exports = {
    ethereum: {
        connection: 'http://52.14.93.190:12933',
        accounts: {
            tokenOwner: {
                privateKey: Buffer.from('5a2b7750a35510214889779e702c34051236ea6981defa7fbeadd1a77770fc70', 'hex'),
                address: '0x792dB58835893e047189f4B6639eda85Ac34113f'.toLocaleLowerCase()
            },
            l2Owner: {
                privateKey: Buffer.from('26655fe5ccb52d03c5f6d31b2676ad525a77ada04ffa33fa2878d0ed261bf2e4', 'hex'),
                address: '0x07FA9eDa277336A4F5B452207dfDE07e12eCC7D9'.toLocaleLowerCase()
            },
            l2Oracle: {
                privateKey: Buffer.from('2a57aa6f124f0ee15047326ef66d9150f6c6ab47390b73a77df573667a5379e6', 'hex'),
                address: '0xe671b4223765C6D39203Fd308d0AFe6451396635'.toLocaleLowerCase()
            },
            userA: {
                privateKey: Buffer.from('7b847487bc832ba667513e25d02276083944eb41a025978804f3e7d96eb25625', 'hex'),
                address: '0x0B7D92778316c4AA34A26febFB3C005b077e1117'.toLocaleLowerCase()
            },
            userB: {
                privateKey: Buffer.from('1cc34bbba75f7dae0c966157533fb16d8d7ec73bc807526a3639277f3230e7c3', 'hex'),
                address: '0x416871e599A146a15e3136535F951e4bad71ABB9'.toLocaleLowerCase()
            },
            userC: {
                privateKey: Buffer.from('f40258936a170b35d5f18fa7c49c47c25522fb35a71a46c24d257b13e369c6f9', 'hex'),
                address: '0xAD1219e0A29Fc581De227622535bb2A781bDfDe6'.toLocaleLowerCase()
            }
        },
        tokens: {
            OMG: {
                name: 'OMGToken',
                symbol: 'OMG',
                decimals: 18
            },
            USDT: {
                name: 'Tether',
                symbol: 'USDT',
                decimals: 6
            }
        }
    },
    qtum: {
        connection: {
            address: '52.14.93.190',
            port: '18533',
            username: 'wardencliffe',
            password: 'iYuN5iGNTLlyRi5mbxeBLUF18skVyG2HqNs2ANrUVS4=',
            testnet: true
        },
        accounts: {
            default: {
                account: '',
                wif: 'cTN1pe5FAYzmWgt3fMi9EmRuMDQzmvECxj4P3iaCuFzX9TXBddSH',
                privateKey: Buffer.from('ac7f7510c9e831e1646994d34d16fac9c641260da65c24226f81867e89fd4642', 'hex'),
                addressEthereum: '0xf34e5571697a7581eb9d503e6db8b0e4ae5ae5b2'.toLocaleLowerCase(),
                addressQtum: 'qfjsCNVFxWRWbuNEyyFBRzpxrYGu8Ut3ut'
            },
            tokenOwner: {
                account: 'token_owner',
                wif: 'cMi8wquDg9p6eRbyPBYEeSiSbdLg73GNuwdwwihh3ZRu74SvxaAr',
                privateKey: Buffer.from('03d3507cc2acbb56bd080747e41bab1a1d0670bafe592c6eb1d83fa5247550d6', 'hex'),
                addressEthereum: '0x9d5c9730ee1b64f9967b2525f4ed815e939976e0'.toLocaleLowerCase(),
                addressQtum: 'qXuSATPBQh6C41EmrSuPF5Zud3BJ8BWzTQ'
            },
            l2Owner: {
                account: 'l2_owner',
                wif: 'cTrsDcYvKMP971jYpmd63cDfPszTEsLMyAdgJe41uvvHR1pXtnLi',
                privateKey: Buffer.from('bb571b9b871ee63644d688885fb3e9ef7df791c364e39384ad5e7aae2e85753c', 'hex'),
                addressEthereum: '0x9a697f79f511e7b86977bba0058e426f7396adf1'.toLocaleLowerCase(),
                addressQtum: 'qXdqbhXfpkrheCW2Ugd7sQf956j8ZYuzco'
            },
            l2Oracle: {
                account: 'l2_oracle',
                wif: 'cUeAzjf2cNszfKqVGRiQ1b2AaRa4ifkGmqp1UdVWAix5aXSJp34g',
                privateKey: Buffer.from('d2a5cc9823eec5a02524a1c5de9d58e3d1aeae11aab814ee6a5b5799ed67c8c1', 'hex'),
                addressEthereum: '0xa65ed93c1a20e2af0a44160de76f70d5ca13dbf9'.toLocaleLowerCase(),
                addressQtum: 'qYj4x4zGxVCvMEt66gmNKKEi9EPDX7XJvF'
            },
            userA: {
                account: 'user_a',
                wif: 'cNByYPUSNTr7YP72bTXVzVebNf9U5HEYfJPKSG3fnHx4m5Fp2tqB',
                privateKey: Buffer.from('12257382cc220e8fe53e7d0ab50f398db5ad84504a80b9b46368580359f5bdc6', 'hex'),
                addressEthereum: '0x1c050638d442cbb733ad7082b7446d2afebb52da'.toLocaleLowerCase(),
                addressQtum: 'qL7Y6VoPoGLbCQTSCP2saP2yjkZDtKvsPH'
            },
            userB: {
                account: 'user_b',
                wif: 'cPWDBUtvuQVW2XdPbh5WPSsXDiYNMgVGG3wyeLMXhUtug4e5cFUn',
                privateKey: Buffer.from('395d532025e7f5050cbd92008b9ca1e068ff8b85df0d2701bc50a92d0d923c81', 'hex'),
                addressEthereum: '0xb40f25c3b6b26b80abb0e7604d84ef9a36783259'.toLocaleLowerCase(),
                addressQtum: 'qZySw2czbLzyfX6whqbBdADgjGSRm7pL9u'
            },
            userC: {
                account: 'user_c',
                wif: 'cQrZveYCSphspDVrzgJNzGYU5DJNJixo8q8fkLV1PcKyrbzMxfzk',
                privateKey: Buffer.from('61acb8f5349de3d45c6239e5a2c933071b2eb438ce2ebf4953aefde073a18aa8', 'hex'),
                addressEthereum: '0xde5de05fd839235a644bdb873ba84d5b7828a72e'.toLocaleLowerCase(),
                addressQtum: 'qdq9dGXuJrgXUEhD8FeHXurUvhBKZuBpSF'
            }
        },
        tokens: {
            INK: {
                name: 'INK Coin',
                symbol: 'INK',
                decimals: 9
            },
            BOT: {
                name: 'Bodhi',
                symbol: 'BOT',
                decimals: 8
            },
            QBT: {
                name: 'Qbao',
                symbol: 'QBT',
                decimals: 8
            }
        }
    }
}