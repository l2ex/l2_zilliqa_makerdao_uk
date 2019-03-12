pragma solidity ^0.4.25;

import './common/ERC20.sol';
import './common/SafeMath.sol';


/// @title Base L2 DEX smart contract implementation.
contract L2Dex {
    using SafeMath for uint256;

    ///////////////////////////////////////////////////
    // CONTRACT STRUCTURES
    ///////////////////////////////////////////////////

    struct Account {
        // Amount of either ETH/QTUM or tokens available to trade by user
        uint256 balance;
        // Amount of either ETH/QTUM or tokens pending to move to/from (depends on sign of the value) the balance
        int256 change;
        // Amount of either ETH/QTUM or tokens available to withdraw by user
        uint256 withdrawable;
        // Amount of either ETH/QTUM or tokens additionally available to trade by user (only for market makers)
        uint256 credited;
        // Index of the last pushed transaction
        uint256 nonce;
    }

    struct Channel {
        // Channel expiration date (timestamp in seconds)
        uint256 expiration;
        // Accounts related to the channel where key of a map is token address
        // Zero key [address(0)] is used for ETH/QTUM instead of tokens
        mapping(address => Account) accounts;
        // We also save contract owner address at moment of channel creation or extension to make sure
        // all off-chain transactions signed by contract owner will be able to use by channel owner
        // in case if contract owner is changed for emergency (just to make it really reliable)
        address contractOwner;
    }

    struct Credit {
        // Amount of either ETH/QTUM or tokens deposited as credit guaranty
        uint256 deposit;
        // Credit ratio used for the credit
        uint256 ratio;
        // Credit fee per year for the credit where 10**16 means 1% per year
        uint256 fee;
        // Issue credit date (timestamp in seconds)
        uint256 issuedAt;
        // Repay credit date (timestamp in seconds)
        uint256 repaidAt;
        // Last credit fee charge date (timestamp in seconds)
        uint256 feeChargedAt;
    }

    struct MarketMaker {
        // Maximum allowed credit ratio for the market maker
        uint256 creditRatio;
        // Credit fee per year for the market maker where 10**16 means 1% per year
        uint256 creditFee;
        // Flag indicating if the market maker is unregistered
        bool unregistered;
        // Issued credits by index starting from 1
        mapping(uint16 => Credit) credits;
        // Count of issued credits
        uint16 creditCount;
    }

    ///////////////////////////////////////////////////
    // EVENTS
    ///////////////////////////////////////////////////

    event MarketMakerRegistered(address indexed who, uint256 indexed creditRatio, uint256 indexed creditFee);
    event MarketMakerUnregistered(address indexed who);

    event CreditIssued(address indexed debtor, address indexed token, uint16 creditId, uint256 deposit, uint256 credit);
    event CreditRepaid(address indexed debtor, address indexed token, uint16 creditId);
    event CreditFeeCharged(address indexed debtor, address indexed token, uint16 creditId, uint256 amount);

    event DepositInternal(address indexed token, uint256 amount, uint256 balance);
    event WithdrawInternal(address indexed token, uint256 amount, uint256 balance);

    event Deposit(address indexed channelOwner, address indexed token, uint256 amount, uint256 balance);
    event Withdraw(address indexed channelOwner, address indexed token, uint256 amount, uint256 balance);

    event ChannelExtend(address indexed channelOwner, uint256 expiration);
    event ChannelUpdate(
        address indexed channelOwner,
        address indexed token,
        uint256 balance,
        int256 change,
        uint256 withdrawable,
        uint256 nonce
    );

    ///////////////////////////////////////////////////
    // CONTRACT FUNCTIONS
    ///////////////////////////////////////////////////

    /// @dev Constructor sets initial owner and oracle addresses.
    constructor(address _oracle) public {
        require(_oracle != address(0));
        owner = msg.sender;
        oracle = _oracle;
    }

    /// @dev Deposits ETH/QTUM to a channel by user.
    function() public payable {
        deposit(address(0), 0);
    }

    /// @dev Changes owner address by oracle.
    function changeOwner(address _owner) public onlyOracle {
        require(_owner != address(0));
        owner = _owner;
    }

    ///////////////////////////////////////////////////
    // MARKET MAKERS FUNCTIONS
    ///////////////////////////////////////////////////

    /// @dev Registers market maker.
    function registerMarketMaker(address who, uint256 creditRatio, uint256 creditFee) public onlyOwner {
        require(creditRatio > CREDIT_RATIO_DENOMINATOR);
        require(marketMakers[who].creditRatio == 0);
        marketMakers[who] = MarketMaker(creditRatio, creditFee, false, 0);
        emit MarketMakerRegistered(who, creditRatio, creditFee);
    }

    /// @dev Unregisters market maker.
    function unregisterMarketMaker(address who) public onlyOwner {
        require(marketMakers[who].creditRatio > 0);
        marketMakers[who].unregistered = true;
        emit MarketMakerUnregistered(who);
    }

    ///////////////////////////////////////////////////
    // CREDITS FUNCTIONS
    ///////////////////////////////////////////////////

    /// @dev Deposits ETH/QTUM or tokens to a channel by market maker and take a credit.
    function takeCredit(address token, uint256 amount) public notExpired payable {
        MarketMaker storage marketMaker = marketMakers[msg.sender];
        require(marketMaker.creditRatio > 0 && !marketMaker.unregistered);
        uint256 creditRatio = CREDIT_RATIO_DEFAULT; // TODO: Take from function arguments?
        //require(creditRatio > CREDIT_RATIO_DENOMINATOR && creditRatio <= marketMaker.creditRatio);
        uint256 depositAmount = 0;
        if (token != address(0)) {
            require(msg.value == 0 && amount > 0);
            depositAmount = amount;
            // Transfer tokens from the sender to the contract and check result
            // Note: At least specified amount of tokens should be allowed to spend by the contract before deposit!
            require(ERC20(token).transferFrom(msg.sender, this, depositAmount));
        } else {
            require(msg.value > 0 && amount == 0);
            depositAmount = msg.value;
        }
        uint256 creditAmount = depositAmount.mul(creditRatio).div(CREDIT_RATIO_DENOMINATOR).sub(depositAmount);
        Channel storage channel = channels[msg.sender];
        if (channel.expiration == 0) {
            channel.expiration = now.add(TTL_DEFAULT);
            channel.contractOwner = owner;
            emit ChannelExtend(msg.sender, channel.expiration);
        }
        Account storage account = channel.accounts[token];
        account.balance = account.balance.add(depositAmount).add(creditAmount);
        account.credited = account.credited.add(creditAmount);
        uint16 creditId = marketMaker.creditCount + 1;
        marketMaker.credits[creditId] = Credit(depositAmount, creditRatio, marketMaker.creditFee, now, 0, 0);
        marketMaker.creditCount = marketMaker.creditCount + 1;
        emit CreditIssued(msg.sender, token, creditId, depositAmount, creditAmount);
        emit Deposit(msg.sender, token, depositAmount.add(creditAmount), account.balance);
        emit ChannelUpdate(msg.sender, token, account.balance, account.change, account.withdrawable, account.nonce);
    }

    /// @dev Withdraws ETH/QTUM or tokens from a market maker channel and repay the credit.
    function repayCredit(address token, uint16 creditId) public notExpired {
        MarketMaker storage marketMaker = marketMakers[msg.sender];
        require(marketMaker.creditRatio > 0 && !marketMaker.unregistered);
        require(creditId >= 1 && creditId <= marketMaker.creditCount);
        Credit storage credit = marketMaker.credits[creditId];
        require(credit.issuedAt > 0 && credit.repaidAt == 0);
        uint256 creditAmount = credit.deposit.mul(credit.ratio).div(CREDIT_RATIO_DENOMINATOR).sub(credit.deposit);
        uint256 feeDuration = now.sub(credit.issuedAt);
        uint256 feeAmount = feeDuration.mul(creditAmount).div(SECONDS_IN_YEAR);
        Channel storage channel = channels[msg.sender];
        Account storage account = channel.accounts[token];
        account.balance = account.balance.sub(credit.deposit).sub(creditAmount).sub(feeAmount);
        account.credited = account.credited.sub(creditAmount);
        credit.repaidAt = now;
        credit.feeChargedAt = now;
        emit CreditFeeCharged(msg.sender, token, creditId, feeAmount);
        emit CreditRepaid(msg.sender, token, creditId);
        emit Withdraw(msg.sender, token, credit.deposit.add(creditAmount), account.balance);
        emit ChannelUpdate(msg.sender, token, account.balance, account.change, account.withdrawable, account.nonce);
    }

    /// @dev Withdraws ETH/QTUM or tokens from a market maker channel to pay the credit fee.
    function chargeCreditFee(address debtor, address token, uint16 creditId) public onlyOwner {
        MarketMaker storage marketMaker = marketMakers[debtor];
        require(marketMaker.creditRatio > 0 && !marketMaker.unregistered);
        require(creditId >= 1 && creditId <= marketMaker.creditCount);
        Credit storage credit = marketMaker.credits[creditId];
        require(credit.issuedAt > 0 && credit.repaidAt == 0);
        uint256 creditAmount = credit.deposit.mul(credit.ratio).div(CREDIT_RATIO_DENOMINATOR).sub(credit.deposit);
        uint256 feeDuration = now.sub(credit.issuedAt);
        uint256 feeAmount = feeDuration.mul(creditAmount).div(SECONDS_IN_YEAR);
        Account storage account = channels[debtor].accounts[token];
        account.balance = account.balance.sub(feeAmount);
        credit.feeChargedAt = now;
        emit CreditFeeCharged(debtor, token, creditId, feeAmount);
        emit ChannelUpdate(debtor, token, account.balance, account.change, account.withdrawable, account.nonce);
    }

    ///////////////////////////////////////////////////
    // INTERNAL BALANCES FUNCTIONS
    ///////////////////////////////////////////////////

    /// @dev Deposits ETH/QTUM or tokens to the contract balance.
    function depositInternal(address token, uint256 amount) public payable {
        uint256 depositAmount = 0;
        if (token != address(0)) {
            require(msg.value == 0 && amount > 0);
            depositAmount = amount;
            // Transfer tokens from the sender to the contract and check result
            // Note: At least specified amount of tokens should be allowed to spend by the contract before deposit!
            require(ERC20(token).transferFrom(msg.sender, this, depositAmount));
        } else {
            require(msg.value > 0 && amount == 0);
            depositAmount = msg.value;
        }
        balances[token] = balances[token].add(depositAmount);
        emit DepositInternal(token, depositAmount, balances[token]);
    }

    /// @dev Withdraws specified amount of ETH/QTUM or tokens to the contract owner.
    function withdrawInternal(address token, uint256 amount) public onlyOwner {
        require(amount > 0 && amount <= balances[token]);
        if (token != address(0)) {
            require(ERC20(token).transfer(owner, amount));
        } else {
            owner.transfer(amount);
        }
        balances[token] = balances[token].sub(amount);
        emit WithdrawInternal(token, amount, balances[token]);
    }

    ///////////////////////////////////////////////////
    // CHANNEL BALANCES FUNCTIONS
    ///////////////////////////////////////////////////

    /// @dev Deposits ETH/QTUM or tokens to a channel by user.
    function deposit(address token, uint256 amount) public notExpired payable {
        uint256 depositAmount = 0;
        if (token != address(0)) {
            require(msg.value == 0 && amount > 0);
            depositAmount = amount;
            // Transfer tokens from the sender to the contract and check result
            // Note: At least specified amount of tokens should be allowed to spend by the contract before deposit!
            require(ERC20(token).transferFrom(msg.sender, this, depositAmount));
        } else {
            require(msg.value > 0 && amount == 0);
            depositAmount = msg.value;
        }
        Channel storage channel = channels[msg.sender];
        if (channel.expiration == 0) {
            channel.expiration = now.add(TTL_DEFAULT);
            channel.contractOwner = owner;
            emit ChannelExtend(msg.sender, channel.expiration);
        }
        Account storage account = channel.accounts[token];
        account.balance = account.balance.add(depositAmount);
        emit Deposit(msg.sender, token, depositAmount, account.balance);
        emit ChannelUpdate(msg.sender, token, account.balance, account.change, account.withdrawable, account.nonce);
    }

    /// @dev Withdraws specified amount of ETH/QTUM or tokens to user.
    function withdraw(address token, uint256 amount) public canWithdraw(token) {
        Channel storage channel = channels[msg.sender];
        Account storage account = channel.accounts[token];
        // Check if channel is expired and there is something we should change in channel
        if (isChannelExpired(channel)) {
            // Before widthdraw it is necessary to apply current balance change
            updateBalance(account, token);
            // Before widthdraw it is also necessary to update withdrawable amount
            updateWithdrawable(account, account.balance);
        }
        require(amount > 0 && amount <= account.withdrawable);
        if (token != address(0)) {
            require(ERC20(token).transfer(msg.sender, amount));
        } else {
            msg.sender.transfer(amount);
        }
        account.withdrawable = account.withdrawable.sub(amount);
        emit Withdraw(msg.sender, token, amount, account.balance);
        emit ChannelUpdate(msg.sender, token, account.balance, account.change, account.withdrawable, account.nonce);
    }

    ///////////////////////////////////////////////////
    // CHANNEL FUNCTIONS
    ///////////////////////////////////////////////////

    /// @dev Extends expiration of sender's channel.
    function extendChannel(uint256 ttl) public {
        require(ttl >= TTL_MIN);
        Channel storage channel = channels[msg.sender];
        uint256 expiration = now.add(ttl);
        require(channel.expiration > 0 && channel.expiration < expiration);
        channel.expiration = expiration;
        channel.contractOwner = owner;
        emit ChannelExtend(msg.sender, channel.expiration);
    }

    /// @dev Pushes off-chain transaction with most recent balance change by user or by contract owner.
    /// @param channelOwner Owner address of channel for which off-chain transaction is signed.
    /// @param token Address of token contract to identify the channel (zero address used for ETH/QTUM).
    /// @param change New balance change value.
    /// @param nonce Index of off-chain transaction inside the channel (counted separately for each channel and token).
    /// @param apply Flag indicating if new balance change should be applied to the channel balance.
    /// @param free Amount of currency which will be allowed to withdraw (moved from `balance` to `withdrawable`).
    /// @param v Signature: recovery value in range from 27 to 30.
    /// @param r Signature: first 32 bytes.
    /// @param s Signature: second 32 bytes.
    function updateChannel(
        address channelOwner,
        address token,
        int256 change,
        uint256 nonce,
        bool apply,
        uint256 free,
        uint8 v,
        bytes32 r,
        bytes32 s
    )
        public
    {
        Channel storage channel = channels[channelOwner];
        Account storage account = channel.accounts[token];
        require(channel.expiration > 0 && nonce > account.nonce);
        require(change >= 0 || account.balance >= uint256(-change));
        // Make sure signature is valid and recover signer address
        bytes32 messageHash = keccak256(abi.encodePacked(channelOwner, token, change, nonce, apply, free));
        address signer = recoverSignerAddress(messageHash, v, r, s);
        if (signer == channelOwner) {
            // Transaction signed by user who owns the channel
            // Only contract owner can push off-chain transactions signed by channel owner if the channel not expired
            require(isChannelExpired(channel) || msg.sender == owner);
        } else if (signer == owner || signer == channel.contractOwner) {
            // Transaction signed by the contract owner
            // Only channel owner can push off-chain transactions signed by contract owner if the channel not expired
            require(isChannelExpired(channel) || msg.sender == channelOwner);
        } else {
            // Specified arguments are not valid
            revert();
        }
        account.change = change;
        if (signer == owner || signer == channel.contractOwner) {
            if (apply) {
                // Applying balance change to a account balance is requested so just do it
                updateBalance(account, token);
            }
            if (free > 0) {
                // Move requested amount of currency from `balance` to `withdrawable`
                updateWithdrawable(account, free);
            }
        }
        account.nonce = nonce;
        emit ChannelUpdate(channelOwner, token, account.balance, account.change, account.withdrawable, account.nonce);
    }

    ///////////////////////////////////////////////////
    // READ FUNCTIONS
    ///////////////////////////////////////////////////

    /// @dev Returns internal balance of the contract.
    function getBalanceInternal(address token) public view returns (uint256) {
        return balances[token];
    }

    /// @dev Returns channel expiration time as timestamp in seconds.
    function getExpiration(address channelOwner) public view returns (uint256) {
        return channels[channelOwner].expiration;
    }

    /// @dev Returns channel balance.
    function getBalance(address channelOwner, address token) public view returns (uint256) {
        return channels[channelOwner].accounts[token].balance;
    }

    /// @dev Returns channel balance change pending to move to/from channel balance.
    function getBalanceChange(address channelOwner, address token) public view returns (int256) {
        return channels[channelOwner].accounts[token].change;
    }

    /// @dev Returns amount of currency available to withdraw from channel by user.
    function getBalanceWithdrawable(address channelOwner, address token) public view returns (uint256) {
        return channels[channelOwner].accounts[token].withdrawable;
    }

    /// @dev Returns still available channel balance to use (balance + change).
    function getBalanceAvailable(address channelOwner, address token) public view returns (uint256) {
        Account memory account = channels[channelOwner].accounts[token];
        return calcBalanceApplied(account);
    }

    /// @dev Returns total channel balance (balance + change + withdrawable).
    function getBalanceTotal(address channelOwner, address token) public view returns (uint256) {
        Account memory account = channels[channelOwner].accounts[token];
        return calcBalanceApplied(account).add(account.withdrawable);
    }

    /// @dev Returns channel balance credited (only for market makers).
    function getBalanceCredited(address channelOwner, address token) public view returns (uint256) {
        return channels[channelOwner].accounts[token].credited;
    }

    /// @dev Returns index of last pushed channel update transaction.
    function getNonce(address channelOwner, address token) public view returns (uint256) {
        return channels[channelOwner].accounts[token].nonce;
    }

    ///////////////////////////////////////////////////
    // INTERNAL FUNCTIONS
    ///////////////////////////////////////////////////

    /// @dev Virtual function which should return signer address which can be compared to `msg.sender`.
    function recoverSignerAddress(bytes32 dataHash, uint8 v, bytes32 r, bytes32 s) internal returns (address);

    ///////////////////////////////////////////////////
    // PRIVATE FUNCTIONS
    ///////////////////////////////////////////////////

    /// @dev Updates account balance according to balance change value.
    function updateBalance(Account storage account, address token) private {
        if (account.change > 0) {
            balances[token] = balances[token].sub(uint256(account.change));
            account.balance = account.balance.add(uint256(account.change));
            account.change = 0;
        } else if (account.change < 0) {
            account.balance = account.balance.sub(uint256(-account.change));
            balances[token] = balances[token].add(uint256(-account.change));
            account.change = 0;
        }
    }

    /// @dev Updates amount of ETH/QTUM or tokens allowed to withdraw by user.
    function updateWithdrawable(Account storage account, uint256 free) private {
        if (free > 0) {
            require(free <= account.balance);
            account.balance = account.balance.sub(free);
            account.withdrawable = account.withdrawable.add(free);
        }
    }

    /// @dev Returns true if channel timeout is expired (only for existing channel).
    function isChannelExpired(Channel memory channel) private view returns (bool) {
        return channel.expiration > 0 && channel.expiration <= now;
    }

    /// @dev Returns balance with considering balance change.
    function calcBalanceApplied(Account memory account) private pure returns (uint256) {
        if (account.change > 0) {
            return account.balance.add(uint256(account.change));
        } else if (account.change < 0) {
            return account.balance.sub(uint256(-account.change));
        } else {
            return account.balance;
        }
    }

    ///////////////////////////////////////////////////
    // MODIFIERS
    ///////////////////////////////////////////////////

    /// @dev Throws if called by any account other than the owner.
    modifier onlyOwner() {
        require(msg.sender == owner);
        _;
    }

    /// @dev Throws if called by any account other than the oracle.
    modifier onlyOracle() {
        require(msg.sender == oracle);
        _;
    }

    /// @dev Throws if called by any account other than one of registered market makers.
    modifier onlyMarketMaker() {
        require(marketMakers[msg.sender].creditRatio > 0 && !marketMakers[msg.sender].unregistered);
        _;
    }

    /// @dev Throws if channel exists and expired.
    modifier notExpired() {
        require(!isChannelExpired(channels[msg.sender]));
        _;
    }

    /// @dev Throws if channel cannot be withdrawn.
    modifier canWithdraw(address token) {
        // There should be something that can be withdrawn on the channel
        require(getBalanceTotal(msg.sender, token) > 0);
        // The channel should be either prepared for withdraw by owner or expired
        require(getBalanceWithdrawable(msg.sender, token) > 0 || isChannelExpired(channels[msg.sender]));
        _;
    }

    ///////////////////////////////////////////////////
    // CONTRACT MEMBERS
    ///////////////////////////////////////////////////

    // Address of account which has all permissions to manage channels
    address public owner;
    // Reserved address that can be used only to change owner for emergency
    address public oracle;

    // Addresses of registered market makers
    mapping(uint64 => address) public marketMakerAddresses;
    // Count of registered market makers
    uint64 public marketMakerCount;

    // Existing channels where key of map is address of account which owns a channel
    mapping(address => Channel) private channels;

    // Amount of ETH/QTUM or tokens owned by the contract
    // Zero key [address(0)] is used for ETH/QTUM instead of tokens
    mapping(address => uint256) private balances;

    // Registered market makers
    mapping(address => MarketMaker) private marketMakers;

    // Minimal TTL that can be used to extend existing channel
    uint256 constant public TTL_MIN = 1 days;
    // Initial TTL for new channels created just after the first deposit
    uint256 constant public TTL_DEFAULT = 20 days;
    // Default credit ratio for market makers (10^18 equals to ratio 1)
    uint256 constant public CREDIT_RATIO_DEFAULT = 10 * CREDIT_RATIO_DENOMINATOR;

    // Denominator of credit ratio for market makers
    uint256 constant private CREDIT_RATIO_DENOMINATOR = 10**18;
    // Amount of seconds in a year
    uint256 constant private SECONDS_IN_YEAR = 31536000;
}