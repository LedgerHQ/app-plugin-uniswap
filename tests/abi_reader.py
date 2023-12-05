import json
import os
from web3 import Web3

ABIS_FOLDER = "%s/abis" % (os.path.dirname(__file__))

def read_uniswap_contract_data():
    with open("%s/0x3fC91A3afd70395Cd496C647d5a6CC9D4B2b7FAD.abi.json" % (ABIS_FOLDER)) as file:
        contract = Web3().eth.contract(
            abi=json.load(file),
            # Get address from filename
            address=bytes.fromhex(os.path.basename(file.name).split(".")[0].split("x")[-1])
        )
    return contract

