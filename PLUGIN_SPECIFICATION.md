# Technical Specification

## About

This documentation describes the smart contracts and functions supported by the plugin.

## Smart Contracts

|  Network | Version | Smart Contract | Address |
|   ----   |   ---   |      ----      |   ---   |
| NetworkName   | V2.0  | Uniswap  | `0x66a9893cc07d91d95644aedd05d03f95e1dba8af` |

## Functions

For the smart contracts implemented, the functions covered by the plugin shall be described here:

|Contract |    Function   | Selector  | Displayed Parameters |
|   ---   |    ---        | ---       | --- |
|UniversalRouter  | Execute           | `0x3593564c` | Depends on the commands sent |

## COMMANDS supported

Subset of commands from the Universal Router:
https://docs.uniswap.org/contracts/universal-router/technical-reference#command-inputs

| Command |
| ---- |
| V2_SWAP_EXACT_IN |
| V2_SWAP_EXACT_OUT |
| V3_SWAP_EXACT_IN |
| V3_SWAP_EXACT_OUT |
| V4_SWAP |
| WRAP_ETH |
| UNWRAP_ETH |
| PERMIT2_PERMIT_BATCH |
| PERMIT2_TRANSFER_FROM |
| PERMIT2_TRANSFER_FROM_BATCH |
| PERMIT2_PERMIT |
| PAY_PORTION |
| SWEEP |

Both the Universal Router 2.0 and 2.1.1 input layouts are supported: the
2.1.1 layout appends a `minHopPriceX36` array to every swap input, and the
plugin detects which layout is in use per swap input from its path offset
(rejecting unknown layouts rather than misparsing them).

Within V4_SWAP, the path-form swap actions (`SWAP_EXACT_IN`, `SWAP_EXACT_OUT`)
plus `SETTLE`, `SETTLE_ALL`, `TAKE` and `TAKE_ALL` are supported. The
`*_SINGLE` swap action forms and all other actions are rejected.

## Route support

The plugin collapses the commands of a transaction into a single displayed
swap: one input asset and one output asset (plus swap type, amounts, recipient
and optional interface fee). Multi-hop paths, split routes (several commands
sharing the same input or output asset) and mixed-version routes (V2 / V3 / V4
legs combined, including legs chained through another version's pool or
through the native / wrapped boundary) are all supported, as long as the
route collapses to one input and one output asset.

Intermediate assets are never displayed. Routes that do not collapse to a
single input / output pair, exceed 16 commands, or contain unsupported
commands are rejected.
