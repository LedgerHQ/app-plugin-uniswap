/*******************************************************************************
 *   Plugin Boilerplate
 *   (c) 2023 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#pragma once

#include <string.h>
#include "eth_plugin_interface.h"

// All possible selectors of your plugin.
// EDIT THIS: Enter your selectors here, in the format X(NAME, value)
// A Xmacro below will create for you:
//     - an enum named selector_t with every NAME
//     - a map named SELECTORS associating each NAME with it's value
#define SELECTORS_LIST(X) X(EXECUTE, 0x3593564c)

// Xmacro helpers to define the enum and map
// Do not modify !
#define TO_ENUM(selector_name, selector_id)  selector_name,
#define TO_VALUE(selector_name, selector_id) selector_id,

// This enum will be automatically expanded to hold all selector names.
// The value SELECTOR_COUNT can be used to get the number of defined selectors
// Do not modify !
typedef enum selector_e {
    SELECTORS_LIST(TO_ENUM) SELECTOR_COUNT,
} selector_t;

// This array will be automatically expanded to map all selector_t names with the correct value.
// Do not modify !
extern const uint32_t SELECTORS[SELECTOR_COUNT];

// Enumeration used to parse the smart contract data.
typedef enum parameter_e {
    COMMANDS_OFFSET = 0,
    INPUTS_HEADER_OFFSET,
    DEADLINE,
    COMMANDS_LENGTH,
    COMMANDS,
    INPUTS_NUMBER,
    INPUTS_OFFSET,

    // Parsing WRAP_ETH
    INPUT_WRAP_ETH_LENGTH,
    INPUT_WRAP_ETH_RECIPIENT,
    INPUT_WRAP_ETH_AMOUNT,

    // Parsing UNWRAP_WETH
    INPUT_UNWRAP_WETH_LENGTH,
    INPUT_UNWRAP_WETH_RECIPIENT,
    INPUT_UNWRAP_WETH_AMOUNT,

    // Parsing UNWRAP_WETH
    INPUT_PAY_PORTION_LENGTH,
    INPUT_PAY_PORTION_TOKEN,
    INPUT_PAY_PORTION_RECIPIENT,
    INPUT_PAY_PORTION_AMOUNT,

    // Parsing PERMIT2_PERMIT
    INPUT_PERMIT2_LENGTH,
    INPUT_PERMIT2_SKIP_TOKEN,

    // Parsing V2_SWAP_EXACT_IN
    INPUT_V2_SWAP_EXACT_IN_LENGTH,
    INPUT_V2_SWAP_EXACT_IN_RECIPIENT,
    INPUT_V2_SWAP_EXACT_IN_AMOUNT_IN,
    INPUT_V2_SWAP_EXACT_IN_AMOUNT_OUT_MIN,
    INPUT_V2_SWAP_EXACT_IN_PATH_OFFSET,
    INPUT_V2_SWAP_EXACT_IN_PAYER_IS_USER,
    INPUT_V2_SWAP_EXACT_IN_PATH_LENGTH,
    INPUT_V2_SWAP_EXACT_IN_PATH,

    // Parsing V2_SWAP_EXACT_OUT
    INPUT_V2_SWAP_EXACT_OUT_LENGTH,
    INPUT_V2_SWAP_EXACT_OUT_RECIPIENT,
    INPUT_V2_SWAP_EXACT_OUT_AMOUNT_OUT,
    INPUT_V2_SWAP_EXACT_OUT_AMOUNT_IN_MAX,
    INPUT_V2_SWAP_EXACT_OUT_PATH_OFFSET,
    INPUT_V2_SWAP_EXACT_OUT_PAYER_IS_USER,
    INPUT_V2_SWAP_EXACT_OUT_PATH_LENGTH,
    INPUT_V2_SWAP_EXACT_OUT_PATH,

    // Parsing V3_SWAP_EXACT_IN
    INPUT_V3_SWAP_EXACT_IN_LENGTH,
    INPUT_V3_SWAP_EXACT_IN_RECIPIENT,
    INPUT_V3_SWAP_EXACT_IN_AMOUNT_IN,
    INPUT_V3_SWAP_EXACT_IN_AMOUNT_OUT_MIN,
    INPUT_V3_SWAP_EXACT_IN_PATH_OFFSET,
    INPUT_V3_SWAP_EXACT_IN_PAYER_IS_USER,
    INPUT_V3_SWAP_EXACT_IN_PATH_LENGTH,
    INPUT_V3_SWAP_EXACT_IN_PATH,

    // Parsing V3_SWAP_EXACT_OUT
    INPUT_V3_SWAP_EXACT_OUT_LENGTH,
    INPUT_V3_SWAP_EXACT_OUT_RECIPIENT,
    INPUT_V3_SWAP_EXACT_OUT_AMOUNT_OUT,
    INPUT_V3_SWAP_EXACT_OUT_AMOUNT_IN_MAX,
    INPUT_V3_SWAP_EXACT_OUT_PATH_OFFSET,
    INPUT_V3_SWAP_EXACT_OUT_PAYER_IS_USER,
    INPUT_V3_SWAP_EXACT_OUT_PATH_LENGTH,
    INPUT_V3_SWAP_EXACT_OUT_PATH,

    UNEXPECTED_PARAMETER,
} parameter_t;

typedef enum asset_type_e {
    UNSET = 0,
    ETH,
    WETH,
    UNKNOWN_TOKEN,
    KNOWN_TOKEN,
} asset_type_t;

// Generic structure working for both the input of the swap and the output
typedef struct io_data_s {
    // Structure management byte
    asset_type_t asset_type;

    // Received amount in current command.
    // - Added to amount if the corresponding token address is the same
    // - Dropped if the corresponding token address is a swap intermediate
    uint8_t tmp_amount[INT256_LENGTH];

    // Amount of currency swapped
    uint8_t amount[INT256_LENGTH];
    union {
        // Amount of currency wrapped / unwrapped if applicable
        // Set at parsing stage
        // Used at finalize stage to ensure it matches 'amount'
        uint8_t wrap_unwrap_amount[INT256_LENGTH];

        // Address of the token if it is unknown
        // Set at parsing stage
        // Used again at query_ui stage to display the token address if no CAL data is provided
        // Used at finalize stage to request the token info
        uint8_t address[ADDRESS_LENGTH];

        // Token data from the CAL if the token is unknown
        // Set at provide_info stage if CAL data is provided
        // Used at query_ui stage to display the token info if CAL data is provided
        struct {
            uint8_t decimals;
            char ticker[MAX_TICKER_LEN];
        } token_info;
    } u;
} io_data_t;

typedef enum intermediate_status_e {
    UNUSED = 0,
    WRITING,
    INTERMEDIATE_INPUT,
    INTERMEDIATE_OUTPUT,
} intermediate_status_t;

// Subset of intermediate_status_t
typedef enum io_type_e {
    INPUT = INTERMEDIATE_INPUT,
    OUTPUT = INTERMEDIATE_OUTPUT,
} io_type_t;

typedef enum split_reception_status_e {
    SPLIT_RECEPTION_UNUSED = 0,
    MATCHING_OWN_IO = (0x1 << 0),
    MATCHING_OPPOSING_IO = (0x1 << 1),
    MATCHING_INTERMEDIATE = (0x1 << 2),
    WRITTING_IN_IO = (0x1 << 3),
    WRITTING_IN_INTERMEDIATE = (0x1 << 4),
} split_reception_status_t;

// Generic structure working for both the input of the swap and the output
typedef struct intermediate_data_s {
    split_reception_status_t split_reception_status;

    // Structure management byte
    intermediate_status_t intermediate_status;

    // Amount of currency swapped
    uint8_t address[ADDRESS_LENGTH];

    // uint8_t tmp_address_in[ADDRESS_LENGTH];
    // uint8_t tmp_address_out[ADDRESS_LENGTH];
} intermediate_data_t;

typedef enum swap_type_e {
    NONE,
    EXACT_IN,
    EXACT_OUT,
} swap_type_t;

// Arbitrary number, I have not seen an example of a TX using more than 8 commands
#define MAX_COMMANDS_HANDLED 8

typedef struct context_s {
    // Set to be the next param we expect to parse.
    parameter_t next_param;

    // Used at query_ui stage to display the correct string
    swap_type_t swap_type;

    // A Uniswap TX is split in commands. We will first receive a summary of commands to come, then
    // the input for each command.
    // We save the expected commands when reading the header, then read / increment the current
    // command counter
    uint8_t commands_number;
    uint8_t commands[MAX_COMMANDS_HANDLED];
    uint8_t current_command;

    // We need to save the length of some inputs when receiving them
    // As we can't be reading both at the same time and this data is useless once the parsing is
    // done, we can safely unionize the lengths
    union {
        // Used to skip the permit2
        uint16_t permit2_length;
        // Used to know the size of a swap path
        uint16_t path_length;
    };

    // We need to save some offsets for parsing purposes
    // As we can't be reading both at the same time and this data is useless once the parsing is
    // done, we can safely unionize the offsets
    union {
        uint16_t current_permit_read;
        uint16_t current_path_read;
        uint16_t current_input_offset_read;
    };

    // The data for the input of the swap
    io_data_t input;
    // The data for the output of the swap
    io_data_t output;

    bool sweep_received;

    uint16_t pay_portion_amount;

    bool recipient_set;
    uint8_t recipient[ADDRESS_LENGTH];

    intermediate_data_t intermediate;

    uint8_t own_address[ADDRESS_LENGTH];

    selector_t selectorIndex;
} context_t;

// Check if the context structure will fit in the RAM section ETH will prepare for us
// Do not remove!
// ASSERT_SIZEOF_PLUGIN_CONTEXT(context_t);

// char (*__kaboom)[sizeof( selector_t )] = 1;

// Plugin-only memory allocated by the Ethereum application and used by the plugin.
// #define PLUGIN_CONTEXT_SIZE2 (10 * INT256_LENGTH)
// It is recommended to cast the raw uin8_t array to a structure meaningfull for your plugin
// Helper to check that the actual plugin context structure is not bigger than the allocated memory
// #define ASSERT_SIZEOF_PLUGIN_CONTEXT2(s) \
//     _Static_assert(sizeof(s) <= PLUGIN_CONTEXT_SIZE2, "Plugin context structure is too big.")
