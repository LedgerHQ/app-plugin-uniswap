#include "plugin.h"
#include "debug.h"
#include "weth_token.h"
#include "uniswap_contract_helpers.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define WRAP_UNWRAP_NAME(io_type) (io_type == INPUT ? "wrap" : "unwrap")
#define IO_NAME(io_type)          (io_type == INPUT ? "input" : "output")
#define OPPOSITE_IO_NAME(io_type) (io_type == INPUT ? "output" : "input")

#define PRINT_PARAMETER(title, parameter)                \
    do {                                                 \
        PRINTF(title);                                   \
        for (uint8_t i = 0; i < PARAMETER_LENGTH; ++i) { \
            PRINTF("%02x", parameter[i]);                \
        }                                                \
        PRINTF("\n");                                    \
    } while (0);

static int add_parameters(uint8_t dest[PARAMETER_LENGTH], const uint8_t to_add[PARAMETER_LENGTH]) {
    PRINTF("current_value: %.*H\n", PARAMETER_LENGTH, (uint8_t *) dest);
    PRINTF("to_add: %.*H\n", PARAMETER_LENGTH, (uint8_t *) to_add);
    bool rem = 0;
    for (int i = PARAMETER_LENGTH - 1; i >= 0; --i) {
        uint16_t new = dest[i] + to_add[i] + (uint8_t) rem;
        rem = (new > UINT8_MAX);
        dest[i] = (uint8_t) new;
    }
    PRINTF("res: %.*H\n", PARAMETER_LENGTH, (uint8_t *) dest);
    if (rem) {
        return -1;
    } else {
        return 0;
    }
}

typedef enum command_type_e {
    V3_SWAP_EXACT_IN = 0x00,
    V3_SWAP_EXACT_OUT = 0x01,
    PERMIT2_TRANSFER_FROM = 0x02,
    PERMIT2_PERMIT_BATCH = 0x03,
    SWEEP = 0x04,
    TRANSFER = 0x05,
    PAY_PORTION = 0x06,

    V2_SWAP_EXACT_IN = 0x08,
    V2_SWAP_EXACT_OUT = 0x09,
    PERMIT2_PERMIT = 0x0a,
    WRAP_ETH = 0x0b,
    UNWRAP_WETH = 0x0c,
    PERMIT2_TRANSFER_FROM_BATCH = 0x0d,
    BALANCE_CHECK_ERC20 = 0x0e,

    // NFT-related command types
    SEAPORT = 0x10,
    LOOKS_RARE_721 = 0x11,
    NFTX = 0x12,
    CRYPTOPUNKS = 0x13,
    LOOKS_RARE_1155 = 0x14,
    OWNER_CHECK_721 = 0x15,
    OWNER_CHECK_1155 = 0x16,
    SWEEP_ERC721 = 0x17,

    X2Y2_721 = 0x18,
    SUDOSWAP = 0x19,
    NFT20 = 0x1a,
    X2Y2_1155 = 0x1b,
    FOUNDATION = 0x1c,
    SWEEP_ERC1155 = 0x1d,
    ELEMENT_MARKET = 0x1e,

    EXECUTE_SUB_PLAN = 0x20,
    SEAPORT_V1_4 = 0x21,
    APPROVE_ERC20 = 0x22,
} command_type_t;

static uint8_t prepare_reading_next_input(context_t *context) {
    if (context->current_command >= context->commands_number) {
        // We have read all expected inputs
        PRINTF("All command data have been read\n");
        context->next_param = UNEXPECTED_PARAMETER;
    } else {
        PRINTF("Preparing to read data for command (%d)\n", context->current_command);
        command_type_t current_command = context->commands[context->current_command];
        switch (current_command) {
            case V2_SWAP_EXACT_IN:
                PRINTF("Preparing to read V2_SWAP_EXACT_IN\n");
                context->next_param = INPUT_V2_SWAP_EXACT_IN_LENGTH;
                break;
            case V2_SWAP_EXACT_OUT:
                PRINTF("Preparing to read V2_SWAP_EXACT_OUT\n");
                context->next_param = INPUT_V2_SWAP_EXACT_OUT_LENGTH;
                break;
            case V3_SWAP_EXACT_IN:
                PRINTF("Preparing to read V3_SWAP_EXACT_IN\n");
                context->next_param = INPUT_V3_SWAP_EXACT_IN_LENGTH;
                break;
            case V3_SWAP_EXACT_OUT:
                PRINTF("Preparing to read V3_SWAP_EXACT_OUT\n");
                context->next_param = INPUT_V3_SWAP_EXACT_OUT_LENGTH;
                break;
            case PERMIT2_TRANSFER_FROM:
            case PERMIT2_PERMIT_BATCH:
            case PERMIT2_TRANSFER_FROM_BATCH:
            case PERMIT2_PERMIT:
                PRINTF("Preparing to read PERMIT2_X related command\n");
                context->next_param = INPUT_PERMIT2_LENGTH;
                break;
            case WRAP_ETH:
                PRINTF("Preparing to read WRAP_ETH\n");
                context->next_param = INPUT_WRAP_ETH_LENGTH;
                break;
            case UNWRAP_WETH:
                PRINTF("Preparing to read UNWRAP_WETH\n");
                context->next_param = INPUT_UNWRAP_WETH_LENGTH;
                break;
            case PAY_PORTION:
                PRINTF("Preparing to read PAY_PORTION\n");
                context->next_param = INPUT_PAY_PORTION_LENGTH;
                break;
            default:
                PRINTF("Error: command %d not handled\n", current_command);
                return -1;
        }
    }
    return 0;
}

// A wrap or an unwrap can be handled the same way thanks to the io_data_t structure.
static int add_wrap_or_unwrap(const uint8_t parameter[PARAMETER_LENGTH],
                              context_t *context,
                              io_type_t direction) {
    io_data_t *io_data;
    if (direction == OUTPUT) {
        PRINTF("&context->output\n");
        io_data = &context->output;
    } else {
        PRINTF("&context->input\n");
        io_data = &context->input;
    }

    if (allzeroes(parameter, PARAMETER_LENGTH) && direction == OUTPUT) {
        PRINTF("Sweep received\n");
        context->sweep_received = true;
    } else {
        PRINTF("io_data->asset_type %d\n", io_data->asset_type);
        if (io_data->asset_type == UNSET) {
            // Nothing received yet, we indicate we received data and set it's type as ETH
            PRINTF("Setting %s to ETH\n", IO_NAME(direction));
        } else if (io_data->asset_type == WETH) {
            // We have already received a v2 or v3 swap with WETH as input or output, change it to
            // ETH
            PRINTF("Switching %s from WETH to ETH\n", IO_NAME(direction));
        } else if (io_data->asset_type == ETH) {
            // We received a previous wrap or unwrap request, refuse
            PRINTF("Error: received a second %s command\n", WRAP_UNWRAP_NAME(direction));
            return -1;
        } else {
            // We have already received a swap with some != WETH token, refuse
            PRINTF("Error: received a %s command but %s is a token\n",
                   WRAP_UNWRAP_NAME(direction),
                   IO_NAME(direction));
            return -1;
        }
        io_data->asset_type = ETH;
        copy_parameter(io_data->u.wrap_unwrap_amount,
                       parameter,
                       sizeof(io_data->u.wrap_unwrap_amount));
        PRINT_PARAMETER("io_data->u.wrap_unwrap_amount: ", io_data->u.wrap_unwrap_amount);
    }

    return 0;
}

// // Set the type of input or output token. Handle it the same way thanks to the io_data_t
// structure. static int set_token(const uint8_t address[ADDRESS_LENGTH], io_data_t *io_data) {
//     if (token_is_weth(address)) {
//         PRINTF("Token to set is WETH\n");
//         if (io_data->asset_type == UNSET) {
//             PRINTF("Saving as first\n");
//             io_data->asset_type = WETH;
//         } else {
//             PRINTF("Already received something, check\n");
//             if (io_data->asset_type == UNKNOWN_TOKEN) {
//                 PRINTF("Error: input is WETH, previous input is not\n");
//                 return -1;
//             }
//         }
//     } else {
//         // Unknown Token
//         PRINTF("Token to set is unknown\n");
//         if (io_data->asset_type == UNSET) {
//             PRINTF("Saving as first\n");
//             io_data->asset_type = UNKNOWN_TOKEN;
//             memmove(io_data->u.address, address, ADDRESS_LENGTH);
//             PRINTF("io_data->u.address = 0x");
//             for (int i = 0; i < ADDRESS_LENGTH; ++i) {
//                 PRINTF("%02x", io_data->u.address[i]);
//             }
//             PRINTF("\n");
//         } else {
//             const uint8_t (*ref)[ADDRESS_LENGTH];
//             if (io_data->asset_type == WETH || io_data->asset_type == ETH) {
//                 ref = &weth_address;
//                 PRINTF("Comparing with WETH\n");
//             } else {
//                 ref = &io_data->u.address;
//                 PRINTF("Comparing with saved output token\n");
//             }
//             PRINTF("Trying to match : %.*H\n", ADDRESS_LENGTH, address);
//             PRINTF("with ref : %.*H\n", ADDRESS_LENGTH, *ref);
//             if (memcmp(*ref, address, ADDRESS_LENGTH) != 0) {
//                 PRINTF("Error: Output token mismatch\n");
//                 return -1;
//             }
//         }
//     }

//     return 0;
// }

// Set the type of input or output token. Handle it the same way thanks to the io_data_t structure.
static int set_token(const uint8_t address[ADDRESS_LENGTH], io_data_t *io_data) {
    PRINTF("Setting token\n");
    if (token_is_weth(address)) {
        PRINTF("Token to set is WETH\n");
        io_data->asset_type = WETH;
    } else {
        PRINTF("Token to set is unknown\n");
        io_data->asset_type = UNKNOWN_TOKEN;
        memmove(io_data->u.address, address, ADDRESS_LENGTH);
    }

    return 0;
}

static bool address_partially_matches_io(const uint8_t *address,
                                         io_data_t *io,
                                         bool accept_eth,
                                         uint8_t offset,
                                         uint8_t length) {
    if (io->asset_type == UNSET) {
        return false;
    }
    if (io->asset_type == ETH && !accept_eth) {
        return false;
    }

    const uint8_t(*ref)[ADDRESS_LENGTH];
    if (io->asset_type == WETH || io->asset_type == ETH) {
        PRINTF("Comparing with WETH\n");
        ref = &weth_address;
    } else {
        PRINTF("Comparing with saved token\n");
        ref = &io->u.address;
    }
    PRINTF("Trying to match : %.*H\n", length, address);
    PRINTF("Inside ref : %.*H at offset %d\n", ADDRESS_LENGTH, *ref, offset);
    return (memcmp(*ref + offset, address, length) == 0);
}

static bool address_matches_io(const uint8_t address[ADDRESS_LENGTH],
                               io_data_t *io,
                               bool accept_eth) {
    return address_partially_matches_io((const uint8_t *) address,
                                        io,
                                        accept_eth,
                                        0,
                                        ADDRESS_LENGTH);
}

static bool address_partially_matches_intermediate(const uint8_t *address,
                                                   intermediate_data_t *ref,
                                                   io_type_t address_direction,
                                                   uint8_t offset,
                                                   uint8_t length) {
    // Can only extend the intermediate if it's the opposite direction of a pair
    if ((address_direction == OUTPUT && ref->intermediate_status != INTERMEDIATE_INPUT) ||
        (address_direction == INPUT && ref->intermediate_status != INTERMEDIATE_OUTPUT)) {
        return false;
    }
    if (memcmp(address, ref->address + offset, length) == 0) {
        return true;
    }
    return false;
}

static bool address_matches_intermediate(const uint8_t address[ADDRESS_LENGTH],
                                         intermediate_data_t *ref,
                                         io_type_t address_direction) {
    return address_partially_matches_intermediate((const uint8_t *) address,
                                                  ref,
                                                  address_direction,
                                                  0,
                                                  ADDRESS_LENGTH);
}

static int handle_address_reception(uint8_t address[ADDRESS_LENGTH],
                                    io_data_t *this_io,
                                    io_data_t *opposite_io,
                                    intermediate_data_t *intermediate,
                                    io_type_t address_direction) {
    PRINTF("Handling reception of the %s address of a swap pair\n", IO_NAME(address_direction));
    if (address_matches_io(address, this_io, true)) {
        PRINTF("Same %s as previously, add amount to existing pool\n", IO_NAME(address_direction));
        if (add_parameters(this_io->amount, this_io->tmp_amount) != 0) {
            PRINTF("Error, overflow while adding amounts\n");
            return -1;
        }

    } else if (address_matches_io(address, opposite_io, false)) {
        PRINTF("This %s address extends the previously received %s\n",
               IO_NAME(address_direction),
               OPPOSITE_IO_NAME(address_direction));
        // Drop the saved IO, drop this received edge of the swap pair
        opposite_io->asset_type = UNSET;
        memset(opposite_io->amount, 0, INT256_LENGTH);

    } else if (address_matches_intermediate(address, intermediate, address_direction)) {
        PRINTF("This %s address extends the previously received %s intermediate\n",
               IO_NAME(address_direction),
               OPPOSITE_IO_NAME(address_direction));
        // Drop the saved IO intermediate, drop this received edge of the swap pair
        intermediate->intermediate_status = UNUSED;

    } else if (this_io->asset_type == UNSET) {
        PRINTF("No %s set yet, save this address as it\n", IO_NAME(address_direction));
        if (set_token(address, this_io) != 0) {
            PRINTF("Error in set_token\n");
            return -1;
        }
        memmove(this_io->amount, this_io->tmp_amount, PARAMETER_LENGTH);

    } else if (intermediate->intermediate_status == UNUSED) {
        PRINTF("This %s address did not match anything, treat it as the edge of a split swap\n",
               IO_NAME(address_direction));
        intermediate->intermediate_status = (intermediate_status_t) address_direction;
        memmove(intermediate->address, address, ADDRESS_LENGTH);

    } else {
        PRINTF(
            "This %s address did not match anything, and we already have an unresolved edge of a "
            "split swap\n",
            IO_NAME(address_direction));
        PRINTF("Input matched nothing and no space left\n");
        return -1;
    }

    return 0;
}

static int parse_v2_path(context_t *context,
                         const uint8_t parameter[PARAMETER_LENGTH],
                         bool *finished) {
    if (context->current_path_read == 0) {
        // First parameter is the input token
        PRINTF("Handling token IN\n");
        uint8_t address[ADDRESS_LENGTH];
        memmove(address, parameter + (PARAMETER_LENGTH - ADDRESS_LENGTH), ADDRESS_LENGTH);
        if (handle_address_reception(address,
                                     &context->input,
                                     &context->output,
                                     &context->intermediate,
                                     INPUT)) {
            PRINTF("Error handling input address\n");
            return -1;
        }
        *finished = false;

    } else if (context->current_path_read < context->path_length - 1) {
        PRINTF("Skipping intermediate token\n");
        *finished = false;

    } else {
        // Last parameter is the output token
        PRINTF("Handling token OUT\n");
        uint8_t address[ADDRESS_LENGTH];
        memmove(address, parameter + (PARAMETER_LENGTH - ADDRESS_LENGTH), ADDRESS_LENGTH);
        if (handle_address_reception(address,
                                     &context->output,
                                     &context->input,
                                     &context->intermediate,
                                     OUTPUT)) {
            PRINTF("Error handling output address\n");
            return -1;
        }
        *finished = true;
    }

    ++context->current_path_read;
    return 0;
}

static int parse_v3_path(context_t *context,
                         const uint8_t parameter[PARAMETER_LENGTH],
                         bool reversed,
                         bool *finished) {
    uint8_t current_read_this_cycle = 0;
    io_data_t *first_token_to_read;
    io_type_t first_direction;
    io_data_t *last_token_to_read;
    io_type_t last_direction;

    if (reversed) {
        PRINTF("Reading in reverse order (exact output swap)\n");
        first_token_to_read = &context->output;
        first_direction = OUTPUT;
        last_token_to_read = &context->input;
        last_direction = INPUT;
    } else {
        PRINTF("Reading in order (exact input swap)\n");
        first_token_to_read = &context->input;
        first_direction = INPUT;
        last_token_to_read = &context->output;
        last_direction = OUTPUT;
    }

    // Always read initial token first, before reading hops
    if (context->current_path_read == 0) {
        PRINTF("Handling %s token first\n", IO_NAME(first_direction));
        uint8_t address[ADDRESS_LENGTH];
        memmove(address, parameter, ADDRESS_LENGTH);
        context->current_path_read += ADDRESS_LENGTH;
        current_read_this_cycle += ADDRESS_LENGTH;
        // if (set_token(address, first_token_to_read) != 0) {
        //     PRINTF("Error in set_token for first for swap V3\n");
        //     return -1;
        // }
        if (handle_address_reception(address,
                                     first_token_to_read,
                                     last_token_to_read,
                                     &context->intermediate,
                                     first_direction)) {
            PRINTF("Error handling %s address\n", IO_NAME(first_direction));
            return -1;
        }
    }

    // Skip all intermediate network fees and tokens, and the network fees of the ouput
    if (context->current_path_read < (context->path_length - ADDRESS_LENGTH)) {
        uint8_t can_skip =
            MIN(PARAMETER_LENGTH - current_read_this_cycle,
                (context->path_length - ADDRESS_LENGTH) - context->current_path_read);
        PRINTF("Skipping %d bytes of unused fees / intermediate tokens\n", can_skip);
        context->current_path_read += can_skip;
        current_read_this_cycle += can_skip;
    }

    // We have unskipped data: this means that we have the last address left to read in current
    // parameter
    if (current_read_this_cycle < PARAMETER_LENGTH) {
        uint8_t offset_in_address =
            ADDRESS_LENGTH - (context->path_length - context->current_path_read);
        uint8_t can_read =
            MIN(PARAMETER_LENGTH - current_read_this_cycle, ADDRESS_LENGTH - offset_in_address);
        PRINTF("Reading last token offset %d, can read %d\n", offset_in_address, can_read);

        // We are reading the address of the LAST token but we have a RAM size issue:
        //     - the token address may be split in two chunks
        //     - we would have to reconstruct it to know how to handle it
        //     - we can't afford to have this new 20 bytes buffer in RAM
        // Two possibilities:
        //     - we don't have a split reception in progress :
        //         - context->intermediate.address is free
        //         - store the token address in it and decide once it is reconstructed
        //     - we have a split reception in progress:
        //         - context->intermediate.address is NOT free
        //         - only three valid possibilities:
        //             - this address fits the current INPUT -> OUTPUT
        //             - this address extends the current INPUT -> OUTPUT
        //             - this address extends the current dangling split swap
        //         - fail otherwise
        if (context->intermediate.intermediate_status == UNUSED ||
            context->intermediate.intermediate_status == WRITING) {
            context->intermediate.intermediate_status = WRITING;
            memmove(context->intermediate.address + offset_in_address,
                    parameter + current_read_this_cycle,
                    can_read);
        } else {
            PRINTF("Intermediate buffer is not free: match or fail\n");
            bool unused = (context->intermediate.split_reception_status == SPLIT_RECEPTION_UNUSED);
            if (unused || context->intermediate.split_reception_status & MATCHING_OWN_IO) {
                if (address_partially_matches_io(parameter + current_read_this_cycle,
                                                 last_token_to_read,
                                                 true,
                                                 offset_in_address,
                                                 can_read)) {
                    PRINTF("Address fits with current IO\n");
                    context->intermediate.split_reception_status |= MATCHING_OWN_IO;
                } else {
                    context->intermediate.split_reception_status &= ~MATCHING_OWN_IO;
                }
            }
            if (unused || context->intermediate.split_reception_status & MATCHING_OPPOSING_IO) {
                if (address_partially_matches_io(parameter + current_read_this_cycle,
                                                 first_token_to_read,
                                                 false,
                                                 offset_in_address,
                                                 can_read)) {
                    PRINTF("Address extends current IO\n");
                    context->intermediate.split_reception_status |= MATCHING_OPPOSING_IO;
                } else {
                    context->intermediate.split_reception_status &= ~MATCHING_OPPOSING_IO;
                }
            }
            if (unused || context->intermediate.split_reception_status & MATCHING_INTERMEDIATE) {
                if (address_partially_matches_intermediate(parameter + current_read_this_cycle,
                                                           &context->intermediate,
                                                           last_direction,
                                                           offset_in_address,
                                                           can_read)) {
                    PRINTF("Address matches with intermediate\n");
                    context->intermediate.split_reception_status |= MATCHING_INTERMEDIATE;
                } else {
                    context->intermediate.split_reception_status &= ~MATCHING_INTERMEDIATE;
                }
            }

            if (context->intermediate.split_reception_status == SPLIT_RECEPTION_UNUSED) {
                PRINTF("Address matches with nothing\n");
                return -1;
            }
        }
        context->current_path_read += can_read;
        current_read_this_cycle += can_read;
        // Cast to avoid useless clang warning
        (void) current_read_this_cycle;
    }

    // Some cleanup if we have read everything
    if (context->current_path_read == context->path_length) {
        PRINTF("Finished reading path\n");
        if (context->intermediate.intermediate_status == WRITING) {
            PRINTF("Intermediate buffer was used as temp storage\n");
            uint8_t address[ADDRESS_LENGTH];
            memmove(address, context->intermediate.address, ADDRESS_LENGTH);
            context->intermediate.intermediate_status = UNUSED;
            if (handle_address_reception(address,
                                         last_token_to_read,
                                         first_token_to_read,
                                         &context->intermediate,
                                         last_direction)) {
                PRINTF("Error handling %s address\n", IO_NAME(last_direction));
                return -1;
            }
        } else {
            if (context->intermediate.split_reception_status & MATCHING_OWN_IO) {
                PRINTF("Same %s as previously, add amount to existing pool\n",
                       IO_NAME(last_direction));
                if (add_parameters(last_token_to_read->amount, last_token_to_read->tmp_amount) !=
                    0) {
                    PRINTF("Error, overflow while adding amounts\n");
                    return -1;
                }
            } else if (context->intermediate.split_reception_status & MATCHING_OPPOSING_IO) {
                // Drop the saved IO, drop this received edge of the swap pair
                first_token_to_read->asset_type = UNSET;
                memset(first_token_to_read->amount, 0, INT256_LENGTH);
            } else if (context->intermediate.split_reception_status & MATCHING_INTERMEDIATE) {
                // Drop the saved IO intermediate, drop this received edge of the swap pair
                context->intermediate.intermediate_status = UNUSED;
            } else {
                // Unreachable per construct
                return -1;
            }
            context->intermediate.split_reception_status = 0;
        }
        *finished = true;
    } else {
        *finished = false;
    }
    return 0;
}

static int check_or_set_swap_type(context_t *context, swap_type_t swap_type) {
    if (context->swap_type == NONE) {
        context->swap_type = swap_type;
    } else if (context->swap_type != swap_type) {
        PRINTF("Error: can't mix swap types\n");
        return -1;
    }
    return 0;
}

// Check if the address is 0000000000000000000000000000000000000002
static bool is_router_address(const uint8_t address[ADDRESS_LENGTH]) {
    for (uint8_t i = 0; i < ADDRESS_LENGTH - 1; ++i) {
        if (address[i] != 0) {
            return false;
        }
    }
    return (address[ADDRESS_LENGTH - 1] == 2);
}

static int handle_recipient(const uint8_t parameter[PARAMETER_LENGTH], context_t *context) {
    PRINTF("handle_recipient\n");
    if (context->recipient_set) {
        if (memcmp(context->recipient,
                   parameter + PARAMETER_LENGTH - ADDRESS_LENGTH,
                   ADDRESS_LENGTH) != 0) {
            PRINTF("Error: can't mix different recipients\n");
            PRINTF("Received: %.*H\n",
                   ADDRESS_LENGTH,
                   parameter + PARAMETER_LENGTH - ADDRESS_LENGTH);
            PRINTF("Expected: %.*H\n", ADDRESS_LENGTH, context->recipient);
            return -1;
        }
    } else {
        context->recipient_set = true;
        memmove(context->recipient, parameter + PARAMETER_LENGTH - ADDRESS_LENGTH, ADDRESS_LENGTH);
    }
    return 0;
}

// Size of the network fees element in a V3 path
#define NETWORK_FEE_LENGTH 3

static bool v3_path_length_is_valid(uint8_t path_length) {
    if (path_length < 2 * ADDRESS_LENGTH + NETWORK_FEE_LENGTH) {
        PRINTF("Path length %d is too small to make sense\n", path_length);
        return false;
    } else if ((path_length - ADDRESS_LENGTH) % (ADDRESS_LENGTH + NETWORK_FEE_LENGTH) != 0) {
        PRINTF("Path length %d is not address + fees sized\n", path_length);
        return false;
    } else {
        return true;
    }
}

static void handle_execute(ethPluginProvideParameter_t *msg, context_t *context) {
    // debug
    print_parameter_name(context->next_param);

    switch (context->next_param) {
            // ######################
            // Parsing EXECUTE HEADER
            // ######################

        case COMMANDS_OFFSET:
            context->next_param = INPUTS_HEADER_OFFSET;
            break;
        case INPUTS_HEADER_OFFSET:
            context->next_param = DEADLINE;
            break;
        case DEADLINE:
            context->next_param = COMMANDS_LENGTH;
            break;
        case COMMANDS_LENGTH:
            context->commands_number = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            if (context->commands_number > sizeof(context->commands)) {
                PRINTF("Error: too many commands\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = COMMANDS;
            break;
        case COMMANDS:
            memmove(context->commands, msg->parameter, context->commands_number);
            for (int i = 0; i < context->commands_number; ++i) {
                PRINTF("Command n %d: %02x\n", i, context->commands[i]);
            }
            context->next_param = INPUTS_NUMBER;
            break;
        case INPUTS_NUMBER: {
            uint8_t inputs_number = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            if (context->commands_number != inputs_number) {
                PRINTF("Error: context->commands_number != context->inputs_number\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            } else {
                context->current_input_offset_read = 0;
                context->next_param = INPUTS_OFFSET;
            }
        } break;
        case INPUTS_OFFSET:
            ++context->current_input_offset_read;
            if (context->current_input_offset_read == context->commands_number) {
                context->current_command = 0;
                if (prepare_reading_next_input(context) != 0) {
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                }
            }
            break;

            // ################
            // Parsing WRAP_ETH
            // ################

        case INPUT_WRAP_ETH_LENGTH:
            context->next_param = INPUT_WRAP_ETH_RECIPIENT;
            break;
        case INPUT_WRAP_ETH_RECIPIENT:
            if (!is_router_address(msg->parameter + PARAMETER_LENGTH - ADDRESS_LENGTH)) {
                PRINTF("Error unwrap recipient is not the router address\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = INPUT_WRAP_ETH_AMOUNT;
            break;
        case INPUT_WRAP_ETH_AMOUNT:
            if (add_wrap_or_unwrap(msg->parameter, context, INPUT) != 0) {
                PRINTF("Error in add_wrap_or_unwrap for input\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }

            ++context->current_command;
            if (prepare_reading_next_input(context) != 0) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            break;

            // ###################
            // Parsing UNWRAP_WETH
            // ###################

        case INPUT_UNWRAP_WETH_LENGTH:
            context->next_param = INPUT_UNWRAP_WETH_RECIPIENT;
            break;
        case INPUT_UNWRAP_WETH_RECIPIENT:
            if (!is_router_address(msg->parameter + PARAMETER_LENGTH - ADDRESS_LENGTH)) {
                PRINTF("Error unwrap recipient is not the router address\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = INPUT_UNWRAP_WETH_AMOUNT;
            break;
        case INPUT_UNWRAP_WETH_AMOUNT:
            if (add_wrap_or_unwrap(msg->parameter, context, OUTPUT) != 0) {
                PRINTF("Error in add_wrap_or_unwrap for output\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
            }

            ++context->current_command;
            if (prepare_reading_next_input(context) != 0) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            break;

            // ###################
            // Parsing PAY_PORTION
            // ###################

        case INPUT_PAY_PORTION_LENGTH:
            context->next_param = INPUT_PAY_PORTION_TOKEN;
            break;
        case INPUT_PAY_PORTION_TOKEN:
            context->next_param = INPUT_PAY_PORTION_RECIPIENT;
            break;
        case INPUT_PAY_PORTION_RECIPIENT:
            context->next_param = INPUT_PAY_PORTION_AMOUNT;
            break;
        case INPUT_PAY_PORTION_AMOUNT: {
            uint16_t new;
            if (!U2BE_from_parameter(msg->parameter, &new)) {
                PRINTF("Error: Not a valid basis point amount\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            } else {
                uint32_t pay_portion_sum = context->pay_portion_amount + new;
                PRINTF("pay_portion_sum %d\n", pay_portion_sum);
                if (pay_portion_sum > 10000) {
                    PRINTF("Error: Not a valid basis point amount\n");
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                } else {
                    context->pay_portion_amount = pay_portion_sum;
                    ++context->current_command;
                    if (prepare_reading_next_input(context) != 0) {
                        msg->result = ETH_PLUGIN_RESULT_ERROR;
                    }
                }
            }
        } break;

            // ######################
            // Parsing PERMIT2_X
            // ######################

        case INPUT_PERMIT2_LENGTH:
            context->permit2_length = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->current_permit_read = 0;
            context->next_param = INPUT_PERMIT2_SKIP_TOKEN;
            break;
        case INPUT_PERMIT2_SKIP_TOKEN:
            context->current_permit_read += PARAMETER_LENGTH;
            if (context->current_permit_read >= context->permit2_length) {
                ++context->current_command;
                if (prepare_reading_next_input(context) != 0) {
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                }
            }
            break;

        // ########################
        // Parsing V2_SWAP_EXACT_IN
        // ########################
        case INPUT_V2_SWAP_EXACT_IN_LENGTH:
            if (check_or_set_swap_type(context, EXACT_IN) != 0) {
                PRINTF("Error: check_or_set_swap_type failed for EXACT_IN\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = INPUT_V2_SWAP_EXACT_IN_RECIPIENT;
            break;
        case INPUT_V2_SWAP_EXACT_IN_RECIPIENT:
            if (handle_recipient(msg->parameter, context) != 0) {
                PRINTF("Error: handle_recipient failed\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = INPUT_V2_SWAP_EXACT_IN_AMOUNT_IN;
            break;
        case INPUT_V2_SWAP_EXACT_IN_AMOUNT_IN:
            memmove(context->input.tmp_amount, msg->parameter, PARAMETER_LENGTH);
            context->next_param = INPUT_V2_SWAP_EXACT_IN_AMOUNT_OUT_MIN;
            break;
        case INPUT_V2_SWAP_EXACT_IN_AMOUNT_OUT_MIN:
            memmove(context->output.tmp_amount, msg->parameter, PARAMETER_LENGTH);
            context->next_param = INPUT_V2_SWAP_EXACT_IN_PATH_OFFSET;
            break;
        case INPUT_V2_SWAP_EXACT_IN_PATH_OFFSET:
            context->next_param = INPUT_V2_SWAP_EXACT_IN_PAYER_IS_USER;
            break;
        case INPUT_V2_SWAP_EXACT_IN_PAYER_IS_USER:
            context->next_param = INPUT_V2_SWAP_EXACT_IN_PATH_LENGTH;
            break;
        case INPUT_V2_SWAP_EXACT_IN_PATH_LENGTH:
            context->path_length = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            if (context->path_length < 2) {
                PRINTF("Path length is too small to make sense %d\n", context->path_length);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->current_path_read = 0;
            context->next_param = INPUT_V2_SWAP_EXACT_IN_PATH;
            break;
        case INPUT_V2_SWAP_EXACT_IN_PATH: {
            bool finished;
            if (parse_v2_path(context, msg->parameter, &finished) != 0) {
                PRINTF("Failed to parse path for V2_SWAP_EXACT_IN\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
            }

            // If we have finished reading this path, prepare reading input for next command
            if (finished) {
                ++context->current_command;
                if (prepare_reading_next_input(context) != 0) {
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                }
            }
        } break;

            // #########################
            // Parsing V2_SWAP_EXACT_OUT
            // #########################

        case INPUT_V2_SWAP_EXACT_OUT_LENGTH:
            if (check_or_set_swap_type(context, EXACT_OUT) != 0) {
                PRINTF("Error: check_or_set_swap_type failed for EXACT_OUT\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = INPUT_V2_SWAP_EXACT_OUT_RECIPIENT;
            break;
        case INPUT_V2_SWAP_EXACT_OUT_RECIPIENT:
            if (handle_recipient(msg->parameter, context) != 0) {
                PRINTF("Error: handle_recipient failed\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = INPUT_V2_SWAP_EXACT_OUT_AMOUNT_OUT;
            break;
        case INPUT_V2_SWAP_EXACT_OUT_AMOUNT_OUT:
            memmove(context->output.tmp_amount, msg->parameter, PARAMETER_LENGTH);
            context->next_param = INPUT_V2_SWAP_EXACT_OUT_AMOUNT_IN_MAX;
            break;
        case INPUT_V2_SWAP_EXACT_OUT_AMOUNT_IN_MAX:
            memmove(context->input.tmp_amount, msg->parameter, PARAMETER_LENGTH);
            context->next_param = INPUT_V2_SWAP_EXACT_OUT_PATH_OFFSET;
            break;
        case INPUT_V2_SWAP_EXACT_OUT_PATH_OFFSET:
            context->next_param = INPUT_V2_SWAP_EXACT_OUT_PAYER_IS_USER;
            break;
        case INPUT_V2_SWAP_EXACT_OUT_PAYER_IS_USER:
            context->next_param = INPUT_V2_SWAP_EXACT_OUT_PATH_LENGTH;
            break;
        case INPUT_V2_SWAP_EXACT_OUT_PATH_LENGTH:
            context->path_length = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            if (context->path_length < 2) {
                PRINTF("Path length is too small to make sense %d\n", context->path_length);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->current_path_read = 0;
            context->next_param = INPUT_V2_SWAP_EXACT_OUT_PATH;
            break;
        case INPUT_V2_SWAP_EXACT_OUT_PATH: {
            bool finished;
            if (parse_v2_path(context, msg->parameter, &finished) != 0) {
                PRINTF("Failed to parse path for V2_SWAP_EXACT_OUT\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
            }

            // If we have finished reading this path, prepare reading input for next command
            if (finished) {
                ++context->current_command;
                if (prepare_reading_next_input(context) != 0) {
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                }
            }
        } break;

            // ########################
            // Parsing V3_SWAP_EXACT_IN
            // ########################

        case INPUT_V3_SWAP_EXACT_IN_LENGTH:
            if (check_or_set_swap_type(context, EXACT_IN) != 0) {
                PRINTF("Error: check_or_set_swap_type failed for EXACT_IN\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = INPUT_V3_SWAP_EXACT_IN_RECIPIENT;
            break;
        case INPUT_V3_SWAP_EXACT_IN_RECIPIENT:
            if (handle_recipient(msg->parameter, context) != 0) {
                PRINTF("Error: handle_recipient failed\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = INPUT_V3_SWAP_EXACT_IN_AMOUNT_IN;
            break;
        case INPUT_V3_SWAP_EXACT_IN_AMOUNT_IN:
            memmove(context->input.tmp_amount, msg->parameter, PARAMETER_LENGTH);
            context->next_param = INPUT_V3_SWAP_EXACT_IN_AMOUNT_OUT_MIN;
            break;
        case INPUT_V3_SWAP_EXACT_IN_AMOUNT_OUT_MIN:
            memmove(context->output.tmp_amount, msg->parameter, PARAMETER_LENGTH);
            context->next_param = INPUT_V3_SWAP_EXACT_IN_PATH_OFFSET;
            break;
        case INPUT_V3_SWAP_EXACT_IN_PATH_OFFSET:
            context->next_param = INPUT_V3_SWAP_EXACT_IN_PAYER_IS_USER;
            break;
        case INPUT_V3_SWAP_EXACT_IN_PAYER_IS_USER:
            context->next_param = INPUT_V3_SWAP_EXACT_IN_PATH_LENGTH;
            break;
        case INPUT_V3_SWAP_EXACT_IN_PATH_LENGTH:
            context->path_length = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            if (!v3_path_length_is_valid(context->path_length)) {
                PRINTF("Path length is invalid\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->current_path_read = 0;
            context->next_param = INPUT_V3_SWAP_EXACT_IN_PATH;
            break;
        case INPUT_V3_SWAP_EXACT_IN_PATH: {
            bool finished;
            if (parse_v3_path(context, msg->parameter, false, &finished) != 0) {
                PRINTF("Failed to parse path for V3_SWAP_EXACT_IN\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
            }
            // If we have finished reading this path, prepare reading input for next command
            if (finished) {
                ++context->current_command;
                if (prepare_reading_next_input(context) != 0) {
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                }
            }
        } break;

            // #########################
            // Parsing V3_SWAP_EXACT_OUT
            // #########################

        case INPUT_V3_SWAP_EXACT_OUT_LENGTH:
            if (check_or_set_swap_type(context, EXACT_OUT) != 0) {
                PRINTF("Error: check_or_set_swap_type failed for EXACT_OUT\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = INPUT_V3_SWAP_EXACT_OUT_RECIPIENT;
            break;
        case INPUT_V3_SWAP_EXACT_OUT_RECIPIENT:
            if (handle_recipient(msg->parameter, context) != 0) {
                PRINTF("Error: handle_recipient failed\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = INPUT_V3_SWAP_EXACT_OUT_AMOUNT_OUT;
            break;
        case INPUT_V3_SWAP_EXACT_OUT_AMOUNT_OUT:
            memmove(context->output.tmp_amount, msg->parameter, PARAMETER_LENGTH);
            context->next_param = INPUT_V3_SWAP_EXACT_OUT_AMOUNT_IN_MAX;
            break;
        case INPUT_V3_SWAP_EXACT_OUT_AMOUNT_IN_MAX:
            memmove(context->input.tmp_amount, msg->parameter, PARAMETER_LENGTH);
            context->next_param = INPUT_V3_SWAP_EXACT_OUT_PATH_OFFSET;
            break;
        case INPUT_V3_SWAP_EXACT_OUT_PATH_OFFSET:
            context->next_param = INPUT_V3_SWAP_EXACT_OUT_PAYER_IS_USER;
            break;
        case INPUT_V3_SWAP_EXACT_OUT_PAYER_IS_USER:
            context->next_param = INPUT_V3_SWAP_EXACT_OUT_PATH_LENGTH;
            break;
        case INPUT_V3_SWAP_EXACT_OUT_PATH_LENGTH:
            context->path_length = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            if (!v3_path_length_is_valid(context->path_length)) {
                PRINTF("Path length is invalid\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->current_path_read = 0;
            context->next_param = INPUT_V3_SWAP_EXACT_OUT_PATH;
            break;

        case INPUT_V3_SWAP_EXACT_OUT_PATH: {
            bool finished;
            if (parse_v3_path(context, msg->parameter, true, &finished) != 0) {
                PRINTF("Failed to parse path for V3_SWAP_EXACT_OUT\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
            }
            // If we have finished reading this path, prepare reading input for next command
            if (finished) {
                ++context->current_command;
                if (prepare_reading_next_input(context) != 0) {
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                }
            }
        } break;

            // ###
            // END
            // ###

        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(ethPluginProvideParameter_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    // EDIT THIS: adapt the cases and the names of the functions.
    switch (context->selectorIndex) {
        case EXECUTE:
            handle_execute(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
