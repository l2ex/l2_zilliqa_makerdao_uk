pragma solidity ^0.4.25;

import './ERC20.sol';
import './SafeMath.sol';


/// @title ERC20 Token standard implementation
contract ERC20Token is ERC20 {
    using SafeMath for uint256;

    string public constant standard = 'Token 0.1';

    uint256 public totalSupply;
    mapping (address => uint256) public balanceOf;
    mapping (address => mapping (address => uint256)) public allowance;

    uint8 public decimals;
    string public name;
    string public symbol;


    constructor(string _name, string _symbol, uint8 _decimals) public {
        name = _name;
        symbol = _symbol;
        decimals = _decimals;
        totalSupply = 10**9 * 10**uint256(decimals);
        balanceOf[msg.sender] = totalSupply;
    }

    function transfer(address _to, uint256 _value) public returns (bool) {
        require(_to != address(0));
        balanceOf[msg.sender] = balanceOf[msg.sender].sub(_value);
        balanceOf[_to] = balanceOf[_to].add(_value);
        emit Transfer(msg.sender, _to, _value);
        return true;
    }

    function transferFrom(address _from, address _to, uint256 _value) public returns (bool) {
        require(_from != address(0) && _to != address(0));
        allowance[_from][msg.sender] = allowance[_from][msg.sender].sub(_value);
        balanceOf[_from] = balanceOf[_from].sub(_value);
        balanceOf[_to] = balanceOf[_to].add(_value);
        emit Transfer(_from, _to, _value);
        return true;
    }

    function approve(address _spender, uint256 _value) public returns (bool) {
        require(_spender != address(0));
        // To change the approve amount you first have to reduce the addresses`
        //  allowance to zero by calling `approve(_spender, 0)` if it is not
        //  already 0 to mitigate the race condition described here:
        //  https://github.com/ethereum/EIPs/issues/20#issuecomment-263524729
        require(_value == 0 || allowance[msg.sender][_spender] == 0);
        allowance[msg.sender][_spender] = _value;
        emit Approval(msg.sender, _spender, _value);
        return true;
    }

    function mint(address _receiver, uint256 _value) public {
        require(_receiver != address(0));
        require(_value > 0);
        balanceOf[_receiver] = balanceOf[_receiver].add(_value);
        totalSupply = totalSupply.add(_value);
        emit Transfer(address(0), _receiver, _value);
    }

    function burn(uint256 _value) public {
        require(_value > 0 && _value <= balanceOf[msg.sender]);
        balanceOf[msg.sender] = balanceOf[msg.sender].sub(_value);
        totalSupply = totalSupply.sub(_value);
        emit Transfer(msg.sender, address(0), _value);
    }

    function totalSupply() public view returns (uint256) {
        return totalSupply;
    }

    function balanceOf(address _owner) public view returns (uint256) {
        return balanceOf[_owner];
    }
    
    function allowance(address _owner, address _spender) public view returns (uint256) {
        return allowance[_owner][_spender];
    }
}