import pytest
from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
from ragger.error import ExceptionRAPDU

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

AMOUNT_LESS     = 24000000000000000
AMOUNT_STANDARD = 240000000000000000
AMOUNT_MORE     = 2400000000000000000

from_weth_exact_in = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.WETH.address,
                                   intermediate_tokens=[],
                                   out_token=tokens.USDT.address,
                                   amount_in=AMOUNT_STANDARD,
                                   amount_out=1000000000000000000),
]

from_weth_exact_out = [
    crafter.craft_V2_SWAP_EXACT_OUT(in_token=tokens.WETH.address,
                                    intermediate_tokens=[],
                                    out_token=tokens.USDT.address,
                                    amount_in=AMOUNT_STANDARD,
                                    amount_out=1000000000000000000),
]

to_weth_exact_in_self = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                   intermediate_tokens=[],
                                   out_token=tokens.WETH.address,
                                   amount_in=10000000000000000000,
                                   amount_out=AMOUNT_STANDARD,
                                   recipient="0000000000000000000000000000000000000001"),
]
to_weth_exact_in_router = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                   intermediate_tokens=[],
                                   out_token=tokens.WETH.address,
                                   amount_in=10000000000000000000,
                                   amount_out=AMOUNT_STANDARD,
                                   recipient="0000000000000000000000000000000000000002"),
]
to_weth_exact_in_third = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                   intermediate_tokens=[],
                                   out_token=tokens.WETH.address,
                                   amount_in=10000000000000000000,
                                   amount_out=AMOUNT_STANDARD,
                                   recipient="00000000000000000000000000000000000ABCDE"),
]


to_weth_exact_out_self = [
    crafter.craft_V2_SWAP_EXACT_OUT(in_token=tokens.USDT.address,
                                    intermediate_tokens=[],
                                    out_token=tokens.WETH.address,
                                    amount_in=10000000000000000000,
                                    amount_out=AMOUNT_STANDARD,
                                    recipient="0000000000000000000000000000000000000001"),
]
to_weth_exact_out_router = [
    crafter.craft_V2_SWAP_EXACT_OUT(in_token=tokens.USDT.address,
                                    intermediate_tokens=[],
                                    out_token=tokens.WETH.address,
                                    amount_in=10000000000000000000,
                                    amount_out=AMOUNT_STANDARD,
                                    recipient="0000000000000000000000000000000000000002"),
]
to_weth_exact_out_third = [
    crafter.craft_V2_SWAP_EXACT_OUT(in_token=tokens.USDT.address,
                                    intermediate_tokens=[],
                                    out_token=tokens.WETH.address,
                                    amount_in=10000000000000000000,
                                    amount_out=AMOUNT_STANDARD,
                                    recipient="00000000000000000000000000000000000ABCDE"),
]

wrap_less = [
    crafter.craft_WRAP_ETH(amount=AMOUNT_LESS),
]
wrap = [
    crafter.craft_WRAP_ETH(amount=AMOUNT_STANDARD),
]
wrap_more = [
    crafter.craft_WRAP_ETH(amount=AMOUNT_MORE),
]

unwrap_zero = [
    crafter.craft_UNWRAP_WETH(amount=0),
]
unwrap_less = [
    crafter.craft_UNWRAP_WETH(amount=AMOUNT_LESS),
]
unwrap = [
    crafter.craft_UNWRAP_WETH(amount=AMOUNT_STANDARD),
]
unwrap_to_self_1 = [
    crafter.craft_UNWRAP_WETH(amount=AMOUNT_STANDARD, recipient="0000000000000000000000000000000000000001"),
]
unwrap_to_self_2 = [
    crafter.craft_UNWRAP_WETH(amount=AMOUNT_STANDARD, recipient="Dad77910DbDFdE764fC21FCD4E74D71bBACA6D8D"),
]
unwrap_to_third = [
    crafter.craft_UNWRAP_WETH(amount=AMOUNT_STANDARD, recipient="00000000000000000000000000000000000ABCDE"),
]
unwrap_more = [
    crafter.craft_UNWRAP_WETH(amount=AMOUNT_MORE),
]


class TestWrapUnwrap:

    #############
    # Test WRAP #
    #############

    # Can wrap a WETH input #

    def test_valid_wrap_ordered(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(wrap + from_weth_exact_in):
            navigation_helper.ui_validate()

    def test_valid_wrap_unordered(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(from_weth_exact_in + wrap):
            navigation_helper.ui_validate()

    # Can't wrap a WETH output #

    def test_invalid_wrap_ordered(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(wrap + to_weth_exact_in_self)
        assert e.value.status == 0x6A80

    def test_invalid_wrap_unordered(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(to_weth_exact_in_self + wrap)
        assert e.value.status == 0x6A80

    ###############
    # Test UNWRAP #
    ###############

    # Can unwrap a WETH output #

    def test_valid_unwrap_ordered(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(to_weth_exact_in_router + unwrap_to_self_1):
            navigation_helper.ui_validate()

    def test_valid_unwrap_ordered_2(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(to_weth_exact_in_router + unwrap_to_self_2):
            navigation_helper.ui_validate()

    def test_valid_unwrap_ordered_to_third(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(to_weth_exact_in_router + unwrap_to_third):
            navigation_helper.ui_validate()

    def test_invalid_unwrap_unordered(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_async_sign_request(unwrap + to_weth_exact_in_self)
        assert e.value.status == 0x6A80

    # Can't unwrap a WETH input #

    def test_invalid_unwrap_ordered(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(unwrap + from_weth_exact_in)
        assert e.value.status == 0x6A80

    def test_invalid_unwrap_unordered(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(from_weth_exact_in + unwrap)
        assert e.value.status == 0x6A80

    ############################
    # Test WRAP or UNWRAP only #
    ############################

    def test_invalid_unwrap_only(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(unwrap)
        assert e.value.status == 0x6A80

    def test_invalid_wrap_only(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(wrap)
        assert e.value.status == 0x6A80

    ###############################
    # Test WRAP or UNWRAP amounts #
    ###############################

    # Exact IN #

    def test_valid_exact_in_wrap_less(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(wrap_less + from_weth_exact_in):
            navigation_helper.ui_validate()

    def test_valid_exact_in_wrap_more(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(wrap_more + from_weth_exact_in):
            navigation_helper.ui_validate()

    def test_valid_unexact_in_wrap_weth_less(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(to_weth_exact_in_router + unwrap_less):
            navigation_helper.ui_validate()

    # Exact OUT #

    def test_valid_exact_out_wrap_less(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(wrap_less + from_weth_exact_out):
            navigation_helper.ui_validate()

    def test_valid_unexact_out_unwrap_less(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(to_weth_exact_out_router + unwrap_zero):
            navigation_helper.ui_validate()

    def test_valid_exact_out_unwrap_more(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(to_weth_exact_out_router + unwrap_more):
            navigation_helper.ui_validate()

    #################################
    # Test WRAP or UNWRAP recipient #
    #################################

    def test_invalid_wrap_recipient_is_unknown(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request([crafter.craft_WRAP_ETH(amount=AMOUNT_STANDARD, recipient="1230000000000000000000000000000000000001")] + from_weth_exact_in)
        assert e.value.status == 0x6A80

    def test_valid_unwrap_recipient_is_unknown(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(to_weth_exact_in_router + [crafter.craft_UNWRAP_WETH(amount=AMOUNT_STANDARD, recipient="1230000000000000000000000000000000000001")]):
            navigation_helper.ui_validate()

    def test_valid_unwrap_recipient_is_router(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(to_weth_exact_in_router + [crafter.craft_UNWRAP_WETH(amount=AMOUNT_STANDARD, recipient="0000000000000000000000000000000000000002")]):
            navigation_helper.ui_validate()

    def test_valid_wrap_recipient_is_router(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request([crafter.craft_WRAP_ETH(amount=AMOUNT_STANDARD, recipient="0000000000000000000000000000000000000002")] + from_weth_exact_in):
            navigation_helper.ui_validate()

    def test_valid_unwrap_recipient_is_self_1(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(to_weth_exact_in_router + [crafter.craft_UNWRAP_WETH(amount=AMOUNT_STANDARD, recipient="0000000000000000000000000000000000000001")]):
            navigation_helper.ui_validate()

    def test_valid_unwrap_recipient_is_self_2(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(to_weth_exact_in_router + [crafter.craft_UNWRAP_WETH(amount=AMOUNT_STANDARD, recipient="Dad77910DbDFdE764fC21FCD4E74D71bBACA6D8D")]):
            navigation_helper.ui_validate()

    def test_valid_wrap_recipient_is_self_1(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request([crafter.craft_WRAP_ETH(amount=AMOUNT_STANDARD, recipient="0000000000000000000000000000000000000001")] + from_weth_exact_in):
            navigation_helper.ui_validate()

    def test_valid_wrap_recipient_is_self_2(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request([crafter.craft_WRAP_ETH(amount=AMOUNT_STANDARD, recipient="Dad77910DbDFdE764fC21FCD4E74D71bBACA6D8D")] + from_weth_exact_in):
            navigation_helper.ui_validate()
