#include "plugin.h"
#include "uniswap_contract_helpers.h"

// Check if the address is 0000000000000000000000000000000000000001
bool is_sender_address(const uint8_t address[ADDRESS_LENGTH]) {
    for (uint8_t i = 0; i < ADDRESS_LENGTH - 1; ++i) {
        if (address[i] != 0) {
            return false;
        }
    }
    return (address[ADDRESS_LENGTH - 1] == 1);
}
