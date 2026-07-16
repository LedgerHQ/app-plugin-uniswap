#pragma once

#include "plugin.h"

// Per-chain wrapped-native-token ("WETH-equivalent") knowledge.
//
// The Universal Router cannot swap the native currency directly: swaps involving it are
// expressed as a WRAP/UNWRAP command plus a swap path using the chain's wrapped-native
// token (WETH on most chains, WBNB on BNB Chain, ...). Merging those two legs into a
// single displayed swap requires knowing the wrapped-native token address at parsing
// time, before any CAL token information is available. This table provides it per chain.
//
// Chains without an entry refuse WRAP_ETH / UNWRAP_WETH commands: plain token <-> token
// swaps still clear sign, wrap-involving flows fall back to the Ethereum application
// (no clear signing rather than wrong clear signing).
typedef struct wrapped_native_s {
    uint64_t chain_id;
    uint8_t address[ADDRESS_LENGTH];
    // Displayed when the wrapped token itself is a swap input / output (e.g. "WBNB")
    char wrapped_ticker[8];
    // Displayed when the wrap / unwrap is merged into the swap (e.g. "BNB")
    char native_ticker[8];
} wrapped_native_t;

// Select the wrapped-native entry matching the transaction's chain ID and store the
// selection in the context. Finding no entry is not an error.
void select_wrapped_native(context_t *context, const txInt256_t *chain_id);

// True if the current chain has a known wrapped-native token.
bool has_wrapped_native(const context_t *context);

// True if 'token' is the wrapped-native token of the current chain.
bool token_is_weth(const context_t *context, const uint8_t token[ADDRESS_LENGTH]);

// Address of the current chain's wrapped-native token, NULL if none.
const uint8_t *wrapped_native_address(const context_t *context);

// Tickers for display. The fallbacks are never displayed in practice: without a table
// entry no io_data can reach the ETH / WETH asset types.
const char *wrapped_native_ticker(const context_t *context);
const char *native_ticker(const context_t *context);

#define WETH_DECIMALS WEI_TO_ETHER
