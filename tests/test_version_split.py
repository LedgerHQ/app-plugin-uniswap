import pytest
from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
from ragger.error import ExceptionRAPDU

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

usdt_to_wojak_exact_in_v2 = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                   intermediate_tokens=[],
                                   out_token=tokens.WOJAK.address,
                                   amount_in=240000000000000000,
                                   amount_out=1000000000000000000),
]

usdt_to_wojak_exact_out_v2 = [
    crafter.craft_V2_SWAP_EXACT_OUT(in_token=tokens.USDT.address,
                                    intermediate_tokens=[],
                                    out_token=tokens.WOJAK.address,
                                    amount_in=240000000000000000,
                                    amount_out=1000000000000000000),
]

usdt_to_wojak_exact_in_v3 = [
    crafter.craft_V3_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                   intermediate_tokens=[],
                                   out_token=crafter.TokenAndNetworkFee(address=tokens.WOJAK.address, network_fee="0001f4"),
                                   amount_in=240000000000000000,
                                   amount_out=1000000000000000000),
]

usdt_to_wojak_exact_out_v3 = [
    crafter.craft_V3_SWAP_EXACT_OUT(in_token=tokens.USDT.address,
                                    intermediate_tokens=[],
                                    out_token=crafter.TokenAndNetworkFee(address=tokens.WOJAK.address, network_fee="0001f4"),
                                    amount_in=240000000000000000,
                                    amount_out=1000000000000000000),
]

usdt_to_barl_exact_in_v2 = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                   intermediate_tokens=[],
                                   out_token=tokens.BARL.address,
                                   amount_in=240000000000000000,
                                   amount_out=1000000000000000000),
]

usdt_to_barl_exact_out_v3 = [
    crafter.craft_V3_SWAP_EXACT_OUT(in_token=tokens.USDT.address,
                                    intermediate_tokens=[],
                                    out_token=crafter.TokenAndNetworkFee(address=tokens.BARL.address, network_fee="0001f4"),
                                    amount_in=240000000000000000,
                                    amount_out=1000000000000000000),
]

barl_to_wojak_exact_in_v2 = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.BARL.address,
                                   intermediate_tokens=[],
                                   out_token=tokens.WOJAK.address,
                                   amount_in=240000000000000000,
                                   amount_out=1000000000000000000),
]

barl_to_wojak_exact_out_v3 = [
    crafter.craft_V3_SWAP_EXACT_OUT(in_token=tokens.BARL.address,
                                    intermediate_tokens=[],
                                    out_token=crafter.TokenAndNetworkFee(address=tokens.WOJAK.address, network_fee="0001f4"),
                                    amount_in=240000000000000000,
                                    amount_out=1000000000000000000),
]

class TestMixVersions:

    # Valid versions split #

    def test_valid_v2_exact_in_plus_v3_exact_in(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_in_v2 + usdt_to_wojak_exact_in_v3):
            navigation_helper.ui_validate()

    def test_valid_v2_exact_out_plus_v3_exact_out(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_out_v2 + usdt_to_wojak_exact_out_v3):
            navigation_helper.ui_validate()

    # Different swap types #

    def test_invalid_v2_exact_in_plus_v3_exact_out(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(usdt_to_wojak_exact_in_v2 + usdt_to_wojak_exact_out_v3)
        assert e.value.status == 0x6A80

    def test_invalid_v2_exact_out_plus_v3_exact_in(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(usdt_to_wojak_exact_out_v2 + usdt_to_wojak_exact_in_v3)
        assert e.value.status == 0x6A80

    # Different OUTPUT #

    def test_invalid_exact_in_different_output(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(usdt_to_barl_exact_in_v2 + usdt_to_wojak_exact_in_v3)
        assert e.value.status == 0x6A80

    def test_invalid_exact_out_different_output(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(usdt_to_wojak_exact_out_v2 + usdt_to_barl_exact_out_v3)
        assert e.value.status == 0x6A80

    # Different INPUT #

    def test_invalid_exact_in_different_input(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(barl_to_wojak_exact_in_v2 + usdt_to_wojak_exact_in_v3)
        assert e.value.status == 0x6A80

    def test_invalid_exact_out_different_input(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(usdt_to_wojak_exact_out_v2 + barl_to_wojak_exact_out_v3)
        assert e.value.status == 0x6A80

