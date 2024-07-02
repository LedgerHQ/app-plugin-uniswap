#include "plugin.h"
#include "debug.h"

void print_parameter_name(parameter_t parameter) {
    switch (parameter) {
        case COMMANDS_OFFSET:
            PRINTF("COMMANDS_OFFSET\n");
            break;
        case INPUTS_HEADER_OFFSET:
            PRINTF("INPUTS_HEADER_OFFSET\n");
            break;
        case DEADLINE:
            PRINTF("DEADLINE\n");
            break;
        case COMMANDS_LENGTH:
            PRINTF("COMMANDS_LENGTH\n");
            break;
        case COMMANDS:
            PRINTF("COMMANDS\n");
            break;
        case INPUTS_NUMBER:
            PRINTF("INPUTS_NUMBER\n");
            break;
        case INPUTS_OFFSET:
            PRINTF("INPUTS_OFFSET\n");
            break;
        case INPUT_WRAP_ETH_LENGTH:
            PRINTF("INPUT_WRAP_ETH_LENGTH\n");
            break;
        case INPUT_WRAP_ETH_RECIPIENT:
            PRINTF("INPUT_WRAP_ETH_RECIPIENT\n");
            break;
        case INPUT_WRAP_ETH_AMOUNT:
            PRINTF("INPUT_WRAP_ETH_AMOUNT\n");
            break;
        case INPUT_UNWRAP_WETH_LENGTH:
            PRINTF("INPUT_UNWRAP_WETH_LENGTH\n");
            break;
        case INPUT_UNWRAP_WETH_RECIPIENT:
            PRINTF("INPUT_UNWRAP_WETH_RECIPIENT\n");
            break;
        case INPUT_UNWRAP_WETH_AMOUNT:
            PRINTF("INPUT_UNWRAP_WETH_AMOUNT\n");
            break;
        case INPUT_PAY_PORTION_LENGTH:
            PRINTF("INPUT_PAY_PORTION_LENGTH\n");
            break;
        case INPUT_PAY_PORTION_TOKEN:
            PRINTF("INPUT_PAY_PORTION_TOKEN\n");
            break;
        case INPUT_PAY_PORTION_RECIPIENT:
            PRINTF("INPUT_PAY_PORTION_RECIPIENT\n");
            break;
        case INPUT_PAY_PORTION_AMOUNT:
            PRINTF("INPUT_PAY_PORTION_AMOUNT\n");
            break;
        case INPUT_PERMIT2_PERMIT_LENGTH:
            PRINTF("INPUT_PERMIT2_PERMIT_LENGTH\n");
            break;
        case INPUT_PERMIT2_PERMIT_TOKEN:
            PRINTF("INPUT_PERMIT2_PERMIT_TOKEN\n");
            break;
        case INPUT_PERMIT2_PERMIT_AMOUNT:
            PRINTF("INPUT_PERMIT2_PERMIT_AMOUNT\n");
            break;
        case INPUT_PERMIT2_PERMIT_EXPIRATION:
            PRINTF("INPUT_PERMIT2_PERMIT_EXPIRATION\n");
            break;
        case INPUT_PERMIT2_PERMIT_NONCE:
            PRINTF("INPUT_PERMIT2_PERMIT_NONCE\n");
            break;
        case INPUT_PERMIT2_PERMIT_SPENDER:
            PRINTF("INPUT_PERMIT2_PERMIT_SPENDER\n");
            break;
        case INPUT_PERMIT2_PERMIT_SIG_DEADLINE:
            PRINTF("INPUT_PERMIT2_PERMIT_SIG_DEADLINE\n");
            break;
        case INPUT_PERMIT2_PERMIT_SIGNATURE_OFFSET:
            PRINTF("INPUT_PERMIT2_PERMIT_SIGNATURE_OFFSET\n");
            break;
        case INPUT_PERMIT2_PERMIT_SIGNATURE_LENGTH:
            PRINTF("INPUT_PERMIT2_PERMIT_SIGNATURE_LENGTH\n");
            break;
        case INPUT_PERMIT2_PERMIT_SIGNATURE:
            PRINTF("INPUT_PERMIT2_PERMIT_SIGNATURE\n");
            break;
        case INPUT_V2_SWAP_EXACT_IN_LENGTH:
            PRINTF("INPUT_V2_SWAP_EXACT_IN_LENGTH\n");
            break;
        case INPUT_V2_SWAP_EXACT_IN_RECIPIENT:
            PRINTF("INPUT_V2_SWAP_EXACT_IN_RECIPIENT\n");
            break;
        case INPUT_V2_SWAP_EXACT_IN_AMOUNT_IN:
            PRINTF("INPUT_V2_SWAP_EXACT_IN_AMOUNT_IN\n");
            break;
        case INPUT_V2_SWAP_EXACT_IN_AMOUNT_OUT_MIN:
            PRINTF("INPUT_V2_SWAP_EXACT_IN_AMOUNT_OUT_MIN\n");
            break;
        case INPUT_V2_SWAP_EXACT_IN_PATH_OFFSET:
            PRINTF("INPUT_V2_SWAP_EXACT_IN_PATH_OFFSET\n");
            break;
        case INPUT_V2_SWAP_EXACT_IN_PAYER_IS_USER:
            PRINTF("INPUT_V2_SWAP_EXACT_IN_PAYER_IS_USER\n");
            break;
        case INPUT_V2_SWAP_EXACT_IN_PATH_LENGTH:
            PRINTF("INPUT_V2_SWAP_EXACT_IN_PATH_LENGTH\n");
            break;
        case INPUT_V2_SWAP_EXACT_IN_PATH:
            PRINTF("INPUT_V2_SWAP_EXACT_IN_PATH\n");
            break;
        case INPUT_V2_SWAP_EXACT_OUT_LENGTH:
            PRINTF("INPUT_V2_SWAP_EXACT_OUT_LENGTH\n");
            break;
        case INPUT_V2_SWAP_EXACT_OUT_RECIPIENT:
            PRINTF("INPUT_V2_SWAP_EXACT_OUT_RECIPIENT\n");
            break;
        case INPUT_V2_SWAP_EXACT_OUT_AMOUNT_OUT:
            PRINTF("INPUT_V2_SWAP_EXACT_OUT_AMOUNT_OUT\n");
            break;
        case INPUT_V2_SWAP_EXACT_OUT_AMOUNT_IN_MAX:
            PRINTF("INPUT_V2_SWAP_EXACT_OUT_AMOUNT_IN_MAX\n");
            break;
        case INPUT_V2_SWAP_EXACT_OUT_PATH_OFFSET:
            PRINTF("INPUT_V2_SWAP_EXACT_OUT_PATH_OFFSET\n");
            break;
        case INPUT_V2_SWAP_EXACT_OUT_PAYER_IS_USER:
            PRINTF("INPUT_V2_SWAP_EXACT_OUT_PAYER_IS_USER\n");
            break;
        case INPUT_V2_SWAP_EXACT_OUT_PATH_LENGTH:
            PRINTF("INPUT_V2_SWAP_EXACT_OUT_PATH_LENGTH\n");
            break;
        case INPUT_V2_SWAP_EXACT_OUT_PATH:
            PRINTF("INPUT_V2_SWAP_EXACT_OUT_PATH\n");
            break;
        case INPUT_V3_SWAP_EXACT_IN_LENGTH:
            PRINTF("INPUT_V3_SWAP_EXACT_IN_LENGTH\n");
            break;
        case INPUT_V3_SWAP_EXACT_IN_RECIPIENT:
            PRINTF("INPUT_V3_SWAP_EXACT_IN_RECIPIENT\n");
            break;
        case INPUT_V3_SWAP_EXACT_IN_AMOUNT_IN:
            PRINTF("INPUT_V3_SWAP_EXACT_IN_AMOUNT_IN\n");
            break;
        case INPUT_V3_SWAP_EXACT_IN_AMOUNT_OUT_MIN:
            PRINTF("INPUT_V3_SWAP_EXACT_IN_AMOUNT_OUT_MIN\n");
            break;
        case INPUT_V3_SWAP_EXACT_IN_PATH_OFFSET:
            PRINTF("INPUT_V3_SWAP_EXACT_IN_PATH_OFFSET\n");
            break;
        case INPUT_V3_SWAP_EXACT_IN_PAYER_IS_USER:
            PRINTF("INPUT_V3_SWAP_EXACT_IN_PAYER_IS_USER\n");
            break;
        case INPUT_V3_SWAP_EXACT_IN_PATH_LENGTH:
            PRINTF("INPUT_V3_SWAP_EXACT_IN_PATH_LENGTH\n");
            break;
        case INPUT_V3_SWAP_EXACT_IN_PATH:
            PRINTF("INPUT_V3_SWAP_EXACT_IN_PATH\n");
            break;
        case INPUT_V3_SWAP_EXACT_OUT_LENGTH:
            PRINTF("INPUT_V3_SWAP_EXACT_OUT_LENGTH\n");
            break;
        case INPUT_V3_SWAP_EXACT_OUT_RECIPIENT:
            PRINTF("INPUT_V3_SWAP_EXACT_OUT_RECIPIENT\n");
            break;
        case INPUT_V3_SWAP_EXACT_OUT_AMOUNT_OUT:
            PRINTF("INPUT_V3_SWAP_EXACT_OUT_AMOUNT_OUT\n");
            break;
        case INPUT_V3_SWAP_EXACT_OUT_AMOUNT_IN_MAX:
            PRINTF("INPUT_V3_SWAP_EXACT_OUT_AMOUNT_IN_MAX\n");
            break;
        case INPUT_V3_SWAP_EXACT_OUT_PATH_OFFSET:
            PRINTF("INPUT_V3_SWAP_EXACT_OUT_PATH_OFFSET\n");
            break;
        case INPUT_V3_SWAP_EXACT_OUT_PAYER_IS_USER:
            PRINTF("INPUT_V3_SWAP_EXACT_OUT_PAYER_IS_USER\n");
            break;
        case INPUT_V3_SWAP_EXACT_OUT_PATH_LENGTH:
            PRINTF("INPUT_V3_SWAP_EXACT_OUT_PATH_LENGTH\n");
            break;
        case INPUT_V3_SWAP_EXACT_OUT_PATH:
            PRINTF("INPUT_V3_SWAP_EXACT_OUT_PATH\n");
            break;
        case UNEXPECTED_PARAMETER:
            PRINTF("UNEXPECTED_PARAMETER\n");
            break;
        default:
            PRINTF("!!!!! UNKNOWN !!!!!\n");
            break;
    }
}
