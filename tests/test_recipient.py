import pytest
from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
from ragger.error import ExceptionRAPDU

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

weth_to_wojak_recipient_1 = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.WETH.address,
                                   intermediate_tokens=[],
                                   out_token=tokens.WOJAK.address,
                                   amount_in=1234567,
                                   amount_out=7654321,
                                   recipient="3e2324342bf5b8a1dca42915f0489497203d640e"),
]
weth_to_wojak_recipient_2 = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.WETH.address,
                                   intermediate_tokens=[],
                                   out_token=tokens.WOJAK.address,
                                   amount_in=1234567,
                                   amount_out=7654321,
                                   recipient="4e2324342bf5b8a1dca42915f0489497203d640f"),
]
weth_to_wojak_recipient_self = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.WETH.address,
                                   intermediate_tokens=[],
                                   out_token=tokens.WOJAK.address,
                                   amount_in=1234567,
                                   amount_out=7654321,
                                   recipient="Dad77910DbDFdE764fC21FCD4E74D71bBACA6D8D"),
]

class TestRecipients:

    def test_recipient_is_not_self(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WETH)
        with uniswap_client.send_sign_request(weth_to_wojak_recipient_1):
            navigation_helper.ui_validate()

    def test_invalid_mismatch_recipient(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WETH)
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(weth_to_wojak_recipient_1 + weth_to_wojak_recipient_2)
        assert e.value.status == 0x6A80

    def test_valid_recipient_is_self_address(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WETH)
        with uniswap_client.send_sign_request(weth_to_wojak_recipient_self):
            navigation_helper.ui_validate()
