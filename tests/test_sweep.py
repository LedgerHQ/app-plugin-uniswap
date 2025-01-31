import pytest
from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
from ragger.error import ExceptionRAPDU

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

usdt_to_wojak_exact_in_v3_to_router = [
    crafter.craft_V3_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                   intermediate_tokens=[],
                                   out_token=crafter.TokenAndNetworkFee(address=tokens.WOJAK.address, network_fee="0001f4"),
                                   amount_in=240000000000000000,
                                   amount_out=0000000000000000000,
                                   recipient="0000000000000000000000000000000000000002"),
]
usdt_to_wojak_exact_in_v3 = [
    crafter.craft_V3_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                   intermediate_tokens=[],
                                   out_token=crafter.TokenAndNetworkFee(address=tokens.WOJAK.address, network_fee="0001f4"),
                                   amount_in=240000000000000000,
                                   amount_out=0000000000000000000,
                                   recipient="Dead7910DbDFdE764fC21FCD4E74D71bBACA6D8D"),
]

pay_portion = [
    crafter.craft_PAY_PORTION(token=tokens.WOJAK.address,
                              recipient="0000000000000000000000000000000000000002",
                              amount=2),
        ]

sweep = [
    crafter.craft_SWEEP(amount=2500000000000000000,
                        token=tokens.WOJAK.address,
                        recipient="Dad77910DbDFdE764fC21FCD4E74D71bBACA6D8D"),
]

sweep_weth = [
    crafter.craft_SWEEP(amount=2500000000000000000,
                        token=tokens.WETH.address,
                        recipient="Dad77910DbDFdE764fC21FCD4E74D71bBACA6D8D"),
]

sweep_other = [
    crafter.craft_SWEEP(amount=2500000000000000000,
                        token=tokens.WOJAK.address,
                        recipient="Dead7910DbDFdE764fC21FCD4E74D71bBACA6D8D"),
]

class TestSweep:

    def test_sweep_to_self(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        uniswap_client.provide_token_metadata(tokens.WOJAK)
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_in_v3_to_router + pay_portion + sweep):
            navigation_helper.ui_validate()

    def test_sweep_other_recipient(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        uniswap_client.provide_token_metadata(tokens.WOJAK)
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_in_v3_to_router + pay_portion + sweep_other):
            navigation_helper.ui_validate()

    def test_sweep_invalid_swap_recipient(self, uniswap_client):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        uniswap_client.provide_token_metadata(tokens.WOJAK)
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(usdt_to_wojak_exact_in_v3 + pay_portion + sweep)
        assert e.value.status == 0x6A80

    def test_sweep_invalid_swap_token(self, uniswap_client):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        uniswap_client.provide_token_metadata(tokens.WOJAK)
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(usdt_to_wojak_exact_in_v3 + pay_portion + sweep_weth)
        assert e.value.status == 0x6A80
