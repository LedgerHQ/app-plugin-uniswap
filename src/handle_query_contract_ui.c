#include "plugin.h"
#include "uniswap_contract_helpers.h"

static bool warning(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "WARNING", msg->titleLength);
    if ((context->input.asset_type == UNKNOWN_TOKEN) &&
        (context->output.asset_type == UNKNOWN_TOKEN)) {
        strlcpy(msg->msg, "Unknown tokens", msg->msgLength);
    } else if (context->input.asset_type == UNKNOWN_TOKEN) {
        strlcpy(msg->msg, "Unknown token sent", msg->msgLength);
    } else {
        strlcpy(msg->msg, "Unknown token received", msg->msgLength);
    }
    return true;
}

static bool format_address(char *msg, size_t msgLength, const uint8_t address[ADDRESS_LENGTH]) {
    if (msgLength < 43) {
        return false;
    } else {
        msg[0] = '0';
        msg[1] = 'x';
        for (int i = 0; i < 20; i++) {
            msg[2 + 2 * i] = HEXDIGITS[address[i] >> 4];
            msg[3 + 2 * i] = HEXDIGITS[address[i] & 0x0F];
        }
        msg[42] = '\0';
    }
    return true;
}

static bool asset_in(ethQueryContractUI_t *msg, const io_data_t *io_data) {
    if (io_data->asset_type == ETH) {
        strlcpy(msg->title, "Send", msg->titleLength);
        strlcpy(msg->msg, "Ethereum", msg->msgLength);
        return true;
    } else if (io_data->asset_type == KNOWN_TOKEN) {
        strlcpy(msg->title, "Send", msg->titleLength);
        strlcpy(msg->msg, io_data->u.token_info.ticker, msg->msgLength);
        return true;
    } else {
        strlcpy(msg->title, "Send token", msg->titleLength);
        return format_address(msg->msg, msg->msgLength, io_data->u.address);
    }
}

static bool asset_out(ethQueryContractUI_t *msg, const io_data_t *io_data) {
    if (io_data->asset_type == ETH) {
        strlcpy(msg->title, "Receive", msg->titleLength);
        strlcpy(msg->msg, "Ethereum", msg->msgLength);
        return true;
    } else if (io_data->asset_type == KNOWN_TOKEN) {
        strlcpy(msg->title, "Receive", msg->titleLength);
        strlcpy(msg->msg, io_data->u.token_info.ticker, msg->msgLength);
        return true;
    } else {
        strlcpy(msg->title, "Receive token", msg->titleLength);
        return format_address(msg->msg, msg->msgLength, io_data->u.address);
    }
}

static bool format_amount(char *msg, size_t msgLength, const io_data_t *io_data) {
    if (io_data->asset_type == UNKNOWN_TOKEN) {
        return amountToString(io_data->amount, sizeof(io_data->amount), 0, "???", msg, msgLength);
    } else {
        return amountToString(io_data->amount,
                              sizeof(io_data->amount),
                              io_data->u.token_info.decimals,
                              io_data->u.token_info.ticker,
                              msg,
                              msgLength);
    }
}

static bool amount_in(ethQueryContractUI_t *msg, const context_t *context) {
    if (context->swap_type == EXACT_IN) {
        strlcpy(msg->title, "Send", msg->titleLength);
    } else {
        strlcpy(msg->title, "Send minimum", msg->titleLength);
    }
    return format_amount(msg->msg, msg->msgLength, &context->input);
}

static bool amount_out(ethQueryContractUI_t *msg, const context_t *context) {
    if (context->swap_type == EXACT_IN) {
        strlcpy(msg->title, "Get minimum", msg->titleLength);
    } else {
        strlcpy(msg->title, "Receive", msg->titleLength);
    }
    return format_amount(msg->msg, msg->msgLength, &context->output);
}

static bool pay_portion(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Interface fee", msg->titleLength);

    // We can't use snprintf here because plugins are compiled without
    // Resort to doing manual computation

    // We receive an amount in 'basis points' unit: 1 basis point == 0.01 percent
    // First calculate the integer and fractional parts
    uint64_t integer_part = context->pay_portion_amount / 100;
    uint64_t fractional_part = context->pay_portion_amount % 100;

    // Longest string is 100.00%
    char integer_string[4];
    u64_to_string(integer_part, integer_string, sizeof(integer_string));
    char fractionnal_string[3];
    u64_to_string(fractional_part, fractionnal_string, sizeof(fractionnal_string));

    // Concatenate the elements
    strlcat(msg->msg, integer_string, msg->msgLength);
    strlcat(msg->msg, ".", msg->msgLength);
    if (fractional_part == 0) {
        strlcat(msg->msg, "00", msg->msgLength);
    } else if (strlen(fractionnal_string) < 2) {
        strlcat(msg->msg, "0", msg->msgLength);
        strlcat(msg->msg, fractionnal_string, msg->msgLength);
    } else {
        strlcat(msg->msg, fractionnal_string, msg->msgLength);
    }
    strlcat(msg->msg, "%", msg->msgLength);

    return true;
}

static bool recipient_screen(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Swap Recipient:", msg->titleLength);
    return format_address(msg->msg, msg->msgLength, context->recipient);
}

void handle_query_contract_ui(ethQueryContractUI_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    bool ret = false;
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);
    uint8_t index = msg->screenIndex;

    bool has_recipient_screen = !is_sender_address(context->recipient, context->own_address);

    if (context->input.asset_type != UNKNOWN_TOKEN && context->output.asset_type != UNKNOWN_TOKEN) {
        // Skip warning + independent asset display
        index += 3;
    }

    switch (index) {
        case 0:
            ret = warning(msg, context);
            break;
        case 1:
            ret = asset_in(msg, &context->input);
            break;
        case 2:
            ret = asset_out(msg, &context->output);
            break;
        case 3:
            ret = amount_in(msg, context);
            break;
        case 4:
            ret = amount_out(msg, context);
            break;
        case 5:
            // Recipient screen is 5th if applicable, else pay portion if applicable
            if (has_recipient_screen) {
                ret = recipient_screen(msg, context);
            } else if (context->pay_portion_amount != 0) {
                ret = pay_portion(msg, context);
            } else {
                PRINTF("Received an invalid index %d (%d)\n", msg->screenIndex, index);
            }
            break;
        case 6:
            // pay portion is 6th if applicable and recipient screen was 5th
            if (has_recipient_screen && context->pay_portion_amount != 0) {
                ret = pay_portion(msg, context);
            } else {
                PRINTF("Received an invalid index %d (%d)\n", msg->screenIndex, index);
            }
            break;

        default:
            PRINTF("Received an invalid index %d (%d)\n", msg->screenIndex, index);
    }

    msg->result = ret ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}
