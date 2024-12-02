import pytest
from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
from ragger.error import ExceptionRAPDU

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

BIP32_PATH = "m/44'/60'/0'/0/0"

class TestTrailingUnknownParameters:

    def test_trailing_parameter(self, uniswap_client, navigation_helper, backend, uniswap_contract_data):
        usdt_to_wojak_exact_in_v2 = [
            crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                           intermediate_tokens=[],
                                           out_token=tokens.WOJAK.address,
                                           amount_in=240000000000000000,
                                           amount_out=1000000000000000000),
        ]
        pay_portion = [
            crafter.craft_PAY_PORTION(token=tokens.USDT.address,
                                      recipient="0000000000000000000000000000000000000002",
                                      amount=1),
        ]

        uniswap_client.set_external_plugin()

        data = crafter.craft_uniswap_tx(usdt_to_wojak_exact_in_v2 + pay_portion, uniswap_contract_data)
        data += "9999999999999999999999999999999999999999999999999999999999999999"

        with uniswap_client.send_raw_sign_request(data):
            navigation_helper.ui_validate()
