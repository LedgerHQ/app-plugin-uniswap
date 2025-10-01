#include "plugin.h"
#include "check_tx_content.h"

bool check_tx_content(const txContent_t *tx_content) {
    if (tx_content->destinationLength != ADDRESS_LENGTH) {
        PRINTF("Error: mismatch contract address length\n");
        return false;
    }

    return true;
}
