import pytest
from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
from ragger.error import ExceptionRAPDU

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

class TestPermit2:

    def test_permit2(self, uniswap_client, navigation_helper):
        usdt_to_wojak_exact_in_v2 = [
            crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                           intermediate_tokens=[],
                                           out_token=tokens.WOJAK.address,
                                           amount_in=240000000000000000,
                                           amount_out=1000000000000000000),
        ]
        fake_permit2_permit_batch = [crafter.craft_fake_PERMIT2_PERMIT_BATCH()]
        fake_permit2_permit = [crafter.craft_fake_PERMIT2_PERMIT()]
        fake_permit2_transfer_from_batch = [crafter.craft_fake_PERMIT2_TRANSFER_FROM_BATCH()]

        # They all have the same display
        for contract in [usdt_to_wojak_exact_in_v2,
                         usdt_to_wojak_exact_in_v2 + fake_permit2_permit_batch,
                         usdt_to_wojak_exact_in_v2 + fake_permit2_permit,
                         usdt_to_wojak_exact_in_v2 + fake_permit2_transfer_from_batch,
                        ]:
            uniswap_client.set_external_plugin()
            with uniswap_client.send_sign_request(contract):
                navigation_helper.ui_validate()
