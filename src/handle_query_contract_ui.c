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
        strlcpy(msg->title, "Sending", msg->titleLength);
        strlcpy(msg->msg, "Ethereum", msg->msgLength);
        return true;
    } else if (io_data->asset_type == KNOWN_TOKEN) {
        strlcpy(msg->title, "Sending", msg->titleLength);
        strlcpy(msg->msg, io_data->u.token_info.ticker, msg->msgLength);
        return true;
    } else {
        strlcpy(msg->title, "Sending token", msg->titleLength);
        return format_address(msg->msg, msg->msgLength, io_data->u.address);
    }
}

static bool asset_out(ethQueryContractUI_t *msg, const io_data_t *io_data) {
    if (io_data->asset_type == ETH) {
        strlcpy(msg->title, "Receiving", msg->titleLength);
        strlcpy(msg->msg, "Ethereum", msg->msgLength);
        return true;
    } else if (io_data->asset_type == KNOWN_TOKEN) {
        strlcpy(msg->title, "Receiving", msg->titleLength);
        strlcpy(msg->msg, io_data->u.token_info.ticker, msg->msgLength);
        return true;
    } else {
        strlcpy(msg->title, "Receiving token", msg->titleLength);
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
        strlcpy(msg->title, "Sending", msg->titleLength);
    } else {
        strlcpy(msg->title, "Sending min.", msg->titleLength);
    }
    return format_amount(msg->msg, msg->msgLength, &context->input);
}

static bool amount_out(ethQueryContractUI_t *msg, const context_t *context) {
    if (context->swap_type == EXACT_IN) {
        strlcpy(msg->title, "Receiving min.", msg->titleLength);
    } else {
        strlcpy(msg->title, "Receiving", msg->titleLength);
    }
    return format_amount(msg->msg, msg->msgLength, &context->output);
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

    bool has_recipient_screen = !is_sender_address(context->recipient);

    if ((context->input.asset_type == UNKNOWN_TOKEN) ||
        (context->output.asset_type == UNKNOWN_TOKEN)) {
        switch (msg->screenIndex) {
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
                if (has_recipient_screen) {
                    ret = recipient_screen(msg, context);
                    break;
                }
                __attribute__((fallthrough));

            default:
                PRINTF("Received an invalid screenIndex\n");
        }
    } else {
        switch (msg->screenIndex) {
            case 0:
                ret = amount_in(msg, context);
                break;
            case 1:
                ret = amount_out(msg, context);
                break;
            case 2:
                if (has_recipient_screen) {
                    ret = recipient_screen(msg, context);
                    break;
                }
                __attribute__((fallthrough));

            default:
                PRINTF("Received an invalid screenIndex\n");
        }
    }
    msg->result = ret ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}
