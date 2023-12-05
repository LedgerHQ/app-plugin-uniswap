#pragma once

#include "plugin.h"

extern const uint8_t weth_address[ADDRESS_LENGTH];

bool token_is_weth(const uint8_t token[ADDRESS_LENGTH]);

#define WETH_DECIMALS WEI_TO_ETHER
#define WETH_TICKER   "WETH"
