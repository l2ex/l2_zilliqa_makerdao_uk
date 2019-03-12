pragma solidity ^0.4.25;

import "./ERC20.sol";


/// @title QRC20 Token interface
contract QRC20 is ERC20 {
    
    function name() public view returns (string);
    function symbol() public view returns (string);
    function decimals() public view returns (uint8);
}