from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

SELF = "Dad77910DbDFdE764fC21FCD4E74D71bBACA6D8D"
ROUTER = "0000000000000000000000000000000000000002"
AMT = 240000000000000000


class TestUnwrapRefund:

    # Native-input EXACT_OUTPUT: the router wraps the maximum input, swaps, then
    # unwraps the unspent remainder back to the user. The unwrap is a refund and
    # the display stays "Send <= maxIn native -> Get exact token out".
    def test_exact_out_native_in_unwrap_refund(self, uniswap_client, navigation_helper):
        cmds = [
            crafter.craft_WRAP_ETH(amount=AMT),
            crafter.craft_V3_SWAP_EXACT_OUT(in_token=tokens.WETH.address,
                                            intermediate_tokens=[],
                                            out_token=crafter.TokenAndNetworkFee(address=tokens.USDT.address, network_fee="0001f4"),
                                            amount_in=AMT,
                                            amount_out=1000000000,
                                            recipient=SELF),
            crafter.craft_UNWRAP_WETH(amount=0, recipient=SELF),
        ]
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        with uniswap_client.send_sign_request(cmds, amount=AMT):
            navigation_helper.ui_validate()

    # Production-shaped 9-command split: wrap + five swap legs to the router +
    # sweep of the output + unwrap refund of unspent input.
    def test_nine_command_split_with_unwrap_refund(self, uniswap_client, navigation_helper):
        legs = [crafter.craft_WRAP_ETH(amount=5 * AMT)] + [
            crafter.craft_V3_SWAP_EXACT_IN(in_token=tokens.WETH.address,
                                           intermediate_tokens=[],
                                           out_token=crafter.TokenAndNetworkFee(address=tokens.USDT.address, network_fee="0001f4"),
                                           amount_in=AMT,
                                           amount_out=1000000000,
                                           recipient=ROUTER)
            for _ in range(5)
        ] + [
            crafter.craft_SWEEP(amount=5000000000, token=tokens.USDT.address, recipient=SELF),
            crafter.craft_UNWRAP_WETH(amount=0, recipient=SELF),
        ]
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        with uniswap_client.send_sign_request(legs, amount=5 * AMT):
            navigation_helper.ui_validate()
