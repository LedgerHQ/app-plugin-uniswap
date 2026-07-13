import pytest
from ragger.error import ExceptionRAPDU

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

SELF = "Dad77910DbDFdE764fC21FCD4E74D71bBACA6D8D"
POOL = "56534741CD8B152df6d48AdF7ac51f75169A83b2"  # an intermediate pool address
AMT = 240000000000000000


class TestChainedRecipients:

    # Mixed-protocol multi-hop routes chain legs by sending each leg's output
    # directly to the next leg's pool: those recipients are plumbing, only the
    # final leg's recipient is displayed.
    def test_pool_direct_chaining(self, uniswap_client, navigation_helper):
        cmds = [
            crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                           intermediate_tokens=[],
                                           out_token=tokens.WOJAK.address,
                                           amount_in=10000000000000000000,
                                           amount_out=AMT,
                                           recipient=POOL),
            crafter.craft_V3_SWAP_EXACT_IN(in_token=tokens.WOJAK.address,
                                           intermediate_tokens=[],
                                           out_token=crafter.TokenAndNetworkFee(address=tokens.BARL.address, network_fee="0001f4"),
                                           amount_in=AMT,
                                           amount_out=1000000000000000000,
                                           recipient=SELF),
        ]
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        uniswap_client.provide_token_metadata(tokens.BARL)
        with uniswap_client.send_sign_request(cmds):
            navigation_helper.ui_validate()

    # Split legs feeding the same displayed output must still agree on the
    # recipient: conflicting user-facing recipients stay rejected.
    def test_conflicting_split_recipients_rejected(self, uniswap_client, navigation_helper):
        cmds = [
            crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                           intermediate_tokens=[],
                                           out_token=tokens.WOJAK.address,
                                           amount_in=AMT,
                                           amount_out=AMT,
                                           recipient="00000000000000000000000000000000000AAAA1"),
            crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                           intermediate_tokens=[],
                                           out_token=tokens.WOJAK.address,
                                           amount_in=AMT,
                                           amount_out=AMT,
                                           recipient="00000000000000000000000000000000000BBBB2"),
        ]
        uniswap_client.set_external_plugin()
        with pytest.raises(ExceptionRAPDU) as e:
            uniswap_client.send_sync_sign_request(cmds)
        assert e.value.status == 0x6A80
