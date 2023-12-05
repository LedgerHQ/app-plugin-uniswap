class TokenMetadata:
	def __init__(self, ticker, address, decimals, chain_id):
		self.ticker = ticker
		self.address = address
		self.decimals = decimals
		self.chain_id = chain_id

WETH = TokenMetadata("WETH", "c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2", 18, 1)
WOJAK = TokenMetadata("WOJAK", "5026f006b85729a8b14553fae6af249ad16c9aab", 18, 1)
USDT = TokenMetadata("USDT", "dac17f958d2ee523a2206206994597c13d831ec7", 18, 1)
BARL = TokenMetadata("BARL", "3e2324342bf5b8a1dca42915f0489497203d640e", 18, 1)
LORDS = TokenMetadata("LORDS", "686f2404e77Ab0d9070a46cdfb0B7feCDD2318b0", 18, 1)
