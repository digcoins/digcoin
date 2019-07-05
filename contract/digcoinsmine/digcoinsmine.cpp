/*
 * Copyright (C) 2019 Dig Coin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

#include <string>

using namespace eosio;

namespace eosiosystem {
   class system_contract;
}

#define BLOCK_TIME_INTERVAL (0.5)

#define BLOCKS_PER_YEAR ((60 * 60 * 24 * 365) / BLOCK_TIME_INTERVAL)

#define INFLATION_RATE (1.0 / 100)

static inline double calculate_reward(asset token_supply)
{
    return ((token_supply.amount * INFLATION_RATE) / BLOCKS_PER_YEAR);
}

class [[eosio::contract]] digcoinsmine : public contract {
public:
    using contract::contract;

    [[eosio::action]]
    void create(const name &issuer, const asset &maximum_supply)
    {
        require_auth(get_self());

        auto sym = maximum_supply.symbol;
        check(sym.is_valid(), "invalid symbol name");
        check(maximum_supply.is_valid(), "invalid supply");
        check(maximum_supply.amount > 0, "max-supply must be positive");

        stats statstable(get_self(), sym.code().raw());
        auto existing = statstable.find(sym.code().raw());
        check(existing == statstable.end(), "token with symbol already exists");
        check(issuer == get_self(), "issuer must be contract account");

        // make sure a positive quantity reward is possible given the max supply
        double reward_amount = calculate_reward(maximum_supply);
        asset reward = {static_cast<int64_t>(reward_amount), sym};
        check(reward.amount > 0, "Positive rewards are impossible. Increase the max supply or symbol precision.");

        statstable.emplace(get_self(), [&](auto& s) {
            s.supply.symbol = maximum_supply.symbol;
            s.max_supply    = maximum_supply;
            s.issuer        = issuer;
        });
    }

    [[eosio::action]]
    void issue(const name &to, const asset &quantity, const std::string &memo)
    {
        auto sym = quantity.symbol;
        check(sym.is_valid(), "invalid symbol name");
        check(memo.size() <= 256, "memo has more than 256 bytes");

        stats statstable(get_self(), sym.code().raw());
        auto existing = statstable.find(sym.code().raw());
        check(existing != statstable.end(), "token with symbol does not exist, create token before issue");
        const auto& st = *existing;

        require_auth(st.issuer);

        check(to == st.issuer, "tokens can only be issued to issuer account");
        check(st.supply.amount == 0, "this action can only be executed once for a given symbol");

        check(quantity.is_valid(), "invalid quantity");
        check(quantity.amount > 0, "must issue positive quantity");
        check(quantity.amount < st.max_supply.amount, "quantity must be less than maximum supply");
        check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");

        // make sure the initial reward will be >= the smallest possible quantity of an asset
        double reward_amount = calculate_reward(quantity);
        asset reward = {static_cast<int64_t>(reward_amount), sym};
        check(reward.amount > 0, "issue quantity is too small");

        statstable.modify(st, same_payer, [&](auto& s) {
            s.supply += quantity;
        });

        addbalance(st.issuer, quantity, st.issuer);
    }

    [[eosio::action]]
    void transfer(const name &from, const name &to, const asset &quantity, const std::string &memo)
    {
        check(from != to, "cannot transfer to self");
        require_auth(from);
        check(is_account(to), "to account does not exist");

        auto sym = quantity.symbol.code();
        stats statstable(get_self(), sym.raw());
        const auto& st = statstable.get(sym.raw());

        check(from != st.issuer, "Issuer may not transfer tokens.");

        require_recipient(from);
        require_recipient(to);

        check(quantity.is_valid(), "invalid quantity");
        check(quantity.amount > 0, "must transfer positive quantity");
        check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
        check(memo.size() <= 256, "memo has more than 256 bytes");

        auto payer = has_auth(to) ? to : from;

        subbalance(from, quantity);
        addbalance(to, quantity, payer);
    }

    void subbalance(const name &owner, const asset &value)
    {
        accounts from_acnts(get_self(), owner.value);

        const auto& from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
        check(from.balance.amount >= value.amount, "overdrawn balance");

        from_acnts.modify(from, owner, [&](auto& a) {
            a.balance -= value;
        });
    }

    void addbalance(const name &owner, const asset &value, const name &ram_payer)
    {
        accounts to_acnts(get_self(), owner.value);
        auto to = to_acnts.find(value.symbol.code().raw());

        if (to == to_acnts.end()) {
            to_acnts.emplace(ram_payer, [&](auto& a){
                a.balance = value;
            });
        } else {
            to_acnts.modify(to, same_payer, [&](auto& a) {
                a.balance += value;
            });
        }
    }

    [[eosio::action]]
    void open(const name &owner, const symbol &symbol, const name &ram_payer)
    {
        require_auth(ram_payer);

        check(is_account(owner), "owner account does not exist");

        auto sym_code_raw = symbol.code().raw();
        stats statstable(get_self(), sym_code_raw);
        const auto& st = statstable.get(sym_code_raw, "symbol does not exist");
        check(st.supply.symbol == symbol, "symbol precision mismatch");

        accounts acnts(get_self(), owner.value);
        auto it = acnts.find(sym_code_raw);

        if (it == acnts.end()) {
            acnts.emplace(ram_payer, [&](auto& a){
                a.balance = asset{0, symbol};
            });
        }
    }

    [[eosio::action]]
    void close(const name &owner, const symbol &symbol)
    {
        require_auth(owner);
        accounts acnts(get_self(), owner.value);
        auto it = acnts.find(symbol.code().raw());
        check(it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect.");
        check(it->balance.amount == 0, "Cannot close because the balance is not zero.");
        acnts.erase(it);
    }

    [[eosio::action]]
    void mine(const name &miner, const symbol &symbol)
    {
        require_auth(miner);
        check(symbol.is_valid(), "invalid symbol name");

        stats statstable(get_self(), symbol.code().raw());
        auto existing = statstable.find(symbol.code().raw());
        check(existing != statstable.end(), "token with symbol does not exist.");
        const auto& st = *existing;

        check(symbol == st.supply.symbol, "symbol precision mismatch");

        block_timestamp last_reward_time = st.last_reward_time;
        block_timestamp current_time = current_block_time();

        if (last_reward_time >= current_time) {
            std::string memo = "Fail";
            SEND_INLINE_ACTION(*this, miningfail, {{st.issuer, "active"_n}}, {st.issuer, miner, symbol, memo});
            return;
        }

        asset supply = get_supply(get_self(), symbol.code());
        double reward_amount = calculate_reward(supply);

        asset reward = {static_cast<int64_t>(reward_amount), symbol};

        // These checks are probably pointless, but just in case...
        check(reward.is_valid(), "invalid reward quantity");
        check(reward.amount > 0, "must reward positive quantity");
        check(reward.amount <= st.max_supply.amount - st.supply.amount, "This mine is empty. Time to move on.");

        statstable.modify(st, get_self(), [&](auto &s) {
            s.last_reward_time = current_time;
            s.supply += reward;
        });

        require_recipient(get_self());
        require_recipient(miner);

        addbalance(miner, reward, miner);

        std::string memo = "Success!";
        SEND_INLINE_ACTION(*this, miningreward, {{st.issuer, "active"_n}}, {st.issuer, miner, reward, memo});
    }

    [[eosio::action]]
    void miningreward(const name &from, const name &to, const asset &reward, const std::string &memo)
    {
        auto sym_code_raw = reward.symbol.code().raw();
        stats statstable(get_self(), sym_code_raw);
        auto existing = statstable.find(sym_code_raw);
        check(existing != statstable.end(), "token with symbol does not exist.");
        const auto& st = *existing;

        require_auth(st.issuer);
        require_recipient(from);
        require_recipient(to);
    }

    [[eosio::action]]
    void miningfail(const name &from, const name &to, const symbol &symbol, const std::string &memo)
    {
        auto sym_code_raw = symbol.code().raw();
        stats statstable(get_self(), sym_code_raw);
        auto existing = statstable.find(sym_code_raw);
        check(existing != statstable.end(), "token with symbol does not exist.");
        const auto& st = *existing;

        require_auth(st.issuer);
        require_recipient(from);
        require_recipient(to);
    }

    static asset get_supply(const name &token_contract_account, const symbol_code &sym_code)
    {
        stats statstable(token_contract_account, sym_code.raw());
        const auto& st = statstable.get(sym_code.raw());
        return st.supply;
    }

private:
    struct [[eosio::table]] account {
        asset balance;

        uint64_t primary_key()const { return balance.symbol.code().raw(); }
    };

    struct [[eosio::table]] currency_stats {
        asset supply;
        asset max_supply;
        name issuer;
        block_timestamp last_reward_time;

        uint64_t primary_key()const { return supply.symbol.code().raw(); }
    };

    typedef eosio::multi_index< "accounts"_n, account > accounts;
    typedef eosio::multi_index< "stat"_n, currency_stats > stats;
};
