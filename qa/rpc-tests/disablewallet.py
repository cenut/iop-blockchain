#!/usr/bin/env python3
# Copyright (c) 2015-2016 The Bitcoin Core developers

# This program is free software: you can redistribute it and/or modify\n# it under the terms of the GNU General Public License as published by\n# the Free Software Foundation, either version 3 of the License, or\n# (at your option) any later version.\n\n# This program is distributed in the hope that it will be useful,\n# but WITHOUT ANY WARRANTY; without even the implied warranty of\n# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the \n# GNU General Public License for more details.\n\n# You should have received a copy of the GNU General Public License\n# along with this program. If not, see <http://www.gnu.org/licenses/>.#

#
# Exercise API with -disablewallet.
#

from test_framework.test_framework import IoPTestFramework
from test_framework.util import *


class DisableWalletTest (IoPTestFramework):

    def __init__(self):
        super().__init__()
        self.setup_clean_chain = True
        self.num_nodes = 1

    def setup_network(self, split=False):
        self.nodes = start_nodes(self.num_nodes, self.options.tmpdir, [['-disablewallet']])
        self.is_network_split = False
        self.sync_all()

    def run_test (self):
        # Check regression: https://github.com/IoP/IoP/issues/6963#issuecomment-154548880
        x = self.nodes[0].validateaddress('3J98t1WpEZ73CNmQviecrnyiWrnqRhWNLy')
        assert(x['isvalid'] == False)
        x = self.nodes[0].validateaddress('mneYUmWYsuk7kySiURxCi3AGxrAqZxLgPZ')
        assert(x['isvalid'] == True)

        # Checking mining to an address without a wallet
        try:
            self.nodes[0].generatetoaddress(1, 'mneYUmWYsuk7kySiURxCi3AGxrAqZxLgPZ')
        except JSONRPCException as e:
            assert("Invalid address" not in e.error['message'])
            assert("ProcessNewBlock, block not accepted" not in e.error['message'])
            assert("Couldn't create new block" not in e.error['message'])

        try:
            self.nodes[0].generatetoaddress(1, '3J98t1WpEZ73CNmQviecrnyiWrnqRhWNLy')
            raise AssertionError("Must not mine to invalid address!")
        except JSONRPCException as e:
            assert("Invalid address" in e.error['message'])

if __name__ == '__main__':
    DisableWalletTest ().main ()
