#include "plugin.h"

// Sets the first screen to display.
void handle_query_contract_id(ethQueryContractID_t *msg) {
    const context_t *context = (const context_t *) msg->pluginContext;

    strlcpy(msg->name, APPNAME, msg->nameLength);

    if (context->selectorIndex == EXECUTE) {
        if (context->swap_type == EXACT_IN) {
            strlcpy(msg->version, "Swap tokens", msg->versionLength);
        } else {
            strlcpy(msg->version, "Swap tokens", msg->versionLength);
        }
        msg->result = ETH_PLUGIN_RESULT_OK;
    } else {
        PRINTF("Selector index: %d not supported\n", context->selectorIndex);
        msg->result = ETH_PLUGIN_RESULT_ERROR;
    }
}
