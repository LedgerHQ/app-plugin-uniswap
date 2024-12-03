#include "plugin.h"
#include "weth_token.h"
#include "uniswap_contract_helpers.h"

static bool superior(const uint8_t a[PARAMETER_LENGTH], const uint8_t b[PARAMETER_LENGTH]) {
    return (memcmp(a, b, PARAMETER_LENGTH) > 0);
}

#define PRINT_PARAMETER(title, parameter)                \
    do {                                                 \
        PRINTF(title);                                   \
        for (uint8_t i = 0; i < PARAMETER_LENGTH; ++i) { \
            PRINTF("%02x", parameter[i]);                \
        }                                                \
        PRINTF("\n");                                    \
    } while (0);

// Resolve token if possible, request otherwise
static bool resolve_asset(io_data_t *io_data) {
    if (io_data->asset_type == ETH) {
        PRINTF("Finalizing IO with asset ETH\n");
        io_data->u.token_info.decimals = WEI_TO_ETHER;
        strlcpy(io_data->u.token_info.ticker, "ETH", sizeof(io_data->u.token_info.ticker));
        return true;

    } else if (io_data->asset_type == WETH) {
        PRINTF("Finalizing IO with asset WETH\n");
        io_data->u.token_info.decimals = WETH_DECIMALS;
        strlcpy(io_data->u.token_info.ticker, WETH_TICKER, sizeof(io_data->u.token_info.ticker));
        // Handle it like a token from now on
        io_data->asset_type = KNOWN_TOKEN;
        return true;

    } else {
        PRINTF("Requesting CAL data for token %.*H\n", ADDRESS_LENGTH, io_data->u.address);
        return false;
    }
}

void handle_finalize(ethPluginFinalize_t *msg) {
    msg->result = ETH_PLUGIN_RESULT_OK;
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    // EDIT THIS: Set the total number of screen you will need.
    msg->numScreens = 2;

    if (context->swap_type == NONE) {
        PRINTF("Error: finalize failed, context->swap_type == NONE\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    if (!context->recipient_set) {
        PRINTF("Error: !context->recipient_set\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    if (context->input.asset_type == UNSET) {
        PRINTF("Error: input never received\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    if (context->output.asset_type == UNSET) {
        PRINTF("Error: output never received\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    // Maybe not technically impossible but useless edge case that complicates handling.
    if ((context->input.asset_type == ETH || context->input.asset_type == WETH) &&
        (context->output.asset_type == ETH || context->output.asset_type == WETH)) {
        PRINTF("Error: this swap doesn't make sense\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    // This sweep was not just a sweep, it was the main uwrap
    if (context->unwrap_sweep_received && context->output.asset_type == WETH) {
        PRINTF("Sweep is actually the main unwrap\n");
        context->output.asset_type = ETH;
        memset(context->output.u.wrap_unwrap_amount, 0, PARAMETER_LENGTH);
        context->unwrap_sweep_received = false;
    }

    if (context->intermediate.intermediate_status != UNUSED) {
        PRINTF("Error: finalize failed, intermediate_status is still in use\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    if (msg->pluginSharedRO->txContent->value.length != 0) {
        if (context->input.asset_type != ETH) {
            PRINTF("Error: no native eth payment for token swap\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
        } else if (msg->pluginSharedRO->txContent->value.length > 32) {
            PRINTF("Error: invalid value length\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
        } else {
            uint8_t native_value[PARAMETER_LENGTH];
            memset(native_value, 0, PARAMETER_LENGTH);
            memmove(
                native_value + (PARAMETER_LENGTH - msg->pluginSharedRO->txContent->value.length),
                msg->pluginSharedRO->txContent->value.value,
                msg->pluginSharedRO->txContent->value.length);
            if (superior(native_value, context->input.amount)) {
                PRINTF("Using native payment value instead\n");
                memmove(context->input.amount, native_value, PARAMETER_LENGTH);
                PRINTF("New in value %.*H\n", PARAMETER_LENGTH, context->input.amount);
            }
        }
    }

    if (context->output.asset_type != ETH) {
        // Max of sweep and amount out
        if (superior(context->sweep_amount, context->output.amount)) {
            PRINTF("Displaying sweep amount as output\n");
            PRINTF("context->output.amount %.*H\n", PARAMETER_LENGTH, context->output.amount);
            PRINTF("context->sweep_amount %.*H\n", PARAMETER_LENGTH, context->sweep_amount);
            memmove(context->output.amount, context->sweep_amount, PARAMETER_LENGTH);
        }
    } else {
        // Max of sweep, amount out, and unwrap
        uint8_t *max;
        if (superior(context->sweep_amount, context->output.u.wrap_unwrap_amount)) {
            PRINTF("Trying sweep_amount as out output\n");
            max = context->sweep_amount;
        } else {
            PRINTF("Trying unwrap_amount as out output\n");
            max = context->output.u.wrap_unwrap_amount;
        }
        if (superior(max, context->output.amount)) {
            PRINTF("Using max\n");
            memmove(context->output.amount, max, PARAMETER_LENGTH);
        }
    }

    if (!is_sender_address(context->recipient, context->own_address)) {
        PRINTF("Displaying recipient\n");
        ++msg->numScreens;
    }

    if (context->pay_portion_amount != 0) {
        PRINTF("Displaying pay portion\n");
        ++msg->numScreens;
    }

    // Resolve IOs if possible, request otherwise
    if (!resolve_asset(&context->input)) {
        msg->tokenLookup1 = context->input.u.address;
    }

    if (!resolve_asset(&context->output)) {
        msg->tokenLookup2 = context->output.u.address;
    }
}
