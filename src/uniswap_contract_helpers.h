#pragma once

#include "plugin.h"
#include "bip32_utils.h"

bool is_sender_address(const uint8_t address_to_check[ADDRESS_LENGTH],
                       const uint8_t own_address[ADDRESS_LENGTH]);

bool is_router_address(const uint8_t address[ADDRESS_LENGTH]);

int get_self_address(uint8_t address[ADDRESS_LENGTH], bip32_path_t *bip32);

bool is_contract_balance(const uint8_t amount[PARAMETER_LENGTH]);
