#include "plugin_utils.h"
#include "plugin.h"
#include "os.h"
#include "check_tx_content.h"
#include "uniswap_contract_helpers.h"

// Called once to init.
void handle_init_contract(ethPluginInitContract_t *msg) {
    // Make sure we are running a compatible version.
    if (msg->interfaceVersion != ETH_PLUGIN_INTERFACE_VERSION_LATEST) {
        // If not the case, return the `UNAVAILABLE` status.
        PRINTF("Received interface version msg->interfaceVersion, expected %d\n",
               msg->interfaceVersion,
               ETH_PLUGIN_INTERFACE_VERSION_LATEST);
        msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }

    if (!check_tx_content(msg->txContent)) {
        PRINTF("Error in check_tx_content\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    // Double check that the `context_t` struct is not bigger than the maximum size (defined by
    // `msg->pluginContextLength`).
    if (msg->pluginContextLength < sizeof(context_t)) {
        PRINTF("Plugin parameters structure size %d is bigger than allowed size %d\n",
               sizeof(context_t),
               msg->pluginContextLength);
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    context_t *context = (context_t *) msg->pluginContext;

    // Initialize the context (to 0).
    memset(context, 0, sizeof(*context));

    if (get_self_address(context->own_address, msg->bip32) != 0) {
        PRINTF("Error: get_self_address failed\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    size_t index;
    if (!find_selector(U4BE(msg->selector, 0), SELECTORS, SELECTOR_COUNT, &index)) {
        PRINTF("Error: selector not found!\n");
        msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }
    context->selectorIndex = index;
    // check for overflow
    if ((size_t) context->selectorIndex != index) {
        PRINTF("Error: overflow detected on selector index!\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    // Set `next_param` to be the first field we expect to parse.
    // EDIT THIS: Adapt the `cases`, and set the `next_param` to be the first parameter you expect
    // to parse.
    switch (context->selectorIndex) {
        case EXECUTE:
            context->next_param = COMMANDS_OFFSET;
            break;
        default:
            PRINTF("Missing selectorIndex: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    // Return valid status.
    msg->result = ETH_PLUGIN_RESULT_OK;
}
