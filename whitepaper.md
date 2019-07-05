# DIG Coin White Paper

**July 3, 2019**

**Introduction**

This white paper outlines the critical problems with the prevailing EOS custom token model relating to security and centralization, and proposes a use case for a secure, predictable, and eco-friendly mineable currency token for the EOS ecosystem.

- [Background](#background)
  * [The Problem With Custom Tokens](#problemcustom)
  * [The Problem With EOS Tokens](#problemeos)
- [The DIG Token](#digtoken)
  * [Secure Deployment](#deploy)
  * [Currency Supply](#supply)
  * [Mining](#mining)
    + [Reward Formula](#reward)
- [Conclusion](#conclusion)
- [Footnotes](#foot)

<a name="background"></a>
# Background

With the release of the EOS.IO software there has been a flood of new tokens entering the market. Many dApps have their own centrally controlled, custom-tailored token. Moreover, it is common for dApps and users alike to use the native EOS token as a currency. Although this is sometimes necessary for dApp-specific behavior, in many cases it is not only unnecessary, but inconvenient, redundant, and dangerous.

<a name="problemcustom"></a>
## The Problem With Custom Tokens

When someone creates a custom EOS token, by default they hold the private keys to the token contract. Due to the default mutable nature of EOS smart contracts, this means that they have complete control over the currency supply, and the power to alter the balance of anyone's wallet without leaving a trace on the blockchain. They also have the ability to do things such as alter the total supply, alter the rate of inflation, or disable transactions altogether. It is important to note that these powers extend to any would-be thieves or hackers who get a hold of the private keys. This default behavior is how almost all custom EOS tokens are set up as of writing this.

To put it simply, when you use a custom token, you are putting your complete trust in the person(s) or entity who holds the private keys for the given token contract. This trust is not only being placed in their integrity, but in their competence as developers, and in their ability to secure their private keys. By default, these tokens are not only *not* decentralized, but substantially *more* centralized than even standard fiat currencies.

<a name="problemeos"></a>
## The Problem With EOS tokens

The native EOS token is a utility token and a resource. Although it does not suffer from the same security problems as most custom tokens, its use as a currency may still be problematic for those who wish to stake/utilize their tokens. Additionally, the inflation rate is prone to change, and the supply is unlimited. This makes it much more difficult to make predictions about its future value, and more prone to speculation-based volatility.

<a name="digtoken"></a>
# The DIG Token

The DIG token is an eco-friendly mineable token with a fixed maximum rate of inflation, which inherits all of the underlying security properties of the network on which it is built. The purpose of the DIG token is to provide the EOS ecosystem with a single-purpose currency token which users and dApp developers alike can leverage whenever applicable, rather than having to deal with the complications, security risks, and pitfalls of the prevailing custom token model.

<a name="deploy"></a>
## Secure Deployment

Once deployed and initialized, a multi-signature permission scheme will be set up for the DIG token account's active permission. 4 trusted and independent community organizations along with the deployer of the contract will have equally weighted permissions, meaning that in order to alter the contract or make a transaction with the account on which the contract is deployed, all 5 parties will need to provide a signature using their private keys. Additionally, the account's owner permission will be set to `eosio.null`, which is a system account that can never be used by anyone<a href="#note1" id="note1ref"><sup>1</sup></a>.

This will make the contract effectively immutable except in the case of critical bugs or functionality breaking system changes, and therefore immune to the security issues that custom tokens are typically prone to as previously outlined.

<a name="supply"></a>
## Currency Supply

The maximum DIG supply is to be set to 100,000,000,000 DIG. An initial 1% of the supply (1,000,000,000 DIG) is to be issued to the DIG token contract<a href="#note2" id="note2ref"><sup>2</sup></a>. These tokens are permanently locked in the contract (as are any tokens subsequently transferred to it)<a href="#note3" id="note3ref"><sup>3</sup></a>, and therefore all tokens held by the contract should be considered burned and not counted towards the de facto circulating supply. The purpose of this initial issuance is to bootstrap the mining reward function, which uses the token's circulating supply as a variable. Thereafter, DIG tokens can only be obtained through mining, with a maximum yearly inflation rate of ≈1% of the circulating supply.

<a name="mining"></a>
## Mining

DIG tokens can be mined by sending a transaction with the `mine` action to the DIG token contract. Upon success, the contract calls the `miningreward` action which issues a reward to the miner. Rewards are issued at a maximum rate equal to the block production rate. As the EOS block production rate is half a second per block, a maximum of one reward per half-second is issued. The reward is given to the first account that calls the `mine` action in a given block time interval, therefore making the mining difficulty a linear function of the number of miners for a given block time interval.

If a mining attempt fails, the contract calls the `miningfail` action. This action has no effect other than ensuring that the attempt is recorded on the blockchain, and that the minimum resource cost of a transaction is consumed, thus establishing a cost for mining. As such, the requirement to consume EOS network resources in order to mine replaces the traditional proof of work mining paradigm of requiring energy consumption.

<a name="reward"></a>
### Reward Formula

In contrast to most mined cryptocurrencies, DIG mining rewards grow linearly with the circulating supply, according to a fixed maximum inflation rate. The decision to use this reward scheme as opposed to the common halving scheme popularized by Bitcoin was made as an attempt to mitigate token hoarding and price volatility, while incentivizing actual usage and increasing distribution of the token supply.

Rewards are calculated using a simple function: `reward(s) = s * i / k`

With this formula we can calculate that at maximum mining capacity and with an inflation constant `i := 0.01`, the circulating supply `s` will increase by ≈1% per year, given `k := 63,072,000` blocks produced.

<a name="conclusion"></a>
# Conclusion

In light of the lack of a secure and reliable single-purpose currency token within the EOS ecosystem, DIG Coin proposes to fill that void as the first EOS-based environmentally friendly mineable token.

<a name="foot"></a>
# Footnotes

<a id="note1" href="#note1ref"><sup>1</sup></a> The `eosio.null` account has no associated public keys. It can be thought of as a digital black hole.

<a id="note2" href="#note2ref"><sup>2</sup></a> Once the token supply is greater than zero, the contract's `issue` action is disabled.

<a id="note3" href="#note3ref"><sup>3</sup></a> The `transfer` action is explicitly disabled for outbound token transactions originating from the contract account.
