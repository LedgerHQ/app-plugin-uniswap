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

Subset of commands from Universal Router v2.0:
https://docs.uniswap.org/contracts/universal-router/technical-reference#command-inputs

| Command |
| ---- |
| V2_SWAP_EXACT_IN | 
| V2_SWAP_EXACT_OUT |
| V3_SWAP_EXACT_IN |
| V3_SWAP_EXACT_OUT |
| WRAP_ETH |
| UNWRAP_ETH |
| PERMIT2_PERMIT_BATCH |
| PERMIT2_TRANSFER_FROM_BATCH |
| PERMIT2_PERMIT |
| PAY_PORTION |

## Limitation

Due to memory space limitations, it is currently not possible to display and sign a swap with a version split path
It IS possible to combine several swaps of different versions if their path is not split
```
(Let's note O the origin asset and G the goal asset, and I an intermediate asset)

The following scenarios are ALLOWED
--- 
O -> G in V2_SWAP
O -> G in V3_SWAP
---
O -> G in V2_SWAP
O -> I -> ... -> G in V2_SWAP
O -> G in V3_SWAP
O -> I -> ... -> G in V3_SWAP
---

The following scenarios are NOT allowed
---
O -> ... -> I in V3_SWAP
I -> ... -> G in V2_SWAP
---
O -> ... -> I in V2_SWAP
I -> ... -> G in V3_SWAP
---
O -> G in V2_SWAP
O -> ... -> I in V2_SWAP
I -> ... -> G in V3_SWAP
---
```
