import pytest
from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
from ragger.error import ExceptionRAPDU

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

AMOUNT_LESS  = 100000
AMOUNT_EQUAL = 200000
AMOUNT_MORE  = 300000

wrap = [
    crafter.craft_WRAP_ETH(amount=AMOUNT_EQUAL),
]
from_weth_exact_in = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.WETH.address,
                                   intermediate_tokens=[],
                                   out_token=tokens.USDT.address,
                                   amount_in=AMOUNT_EQUAL,
                                   amount_out=1000000000000000000),
]

class TestNativeEthAmount:

    def test_refuse_eth_amount_for_token(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WETH)
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(from_weth_exact_in, amount=Web3.to_wei(1, "ether"))
        assert e.value.status == 0x6A80

    def test_accept_eth_amount_for_native_less(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WETH)
        with uniswap_client.send_sign_request(wrap + from_weth_exact_in, amount=AMOUNT_LESS):
            navigation_helper.ui_validate()

    def test_accept_eth_amount_for_native_equal(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WETH)
        with uniswap_client.send_sign_request(wrap + from_weth_exact_in, amount=AMOUNT_EQUAL):
            navigation_helper.ui_validate()

    def test_accept_eth_amount_for_native_more(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WETH)
        with uniswap_client.send_sign_request(wrap + from_weth_exact_in, amount=AMOUNT_MORE):
            navigation_helper.ui_validate()
