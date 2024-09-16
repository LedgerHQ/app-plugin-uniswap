#include "plugin.h"
#include "check_tx_content.h"

#define UNISWAP_CONTRACT_ADDRESS                                                                  \
    {                                                                                             \
        0x3f, 0xc9, 0x1a, 0x3a, 0xfd, 0x70, 0x39, 0x5c, 0xd4, 0x96, 0xc6, 0x47, 0xd5, 0xa6, 0xcc, \
            0x9d, 0x4b, 0x2b, 0x7f, 0xad                                                          \
    }

bool check_tx_content(const txContent_t *tx_content) {
    bool ret = true;

    if (tx_content->destinationLength != ADDRESS_LENGTH) {
        PRINTF("Error: mismatch contract address length\n");
        ret = false;
    }

    // Technically not needed as ETH application already checks this address against CAL data
    uint8_t uniswap_contract_address[ADDRESS_LENGTH] = UNISWAP_CONTRACT_ADDRESS;
    if (memcmp(uniswap_contract_address, tx_content->destination, ADDRESS_LENGTH) != 0) {
        PRINTF("Error: mismatch contract address\n");
        ret = false;
    }

    if (tx_content->value.length != 0) {
        PRINTF("Error: no native eth payment for Uniswap smart contract\n");
        ret = false;
    }

    return ret;
}
