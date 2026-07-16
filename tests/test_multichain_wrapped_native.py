import pytest

from ragger.error import ExceptionRAPDU

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens
from .uniswap_client import UniswapClient

# Wrapped-native token addresses per chain, mirroring src/weth_token.c
WETH_BASE = "4200000000000000000000000000000000000006"
WBNB_BNB = "bb4cdb9cbd36b01bd1cbaebf2de08d9173bc095c"

AMOUNT_STANDARD = 240000000000000000


class TestMultichainWrappedNative:

    # A wrap + swap on Base must merge into a single ETH -> token display,
    # exactly like on mainnet, using the Base WETH address from the table.
    def test_base_wrap_and_swap(self, backend, uniswap_contract_data, navigation_helper):
        client = UniswapClient(backend, uniswap_contract_data, chain_id=8453)
        client.set_external_plugin()
        client.provide_token_metadata(tokens.USDC_BASE)
        commands = [
            crafter.craft_WRAP_ETH(amount=AMOUNT_STANDARD),
            crafter.craft_V2_SWAP_EXACT_IN(in_token=WETH_BASE,
                                           intermediate_tokens=[],
                                           out_token=tokens.USDC_BASE.address,
                                           amount_in=AMOUNT_STANDARD,
                                           amount_out=850000000),
        ]
        with client.send_sign_request(commands):
            navigation_helper.ui_validate()

    # A swap ending on the wrapped-native token with no unwrap must display the
    # chain's wrapped ticker (WBNB), resolved from the table without CAL data.
    def test_bnb_swap_to_wrapped_native(self, backend, uniswap_contract_data, navigation_helper):
        client = UniswapClient(backend, uniswap_contract_data, chain_id=56)
        client.set_external_plugin()
        client.provide_token_metadata(tokens.CAKE_BNB)
        commands = [
            crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.CAKE_BNB.address,
                                           intermediate_tokens=[],
                                           out_token=WBNB_BNB,
                                           amount_in=10000000000000000000,
                                           amount_out=AMOUNT_STANDARD),
        ]
        with client.send_sign_request(commands):
            navigation_helper.ui_validate()

    # Celo has no wrapped-native token: wrap commands must be refused so the flow
    # falls back to blind signing instead of risking a wrong display.
    def test_wrap_refused_on_chain_without_wrapped_native(self, backend, uniswap_contract_data):
        client = UniswapClient(backend, uniswap_contract_data, chain_id=42220)
        client.set_external_plugin()
        commands = [
            crafter.craft_WRAP_ETH(amount=AMOUNT_STANDARD),
            crafter.craft_V2_SWAP_EXACT_IN(in_token=WETH_BASE,
                                           intermediate_tokens=[],
                                           out_token=tokens.USDT.address,
                                           amount_in=AMOUNT_STANDARD,
                                           amount_out=1000000000000000000),
        ]
        with pytest.raises(ExceptionRAPDU) as e:
            client.send_sync_sign_request(commands)
        assert e.value.status == 0x6A80
