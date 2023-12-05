from web3 import Web3
from contextlib import contextmanager
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord

from .token_metadata_database import TokenMetadata
from .utils import get_appname_from_makefile
from .uniswap_crafter import craft_uniswap_tx

BIP32_PATH = "m/44'/60'/1'/0/0"

PLUGIN_NAME = get_appname_from_makefile()

# We could use 'get_selector_from_data()', but as we have only one possible selector for Uniswap
# commands, let's not bother
UNISWAP_SELECTOR = bytes.fromhex("3593564c")

class UniswapClient:
    def __init__(self, backend, uniswap_contract_data):
        self.client = EthAppClient(backend)
        self.backend = backend
        self.uniswap_contract_data = uniswap_contract_data

    def set_external_plugin(self):
        self.client.set_external_plugin(PLUGIN_NAME,
                                        self.uniswap_contract_data.address,
                                        UNISWAP_SELECTOR)

    @contextmanager
    def send_sign_request(self, uniswap_commands):
        data = craft_uniswap_tx(uniswap_commands, self.uniswap_contract_data)
        # send the transaction
        with self.client.sign(
            BIP32_PATH,
            {
                "nonce": 20,
                "maxFeePerGas": Web3.to_wei(145, "gwei"),
                "maxPriorityFeePerGas": Web3.to_wei(1.5, "gwei"),
                "gas": 173290,
                "to": self.uniswap_contract_data.address,
                # TODO: check ?
                "chainId": ChainId.ETH,
                "data": data
             },
        ):
            yield

    def send_sync_sign_request(self, uniswap_commands):
        with self.send_sign_request(uniswap_commands):
            pass

    def provide_token_metadata_raw(self, ticker, address, decimals, chain_id):
        self.client.provide_token_metadata(ticker,
                                           bytes.fromhex(address),
                                           decimals,
                                           chain_id)


    def provide_token_metadata(self, token_metadata: TokenMetadata):
        self.provide_token_metadata_raw(token_metadata.ticker, token_metadata.address, token_metadata.decimals, token_metadata.chain_id)
