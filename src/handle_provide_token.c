#include "plugin.h"

// We will run in this function if we requested CAL data in handle_finalize

static int provide_token_for_io(io_data_t *io_data,
                                const extraInfo_t *item,
                                bool *info_is_missing) {
    // Check if we need CAL data for this asset
    *info_is_missing = false;
    if (io_data->asset_type == UNKNOWN_TOKEN) {
        PRINTF("CAL data was requested\n");
        if (item == NULL) {
            *info_is_missing = true;
            PRINTF("Warning: no CAL data provided\n");
        } else {
            // Ensure the addresses match
            if (memcmp(io_data->u.address, item->token.address, ADDRESS_LENGTH) != 0) {
                PRINTF("Error: data provided by the CAL does not match\n");
                PRINTF("Needed: %.*H\n", ADDRESS_LENGTH, io_data->u.address);
                PRINTF("Received: %.*H\n", ADDRESS_LENGTH, item->token.address);
                return -1;
            } else {
                // Remember that we know this token.
                io_data->asset_type = KNOWN_TOKEN;
                // Store its decimals.
                io_data->u.token_info.decimals = item->token.decimals;
                // Store its ticker.
                strlcpy(io_data->u.token_info.ticker,
                        (char *) item->token.ticker,
                        sizeof(io_data->u.token_info.ticker));
            }
        }
    } else if (item != NULL) {
        PRINTF("Warning: CAL data provided but not requested\n");
    }
    return 0;
}

void handle_provide_token(ethPluginProvideInfo_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    msg->result = ETH_PLUGIN_RESULT_OK;
    msg->additionalScreens = 0;
    bool token_in_info_missing;
    bool token_out_info_missing;

    if (provide_token_for_io(&context->input, msg->item1, &token_in_info_missing) != 0) {
        PRINTF("Error in provide_token_for_io for input\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
    }

    if (provide_token_for_io(&context->output, msg->item2, &token_out_info_missing) != 0) {
        PRINTF("Error in provide_token_for_io for output\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
    }

    if (token_in_info_missing || token_out_info_missing) {
        // Additional screen for a warning
        msg->additionalScreens += 3;
    }
}
