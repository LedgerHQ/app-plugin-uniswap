import json
import os
import string

import datetime

from typing import Union, List, Optional

class TokenAndNetworkFee:
    address: str
    network_fee: Optional[str]

    def __init__(self, address, network_fee=None):
        self.address = address
        self.network_fee = network_fee

def _address_to_parameter(address: str) -> str:
    return '00' * 12 + address

def _integer_to_parameter(var: int) -> str:
    hex_string = hex(var)[2:]  # Remove the '0x' prefix
    padded_hex_string = '0' * (64 - len(hex_string)) + hex_string
    return padded_hex_string

def _bool_to_parameter(var: bool) -> str:
    if var:
        return _integer_to_parameter(1)
    else:
        return _integer_to_parameter(0)

def _craft_v2_path(in_token: str, intermediate_tokens: [str], out_token: str) -> str:
    path = ""
    path_length = 1 + len(intermediate_tokens) + 1
    path += _integer_to_parameter(path_length)
    path += _address_to_parameter(in_token)
    for step in intermediate_tokens:
        path += _address_to_parameter(step)
    path += _address_to_parameter(out_token)
    return path

def _craft_v3_path(in_token: str, intermediate_tokens: [TokenAndNetworkFee], out_token: TokenAndNetworkFee) -> str:
    path = ""
    path_length = 20 + (len(intermediate_tokens) * (3 + 20)) + (3 + 20)
    path += _integer_to_parameter(path_length)
    path += in_token
    for step in intermediate_tokens:
        path += step.network_fee
        path += step.address
    path += out_token.network_fee
    path += out_token.address
    padding = 32 - (path_length % 32)
    path += '00' * padding
    return path

def _craft_v3_path_reversed(in_token: str, intermediate_tokens: [TokenAndNetworkFee], out_token: TokenAndNetworkFee) -> str:
    path = ""
    path_length = (20 + 3) + (len(intermediate_tokens) * (20 + 3)) + 20
    path += _integer_to_parameter(path_length)
    path += out_token.address
    path += out_token.network_fee
    for step in intermediate_tokens:
        path += step.address
        path += step.network_fee
    path += in_token
    padding = 32 - (path_length % 32)
    path += '00' * padding
    return path

class UniswapCommand:
    command_byte: bytes
    input: bytes

    def __init__(self, command_byte, input):
        self.command_byte = command_byte
        self.input = input

def craft_V3_SWAP_EXACT_IN(in_token: str, intermediate_tokens: [TokenAndNetworkFee], out_token: TokenAndNetworkFee, amount_in: int, amount_out: int, recipient: str = "0000000000000000000000000000000000000001") -> UniswapCommand:
    input_as_str = "000000000000000000000000" + recipient
    input_as_str += _integer_to_parameter(amount_in)
    input_as_str += _integer_to_parameter(amount_out)
    input_as_str += "00000000000000000000000000000000000000000000000000000000000000a0" # path offset from start, always 0xA0
    input_as_str += _bool_to_parameter(False) # payer is user
    input_as_str += _craft_v3_path(in_token, intermediate_tokens, out_token)

    return UniswapCommand(command_byte=bytes.fromhex("00"), input=bytes.fromhex(input_as_str))

def craft_V3_SWAP_EXACT_OUT(in_token: str, intermediate_tokens: Optional[TokenAndNetworkFee], out_token: TokenAndNetworkFee, amount_in: int, amount_out: int, recipient: str = "0000000000000000000000000000000000000001") -> UniswapCommand:
    input_as_str = "000000000000000000000000" + recipient
    input_as_str += _integer_to_parameter(amount_out)
    input_as_str += _integer_to_parameter(amount_in)
    input_as_str += "00000000000000000000000000000000000000000000000000000000000000a0" # path offset from start, always 0xA0
    input_as_str += _bool_to_parameter(False) # payer is user
    input_as_str += _craft_v3_path_reversed(in_token, intermediate_tokens, out_token)

    return UniswapCommand(command_byte=bytes.fromhex("01"), input=bytes.fromhex(input_as_str))

def craft_V2_SWAP_EXACT_IN(in_token: str, intermediate_tokens: Optional[str], out_token: str, amount_in: int, amount_out: int, recipient: str = "0000000000000000000000000000000000000001") -> UniswapCommand:
    input_as_str = "000000000000000000000000" + recipient
    input_as_str += _integer_to_parameter(amount_in)
    input_as_str += _integer_to_parameter(amount_out)
    input_as_str += "00000000000000000000000000000000000000000000000000000000000000a0" # path offset from start, always 0xA0
    input_as_str += _bool_to_parameter(False) # payer is user
    input_as_str += _craft_v2_path(in_token, intermediate_tokens, out_token)

    return UniswapCommand(command_byte=bytes.fromhex("08"), input=bytes.fromhex(input_as_str))

def craft_V2_SWAP_EXACT_OUT(in_token: str, intermediate_tokens: Optional[str], out_token: str, amount_in: int, amount_out: int, recipient: str = "0000000000000000000000000000000000000001") -> UniswapCommand:
    input_as_str = "000000000000000000000000" + recipient
    input_as_str += _integer_to_parameter(amount_out)
    input_as_str += _integer_to_parameter(amount_in)
    input_as_str += "00000000000000000000000000000000000000000000000000000000000000a0" # path offset from start, always 0xA0
    input_as_str += _bool_to_parameter(False) # payer is user
    input_as_str += _craft_v2_path(in_token, intermediate_tokens, out_token)

    return UniswapCommand(command_byte=bytes.fromhex("09"), input=bytes.fromhex(input_as_str))

def craft_WRAP_ETH(amount: int, recipient: str = "0000000000000000000000000000000000000002") -> UniswapCommand:
    input_as_str = "000000000000000000000000" + recipient
    input_as_str += _integer_to_parameter(amount)

    return UniswapCommand(command_byte=bytes.fromhex("0b"), input=bytes.fromhex(input_as_str))

def craft_UNWRAP_WETH(amount: int, recipient: str = "0000000000000000000000000000000000000002") -> UniswapCommand:
    input_as_str = "000000000000000000000000" + recipient
    input_as_str += _integer_to_parameter(amount)

    return UniswapCommand(command_byte=bytes.fromhex("0c"), input=bytes.fromhex(input_as_str))

def craft_uniswap_tx(commands: List[UniswapCommand], uniswap_contract_data) -> bytes:
    commands_bytes = b''
    inputs = []
    for command in commands:
        commands_bytes += command.command_byte
        inputs.append(command.input)

    data = uniswap_contract_data.encodeABI("execute", [
        commands_bytes,
        inputs,
        int(datetime.datetime(2023, 12, 25, 0, 0).timestamp()),
    ])

    print(data)
    selector = data[2:2 + 8]
    print(selector)
    calldata = data[2 + 8:]
    chunks = [calldata[i:i+64] for i in range(0, len(calldata), 64)]
    for chunk in chunks:
        print(chunk)

    return data
