import pytest
from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
from ragger.error import ExceptionRAPDU

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

class TestPayPortion:

    # Valid versions split #

    def test_valid_pay_portion_min(self, uniswap_client, navigation_helper):
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
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_in_v2 + pay_portion):
            navigation_helper.ui_validate()

    def test_valid_pay_portion_mid(self, uniswap_client, navigation_helper):
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
                                      amount=1011),
        ]

        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_in_v2 + pay_portion):
            navigation_helper.ui_validate()

    def test_valid_pay_portion_and_recipient(self, uniswap_client, navigation_helper):
        weth_to_wojak_recipient_1 = [
            crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.WETH.address,
                                           intermediate_tokens=[],
                                           out_token=tokens.WOJAK.address,
                                           amount_in=1234567,
                                           amount_out=7654321,
                                           recipient="3e2324342bf5b8a1dca42915f0489497203d640e"),
        ]
        pay_portion = [
            crafter.craft_PAY_PORTION(token=tokens.USDT.address,
                                      recipient="0000000000000000000000000000000000000002",
                                      amount=1011),
        ]

        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WETH)
        with uniswap_client.send_sign_request(weth_to_wojak_recipient_1 + pay_portion):
            navigation_helper.ui_validate()

    def test_valid_pay_portion_max(self, uniswap_client, navigation_helper):
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
                                      amount=10000),
        ]

        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_in_v2 + pay_portion):
            navigation_helper.ui_validate()

    def test_invalid_pay_portion_too_much(self, uniswap_client):
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
                                      amount=100001),
        ]

        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(usdt_to_wojak_exact_in_v2 + pay_portion)
        assert e.value.status == 0x6A80

    def test_valid_pay_portion_sum(self, uniswap_client, navigation_helper):
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
                                      amount=50),
        ]

        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_in_v2 + pay_portion + pay_portion):
            navigation_helper.ui_validate()

    def test_valid_pay_portion_above_threshold(self, uniswap_client, navigation_helper):
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
                                      amount=201),
        ]

        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_in_v2 + pay_portion):
            navigation_helper.ui_validate()

    @pytest.mark.parametrize('pay_portion_amount', [1, 200])
    def test_valid_pay_portion_under_threshold(self, uniswap_client, navigation_helper, pay_portion_amount):
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
                                      amount=pay_portion_amount),
        ]

        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_in_v2 + pay_portion):
            navigation_helper.ui_validate()

    def test_invalid_pay_portion_sum_overflow(self, uniswap_client, navigation_helper):
        usdt_to_wojak_exact_in_v2 = [
            crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                           intermediate_tokens=[],
                                           out_token=tokens.WOJAK.address,
                                           amount_in=240000000000000000,
                                           amount_out=1000000000000000000),
        ]
        pay_portion_1 = [
            crafter.craft_PAY_PORTION(token=tokens.USDT.address,
                                      recipient="0000000000000000000000000000000000000002",
                                      amount=1),
        ]
        pay_portion_2 = [
            crafter.craft_PAY_PORTION(token=tokens.USDT.address,
                                      recipient="0000000000000000000000000000000000000002",
                                      amount=10000),
        ]

        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(usdt_to_wojak_exact_in_v2 + pay_portion_1 + pay_portion_2)
        assert e.value.status == 0x6A80
