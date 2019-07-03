#!/usr/bin/python3

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

from sys import exit, argv
import json
from time import sleep

from cleos import Cleos_Handler
import cleos as cl

def load_config(path):
    fp = open(path, 'r')

    try:
        return json.loads(fp.read())['config']
    except:
        raise
    finally:
        fp.close()

def mine(cleos_handler):
    while True:
        cl.send_mine_action(cleos_handler)
        sleep(0.01)

def main(config_path):
    config = load_config(config_path)
    C = Cleos_Handler(config['cleos'])
    mine(C)

if __name__ == '__main__':
    if len(argv) != 2:
        print("Usage: ./minedig.py [config_path]")
        exit(1)

    main(argv[1])
