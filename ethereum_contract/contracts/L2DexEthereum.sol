pragma solidity ^0.4.25;

import './L2Dex.sol';


contract L2DexEthereum is L2Dex {

    constructor(address _oracle) L2Dex(_oracle) public {
        // Nothing to do here
    }

    // Just use built-in `ecrecover` function
    function recoverSignerAddress(bytes32 dataHash, uint8 v, bytes32 r, bytes32 s) internal returns (address) {
        return ecrecover(dataHash, v, r, s);
    }
}