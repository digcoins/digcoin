"""
Copyright (C) 2019 Dig Coin

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import subprocess
from errors import *

SYMBOL_CODE = "DIG"
SYMBOL_PRECISION = 4
TOKEN_CONTRACT = "digcoinsmine"

class Cleos_Handler:
    def __init__(self, config):
        self.account = config['account']
        self.wallet_name = config['wallet_name']
        self.wallet_password = config['wallet_password']
        self.cleos_path = config['cleos_path']
        self.verbose = config['verbose_errors']
        self.api_args = self.cleos_path + " --url=" + config['api_url']

        if not self.unlockWallet():
             raise ClassInitError("Failed to unlock cleos wallet")

    def unlockWallet(self):
        s = self.cleos_path + " wallet unlock --name %s --password %s" % (self.wallet_name, self.wallet_password)
        unlockCmd = s.split()

        try:
            subprocess.check_output(unlockCmd)
            return True
        except subprocess.CalledProcessError: # this usually happens when it's already unlocked
            return True
        except Exception as e:
            if self.verbose:
                print("processes exception: %s" % e)

            return False

    def getAccount(self):
        return self.account

    def sendAction(self, account, args):
        command = self.api_args.split() + ["push"] + ["action"] + args + ["-p"] + [account + "@active"]

        try:
            return subprocess.check_output(command, shell=False, timeout=2.0)
        except Exception as e:
            if self.verbose:
                print("processes exception: %s" % (e))

            return None

def send_mine_action(cleos_handler):
    account = cleos_handler.getAccount()
    data = '{"miner":"%s","symbol":"%d,%s"}' % (account, SYMBOL_PRECISION, SYMBOL_CODE)
    args = [TOKEN_CONTRACT] + ["mine"] + [data]
    return cleos_handler.sendAction(account, args)