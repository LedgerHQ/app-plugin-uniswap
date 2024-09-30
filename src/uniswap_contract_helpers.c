#include "plugin.h"
#include "uniswap_contract_helpers.h"
#include "crypto_helpers.h"

// Check if address is 0x1
static bool is_sender_constant(const uint8_t address[ADDRESS_LENGTH]) {
    for (uint8_t i = 0; i < ADDRESS_LENGTH - 1; ++i) {
        if (address[i] != 0) {
            return false;
        }
    }
    return (address[ADDRESS_LENGTH - 1] == 1);
}

// Check if the address is 0000000000000000000000000000000000000001
bool is_sender_address(const uint8_t address_to_check[ADDRESS_LENGTH],
                       const uint8_t own_address[ADDRESS_LENGTH]) {
    if (is_sender_constant(address_to_check)) {
        return true;
    }
    return (memcmp(address_to_check, own_address, ADDRESS_LENGTH) == 0);
}

// Check if the address is 0000000000000000000000000000000000000002
bool is_router_address(const uint8_t address[ADDRESS_LENGTH]) {
    for (uint8_t i = 0; i < ADDRESS_LENGTH - 1; ++i) {
        if (address[i] != 0) {
            return false;
        }
    }
    return (address[ADDRESS_LENGTH - 1] == 2);
}

// Derive our key on the derivation path and save our address in the context
int get_self_address(uint8_t address[ADDRESS_LENGTH], bip32_path_t *bip32) {
    PRINTF("bip32_path = %.*H\n", bip32->length * 4, bip32->path);
    uint8_t raw_pubkey[65];
    if (bip32_derive_get_pubkey_256(CX_CURVE_256K1,
                                    bip32->path,
                                    bip32->length,
                                    raw_pubkey,
                                    NULL,
                                    CX_SHA512) != CX_OK) {
        PRINTF("bip32_derive_get_pubkey_256 FAILED\n");
        return -1;
    }

    getEthAddressFromRawKey((const uint8_t *) raw_pubkey, address);
    PRINTF("Our address is %.*H\n", ADDRESS_LENGTH, address);
    return 0;
}
