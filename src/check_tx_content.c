#include "plugin.h"
#include "check_tx_content.h"

#define UNISWAP_CONTRACT_ADDRESS                                                                  \
    {                                                                                             \
        0x66, 0xa9, 0x89, 0x3c, 0xc0, 0x7d, 0x91, 0xd9, 0x56, 0x44, 0xae, 0xdd, 0x05, 0xd0, 0x3f, \
            0x95, 0xe1, 0xdb, 0xa8, 0xaf                                                          \
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

    return ret;
}
