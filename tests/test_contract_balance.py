import pytest
from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
from ragger.error import ExceptionRAPDU

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens


usdt_to_wojak_exact_in_v3_contract_balance_in = [
    crafter.craft_V3_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                   intermediate_tokens=[],
                                   out_token=crafter.TokenAndNetworkFee(address=tokens.WOJAK.address, network_fee="0001f4"),
                                   amount_in=0x8000000000000000000000000000000000000000000000000000000000000000,
                                   amount_out=0000000000000000000,
                                   recipient="0000000000000000000000000000000000000002"),
]
usdt_to_wojak_exact_in_v3_contract_balance_out = [
    crafter.craft_V3_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                   intermediate_tokens=[],
                                   out_token=crafter.TokenAndNetworkFee(address=tokens.WOJAK.address, network_fee="0001f4"),
                                   amount_in=240000000000000000,
                                   amount_out=0x8000000000000000000000000000000000000000000000000000000000000000,
                                   recipient="Dead7910DbDFdE764fC21FCD4E74D71bBACA6D8D"),
]

class TestContractBalance:

    def test_contract_balance_send(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        uniswap_client.provide_token_metadata(tokens.WOJAK)
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_in_v3_contract_balance_in):
            navigation_helper.ui_validate()

    def test_contract_balance_receive(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        uniswap_client.provide_token_metadata(tokens.WOJAK)
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_in_v3_contract_balance_out):
            navigation_helper.ui_validate()
