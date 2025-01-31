import json
import os
import string
import datetime

from eth_abi import encode
from typing import Union, List, Optional

class TokenAndNetworkFee:
    address: str
    network_fee: Optional[str]

    def __init__(self, address, network_fee=None):
        self.address = address
        self.network_fee = network_fee

def _craft_v2_path(in_token: str, intermediate_tokens: [str], out_token: str) -> str:
    path = []
    path += [in_token]
    for step in intermediate_tokens:
        path += [step]
    path += [out_token]
    return path

def _craft_v3_path(in_token: str, intermediate_tokens: [TokenAndNetworkFee], out_token: TokenAndNetworkFee) -> str:
    path = ""
    path += in_token
    for step in intermediate_tokens:
        path += step.network_fee
        path += step.address
    path += out_token.network_fee
    path += out_token.address
    return bytes.fromhex(path)

def _craft_v3_path_reversed(in_token: str, intermediate_tokens: [TokenAndNetworkFee], out_token: TokenAndNetworkFee) -> str:
    path = ""
    path += out_token.address
    path += out_token.network_fee
    for step in intermediate_tokens:
        path += step.address
        path += step.network_fee
    path += in_token
    return bytes.fromhex(path)

class UniswapCommand:
    command_byte: bytes
    input: bytes

    def __init__(self, command_byte, input):
        self.command_byte = command_byte
        self.input = input

def craft_V3_SWAP_EXACT_IN(in_token: str, intermediate_tokens: [TokenAndNetworkFee], out_token: TokenAndNetworkFee, amount_in: int, amount_out: int, recipient: str = "0000000000000000000000000000000000000001") -> UniswapCommand:
    return UniswapCommand(command_byte=bytes.fromhex("00"),
                          input=encode(['address', 'uint256', 'uint256', 'bytes', 'bool'],
                                        [
                                            recipient,
                                            amount_in,
                                            amount_out,
                                            _craft_v3_path(in_token, intermediate_tokens, out_token),
                                            False,
                                        ]
    ))

def craft_V3_SWAP_EXACT_OUT(in_token: str, intermediate_tokens: Optional[TokenAndNetworkFee], out_token: TokenAndNetworkFee, amount_in: int, amount_out: int, recipient: str = "0000000000000000000000000000000000000001") -> UniswapCommand:
    return UniswapCommand(command_byte=bytes.fromhex("01"),
                          input=encode(['address', 'uint256', 'uint256', 'bytes', 'bool'],
                                        [
                                            recipient,
                                            amount_out,
                                            amount_in,
                                            _craft_v3_path_reversed(in_token, intermediate_tokens, out_token),
                                            False,
                                        ]
    ))

def craft_V2_SWAP_EXACT_IN(in_token: str, intermediate_tokens: Optional[str], out_token: str, amount_in: int, amount_out: int, recipient: str = "0000000000000000000000000000000000000001") -> UniswapCommand:
    return UniswapCommand(command_byte=bytes.fromhex("08"),
                          input=encode(['address', 'uint256', 'uint256', 'address[]', 'bool'],
                                        [
                                            recipient,
                                            amount_in,
                                            amount_out,
                                            _craft_v2_path(in_token, intermediate_tokens, out_token),
                                            False,
                                        ]
    ))

def craft_V2_SWAP_EXACT_OUT(in_token: str, intermediate_tokens: Optional[str], out_token: str, amount_in: int, amount_out: int, recipient: str = "0000000000000000000000000000000000000001") -> UniswapCommand:
    return UniswapCommand(command_byte=bytes.fromhex("09"),
                          input=encode(['address', 'uint256', 'uint256', 'address[]', 'bool'],
                                        [
                                            recipient,
                                            amount_out,
                                            amount_in,
                                            _craft_v2_path(in_token, intermediate_tokens, out_token),
                                            False,
                                        ]
    ))

def craft_WRAP_ETH(amount: int, recipient: str = "0000000000000000000000000000000000000002") -> UniswapCommand:
    return UniswapCommand(command_byte=bytes.fromhex("0b"),
                          input=encode(['address', 'uint256'],
                                        [
                                            recipient,
                                            amount,
                                        ]
    ))

def craft_UNWRAP_WETH(amount: int, recipient: str = "0000000000000000000000000000000000000002") -> UniswapCommand:
    return UniswapCommand(command_byte=bytes.fromhex("0c"),
                          input=encode(['address', 'uint256'],
                                        [
                                            recipient,
                                            amount,
                                        ]
    ))

def craft_PAY_PORTION(token: str, recipient: str, amount: int) -> UniswapCommand:
    return UniswapCommand(command_byte=bytes.fromhex("06"),
                          input=encode(['address', 'address', 'uint256'],
                                        [
                                            token,
                                            recipient,
                                            amount,
                                        ]
    ))

def craft_SWEEP(amount: int, token: str, recipient: str) -> UniswapCommand:
    return UniswapCommand(command_byte=bytes.fromhex("04"),
                          input=encode(['address', 'address', 'uint256'],
                                        [
                                            token,
                                            recipient,
                                            amount,
                                        ]
    ))

def craft_fake(command_byte) -> UniswapCommand:
    # Not a real PERMIT2_PERMIT, we just want some data to skip
    content=b'000000000000000000000000000000000000000212345678901234567890123456789012345678901234567890123456789098765432123456789098765432123456789098765430'
    return UniswapCommand(command_byte=bytes.fromhex(command_byte),
                          input=encode(['bytes'],
                                        [
                                            content
                                        ]
    ))
def craft_fake_PERMIT2_TRANSFER_FROM() -> UniswapCommand:
    return craft_fake("02")
def craft_fake_PERMIT2_PERMIT_BATCH() -> UniswapCommand:
    return craft_fake("03")
def craft_fake_PERMIT2_PERMIT() -> UniswapCommand:
    return craft_fake("0a")
def craft_fake_PERMIT2_TRANSFER_FROM_BATCH() -> UniswapCommand:
    return craft_fake("0d")

def craft_uniswap_tx(commands: List[UniswapCommand], uniswap_contract_data) -> bytes:
    commands_bytes = b''
    inputs = []
    for command in commands:
        commands_bytes += command.command_byte
        inputs.append(command.input)

    data = uniswap_contract_data.encode_abi("execute", [
        commands_bytes,
        inputs,
        int(datetime.datetime(2023, 12, 25, 0, 0).timestamp()),
    ])

    return data
