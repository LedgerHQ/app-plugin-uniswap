import json
import os
from web3 import Web3

ABIS_FOLDER = "%s/abis" % (os.path.dirname(__file__))

def read_uniswap_contract_data():
    with open("%s/0x66a9893cc07d91d95644aedd05d03f95e1dba8af.abi.json" % (ABIS_FOLDER)) as file:
        contract = Web3().eth.contract(
            abi=json.load(file),
            # Get address from filename
            address=bytes.fromhex(os.path.basename(file.name).split(".")[0].split("x")[-1])
        )
    return contract

