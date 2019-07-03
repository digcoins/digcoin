<p align="center">
  <img src="logo/digcoinmed.png"/>
</p>

# Dig Coin
Dig Coin is an eco-friendly mineable cryptocurrency built on-top of EOS platform, the purpose of which is to serve as a solution to the security and centralization problems that plague most EOS-based custom tokens, as well as the inconveniences of using the native EOS token as a currency. See the [whitepaper](whitepaper.md) for details.

## Mining Setup
1. Install the latest version of `cleos` and `Python 3` on your system if they are not already installed. EOS tooklit install instructions can be found [here](https://developers.eos.io/eosio-home/docs/setting-up-your-environment).
2. If necessary, follow [these instructions](https://developers.eos.io/eosio-home/docs/wallets) to set up your cleos wallet.
3. Clone this repo and navigate to its directory.
4. Navigate to the `miner` directory, and open the `config.json` file in your text editor of choice. Fill out the necessary fields.
5. Run the command `python3 minedig.py config.json`
6. Have fun digging, and don't forget to hydrate! 

**DISCLAIMER: This mining software will execute privileged commands using your EOS account's active permission. The author of this software will not be held liable for any damages or loss of funds. Always verify unknown software before running it, and always use best security practices when handling your private keys.**

## Donations
If you find this software useful, a donation of any amount would be greatly appreciated.

EOS account: digcoinminer
