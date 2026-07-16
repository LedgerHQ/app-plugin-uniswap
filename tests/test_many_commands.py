from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

AMOUNT_LEG = 240000000000000000


class TestManyCommands:

    # Production split routes have been observed using up to 12 commands: a
    # many-way split of the same pair collapses to a single input/output display.
    def test_twelve_command_split(self, uniswap_client, navigation_helper):
        legs = [
            crafter.craft_V2_SWAP_EXACT_IN(in_token=tokens.USDT.address,
                                           intermediate_tokens=[],
                                           out_token=tokens.WOJAK.address,
                                           amount_in=AMOUNT_LEG,
                                           amount_out=1000000000000000000,
                                           recipient="0000000000000000000000000000000000000002")
            for _ in range(11)
        ] + [
            crafter.craft_SWEEP(amount=11000000000000000000,
                                token=tokens.WOJAK.address,
                                recipient="0000000000000000000000000000000000000001"),
        ]
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(legs):
            navigation_helper.ui_validate()
