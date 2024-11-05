import pytest
from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
from ragger.error import ExceptionRAPDU

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

usdt_to_wojak_exact_in_v3 = [
    crafter.craft_V3_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                   intermediate_tokens=[],
                                   out_token=crafter.TokenAndNetworkFee(address=tokens.WOJAK.address, network_fee="0001f4"),
                                   amount_in=240000000000000000,
                                   amount_out=0000000000000000000),
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

sweep_invalid_recipient = [
    crafter.craft_SWEEP(amount=2500000000000000000,
                        token=tokens.WOJAK.address,
                        recipient="Dead7910DbDFdE764fC21FCD4E74D71bBACA6D8D"),
]

class TestSweep:

    def test_sweep(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        uniswap_client.provide_token_metadata(tokens.WOJAK)
        with uniswap_client.send_sign_request(usdt_to_wojak_exact_in_v3 + pay_portion + sweep):
            navigation_helper.ui_validate()

    def test_sweep_invalid_recipient(self, uniswap_client):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        uniswap_client.provide_token_metadata(tokens.WOJAK)
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(usdt_to_wojak_exact_in_v3 + pay_portion + sweep_invalid_recipient)
        assert e.value.status == 0x6A80

