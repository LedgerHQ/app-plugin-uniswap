# Chain 138 (DeFi Oracle Meta Mainnet) — plugin test fixtures

Chain ID **138** uses the official Uniswap V3 deployment on DeFi Oracle Meta Mainnet:

| Contract | Address |
|----------|---------|
| UniswapV3Factory | `0x2f7219276e3ce367dB9ec74C1196a8ecEe67841C` |
| SwapRouter02 | `0xde9cD8ee2811E6E64a41D5F68Be315d33995975E` |
| QuoterV2 | `0x6abbB1CEb2468e748a03A00CD6aA9BFE893AFa1f` |
| Permit2 | `0x000000000022D473030F116dDEE9F6B43aC78BA3` |

Full ragger/snapshot fixtures should be added when Uniswap Labs confirms Chain 138 support in the plugin CI matrix. Until then, clear-signing ERC-7730 descriptors cover SwapRouter02 and Permit2 on chain 138.
