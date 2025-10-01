from web3 import Web3
from eth_typing import ChainId
from ledger_app_clients.ethereum.client import EthAppClient, StatusWord

from . import uniswap_crafter as crafter
from . import token_metadata_database as tokens

usdt_to_wojak = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token="dac17f958d2ee523a2206206994597c13d831ec7",
                                   intermediate_tokens=[],
                                   out_token="5026f006b85729a8b14553fae6af249ad16c9aab",
                                   amount_in=10000000000000000000,
                                   amount_out=1000000000000000000),
]

weth_to_wojak = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token="c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2",
                                   intermediate_tokens=[],
                                   out_token="5026f006b85729a8b14553fae6af249ad16c9aab",
                                   amount_in=240000000000000000,
                                   amount_out=1000000000000000000),
]

usdt_to_weth = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token="dac17f958d2ee523a2206206994597c13d831ec7",
                                   intermediate_tokens=[],
                                   out_token="c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2",
                                   amount_in=10000000000000000000,
                                   amount_out=240000000000000000,
                                   recipient="0000000000000000000000000000000000000002"),
]
usdt_to_weth_self = [
    crafter.craft_V2_SWAP_EXACT_IN(in_token="dac17f958d2ee523a2206206994597c13d831ec7",
                                   intermediate_tokens=[],
                                   out_token="c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2",
                                   amount_in=10000000000000000000,
                                   amount_out=240000000000000000,
                                   recipient="0000000000000000000000000000000000000001"),
]

eth_to_wojak = [
    crafter.craft_WRAP_ETH(amount=240000000000000000),
] + weth_to_wojak

usdt_to_eth = usdt_to_weth + [
    crafter.craft_UNWRAP_WETH(amount=240000000000000000, recipient="0000000000000000000000000000000000000001"),
]

class TestTokenMetadata:
    def test_token_metadata_unknown_to_unknown(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(usdt_to_wojak):
            navigation_helper.ui_validate()


    def test_token_metadata_unknown_to_known(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WOJAK)
        with uniswap_client.send_sign_request(usdt_to_wojak):
            navigation_helper.ui_validate()


    def test_token_metadata_known_to_unknown(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        with uniswap_client.send_sign_request(usdt_to_wojak):
            navigation_helper.ui_validate()


    def test_token_metadata_known_to_known(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        uniswap_client.provide_token_metadata(tokens.WOJAK)
        with uniswap_client.send_sign_request(usdt_to_wojak):
            navigation_helper.ui_validate()


    def test_token_metadata_weth_to_known(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WETH)
        uniswap_client.provide_token_metadata(tokens.WOJAK)
        with uniswap_client.send_sign_request(weth_to_wojak):
            navigation_helper.ui_validate()


    def test_token_metadata_weth_to_unknown(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WETH)
        with uniswap_client.send_sign_request(weth_to_wojak):
            navigation_helper.ui_validate()


    def test_token_metadata_known_to_weth(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        uniswap_client.provide_token_metadata(tokens.WETH)
        with uniswap_client.send_sign_request(usdt_to_weth_self):
            navigation_helper.ui_validate()


    def test_token_metadata_unknown_to_weth(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WETH)
        with uniswap_client.send_sign_request(usdt_to_weth_self):
            navigation_helper.ui_validate()


    def test_token_metadata_eth_to_known(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.WOJAK)
        with uniswap_client.send_sign_request(eth_to_wojak):
            navigation_helper.ui_validate()


    def test_token_metadata_eth_to_unknown(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        with uniswap_client.send_sign_request(eth_to_wojak):
            navigation_helper.ui_validate()


    def test_token_metadata_known_to_eth(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.USDT)
        with uniswap_client.send_sign_request(usdt_to_eth):
            navigation_helper.ui_validate()


    def test_token_metadata_unknown_to_unknown_provide_wrong_tokens(self, uniswap_client, navigation_helper):
        uniswap_client.set_external_plugin()
        uniswap_client.provide_token_metadata(tokens.BARL)
        uniswap_client.provide_token_metadata(tokens.LORDS)
        with uniswap_client.send_sign_request(usdt_to_wojak):
            navigation_helper.ui_validate()
