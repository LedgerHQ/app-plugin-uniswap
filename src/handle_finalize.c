#include "plugin.h"
#include "weth_token.h"
#include "uniswap_contract_helpers.h"

static bool inferior_or_equal(const uint8_t a[PARAMETER_LENGTH],
                              const uint8_t b[PARAMETER_LENGTH]) {
    return (memcmp(a, b, PARAMETER_LENGTH) <= 0);
}

static bool superior_or_equal(const uint8_t a[PARAMETER_LENGTH],
                              const uint8_t b[PARAMETER_LENGTH]) {
    return (memcmp(a, b, PARAMETER_LENGTH) >= 0);
}

static bool equal(const uint8_t a[PARAMETER_LENGTH], const uint8_t b[PARAMETER_LENGTH]) {
    return (memcmp(a, b, PARAMETER_LENGTH) == 0);
}

#define PRINT_PARAMETER(title, parameter)                \
    do {                                                 \
        PRINTF(title);                                   \
        for (uint8_t i = 0; i < PARAMETER_LENGTH; ++i) { \
            PRINTF("%02x", parameter[i]);                \
        }                                                \
        PRINTF("\n");                                    \
    } while (0);

/* Table of allowed WRAP / UNWRAP values
 * -------------------------------------------------------------------------------------------------
 * | WRAP / UNWRAP compared to amount: | INFERIOR                | EQUAL | SUPERIOR                |
 * -------------------------------------------------------------------------------------------------
 * | EXACT_IN:                         |                         |       |                         |
 * |    - WRAP: ETH -> WETH -> TOKEN   | KO (will fail on chain) | OK    | KO (waste)              |
 * |    - UNWRAP: TOKEN -> WETH -> ETH | OK (this is a min)      | OK    | KO (will fail on chain) |
 * -------------------------------------------------------------------------------------------------
 * | EXACT_OUT:                        |                         |       |                         |
 * |    - WRAP: ETH -> WETH -> TOKEN   | KO (will fail on chain) | OK    | OK (but sweep needed!)  |
 * |    - UNWRAP: TOKEN -> WETH -> ETH | OK (this is a min)      | OK    | KO (will fail on chain) |
 * -------------------------------------------------------------------------------------------------
 */
static bool valid_wrap_unwrap_amounts(const context_t *context) {
    bool sweep_expected = false;
    if (context->swap_type == EXACT_IN) {
        if (context->input.asset_type == ETH) {
            if (!equal(context->input.u.wrap_unwrap_amount, context->input.amount)) {
                PRINTF("Error: wrap amount is not equal to input\n");
                PRINT_PARAMETER("context->input.u.wrap_unwrap_amount",
                                context->input.u.wrap_unwrap_amount);
                PRINT_PARAMETER("context->input.amount", context->input.amount);
                return false;
            }
        } else if (context->output.asset_type == ETH) {
            if (!inferior_or_equal(context->output.u.wrap_unwrap_amount, context->output.amount)) {
                PRINTF("Error: unwrap amount is not inferior_or_equal to output\n");
                PRINT_PARAMETER("context->output.u.wrap_unwrap_amount",
                                context->output.u.wrap_unwrap_amount);
                PRINT_PARAMETER("context->output.amount", context->output.amount);
                return false;
            }
        }
    } else {
        if (context->input.asset_type == ETH) {
            sweep_expected = true;
            if (!superior_or_equal(context->input.u.wrap_unwrap_amount, context->input.amount)) {
                PRINTF("Error: wrap amount is not superior_or_equal to input\n");
                PRINT_PARAMETER("context->input.u.wrap_unwrap_amount",
                                context->input.u.wrap_unwrap_amount);
                PRINT_PARAMETER("context->input.amount", context->input.amount);
                return false;
            }
        } else if (context->output.asset_type == ETH) {
            if (!inferior_or_equal(context->output.u.wrap_unwrap_amount, context->output.amount)) {
                PRINTF("Error: unwrap amount is not inferior_or_equal to output\n");
                PRINT_PARAMETER("context->output.u.wrap_unwrap_amount",
                                context->output.u.wrap_unwrap_amount);
                PRINT_PARAMETER("context->output.amount", context->output.amount);
                return false;
            }
        }
    }

    if (context->sweep_received && !sweep_expected) {
        PRINTF("Error: sweep received out of context\n");
        return false;
    } else if (!context->sweep_received && sweep_expected) {
        PRINTF("Error: missing sweep\n");
        return false;
    }

    return true;
}

static bool resolve_asset_raw(asset_type_t *asset_type, asset_info_t *asset_info) {
    if (*asset_type == ETH) {
        PRINTF("Finalizing IO with asset ETH\n");
        asset_info->token_info.decimals = WEI_TO_ETHER;
        strlcpy(asset_info->token_info.ticker, "ETH", sizeof(asset_info->token_info.ticker));
        return true;

    } else if (*asset_type == WETH) {
        PRINTF("Finalizing IO with asset WETH\n");
        asset_info->token_info.decimals = WETH_DECIMALS;
        strlcpy(asset_info->token_info.ticker, WETH_TICKER, sizeof(asset_info->token_info.ticker));
        // Handle it like a token from now on
        *asset_type = KNOWN_TOKEN;
        return true;

    } else {
        PRINTF("Requesting CAL data for token %.*H\n", ADDRESS_LENGTH, asset_info->address);
        return false;
    }
}

// Resolve token if possible, request otherwise
static bool resolve_asset(io_data_t *io_data) {
    return resolve_asset_raw(&io_data->asset_type, &io_data->u);
}

void handle_finalize(ethPluginFinalize_t *msg) {
    msg->result = ETH_PLUGIN_RESULT_OK;
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

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
    if (context->sweep_received && context->output.asset_type == WETH) {
        PRINTF("Sweep is actually the main unwrap\n");
        context->output.asset_type = ETH;
        memset(context->output.u.wrap_unwrap_amount, 0, PARAMETER_LENGTH);
        context->sweep_received = false;
    }

    if (context->intermediate.intermediate_status != UNUSED) {
        PRINTF("Error: finalize failed, intermediate_status is still in use\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    if (!valid_wrap_unwrap_amounts(context)) {
        PRINTF("Error: valid_wrap_unwrap_amounts failed\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    if (!is_sender_address(context->recipient, context->own_address)) {
        PRINTF("Displaying recipient\n");
        ++msg->numScreens;
    }

    if (context->pay_portion_asset_type != UNSET) {
        PRINTF("Displaying pay portion\n");
        ++msg->numScreens;
    }

    // Resolve IOs if possible, request otherwise
    if (!resolve_asset(&context->input)) {
        PRINTF("Requesting CAL info for input token\n");
        msg->tokenLookup1 = context->input.u.address;
    }

    if (!resolve_asset(&context->output)) {
        PRINTF("Requesting CAL info for output token\n");
        msg->tokenLookup2 = context->output.u.address;
    }

    if (!resolve_asset_raw(&context->pay_portion_asset_type, &context->pay_portion_asset)) {
        if (msg->tokenLookup1 == NULL) {
            PRINTF("Requesting CAL info for PAY_PORTION token in item 1\n");
            msg->tokenLookup1 = context->pay_portion_asset.address;
        } else if (msg->tokenLookup2 == NULL) {
            PRINTF("Requesting CAL info for PAY_PORTION token in item 2\n");
            msg->tokenLookup2 = context->pay_portion_asset.address;
        } else {
            PRINTF("Warning: Can't request CAL info for PAY_PORTION token\n");
        }
    }
}
