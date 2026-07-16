#include "plugin.h"
#include "check_tx_content.h"

bool check_tx_content(const txContent_t *tx_content) {
    if (tx_content->destinationLength != ADDRESS_LENGTH) {
        PRINTF("Error: mismatch contract address length\n");
        return false;
    }

    // The destination address itself is not checked here: the Ethereum application only
    // starts this plugin for contracts registered to it in the CAL, which covers the
    // Universal Router deployment of each chain.
    return true;
}
