pragma solidity ^0.4.25;


/**
 * @title ERC20 interface
 * @dev Full ERC20 interface described at https://github.com/ethereum/EIPs/issues/20.
 */
contract ERC20 {

  uint256 public totalSupply;

  event Transfer(address indexed from, address indexed to, uint256 value);
  event Approval(address indexed owner, address indexed spender, uint256 value);

  function transfer(address to, uint256 value) public returns (bool);
  function transferFrom(address from, address to, uint256 value) public returns (bool);
  function approve(address spender, uint256 value) public returns (bool);
  function balanceOf(address owner) public view returns (uint256);
  function allowance(address owner, address spender) public view returns (uint256);
}
