#include "plugin.h"

// We will run in this function if we requested CAL data in handle_finalize
static bool provide_token_for_io_raw(asset_type_t *asset_type,
                                    asset_info_t *asset_info,
                                    const extraInfo_t *item) {
    // Check if we need CAL data for this asset
    if (*asset_type == UNKNOWN_TOKEN) {
        PRINTF("CAL data was requested\n");
        if (item == NULL) {
            PRINTF("Warning: no CAL data provided\n");
            return false;
        } else {
            // Ensure the addresses match
            if (memcmp(asset_info->address, item->token.address, ADDRESS_LENGTH) != 0) {
                PRINTF("Warning: data provided by the CAL does not match\n");
                PRINTF("Needed: %.*H\n", ADDRESS_LENGTH, asset_info->address);
                PRINTF("Received: %.*H\n", ADDRESS_LENGTH, item->token.address);
                return false;
            } else {
                // Remember that we know this token.
                *asset_type = KNOWN_TOKEN;
                // Store its decimals.
                asset_info->token_info.decimals = item->token.decimals;
                // Store its ticker.
                strlcpy(asset_info->token_info.ticker,
                        (char *) item->token.ticker,
                        sizeof(asset_info->token_info.ticker));
                return true;
            }
        }
    } else if (item != NULL) {
        PRINTF("Warning: CAL data provided but not requested\n");
        return true;
    }
    return true;
}


static bool provide_token_for_io(io_data_t *io_data,
                                const extraInfo_t *item) {
    return provide_token_for_io_raw(&io_data->asset_type, &io_data->u, item);
};

void handle_provide_token(ethPluginProvideInfo_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    msg->result = ETH_PLUGIN_RESULT_OK;
    msg->additionalScreens = 0;
    bool token_in_info_missing;
    bool token_out_info_missing;
    bool token_pay_portion_info_missing;

    token_in_info_missing = !provide_token_for_io(&context->input, msg->item1);

    token_out_info_missing = !provide_token_for_io(&context->output, msg->item2);

    if (context->pay_portion_asset_type == UNKNOWN_TOKEN) {
        PRINTF("Trying to find info for PAY_PORTION token in CAL\n");
        token_pay_portion_info_missing = true;
        if (provide_token_for_io_raw(&context->pay_portion_asset_type, &context->pay_portion_asset, msg->item1)) {
            PRINTF("Found it in item 1\n");
            token_pay_portion_info_missing = false;
        } else if (provide_token_for_io_raw(&context->pay_portion_asset_type, &context->pay_portion_asset, msg->item2)) {
            PRINTF("Found it in item 2\n");
            token_pay_portion_info_missing = false;
        }
    } else {
        token_pay_portion_info_missing = false;
    }

    if (token_in_info_missing) {
        PRINTF("Missing info for token IN\n");
    }
    if (token_out_info_missing) {
        PRINTF("Missing info for token OUT\n");
    }
    if (token_pay_portion_info_missing) {
        PRINTF("Missing info for token PAY PORTION\n");
    }

    if (token_in_info_missing || token_out_info_missing || token_pay_portion_info_missing) {
        // Additional screen for a warning
        msg->additionalScreens += 1;
        if (token_in_info_missing || token_out_info_missing) {
            // Additional screen for independent in / out asset display
            msg->additionalScreens += 2;
        }
    }
}
