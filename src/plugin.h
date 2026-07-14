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

    // Parsing PAY_PORTION
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
    INPUT_V2_SWAP_EXACT_IN_MINHOP_OFFSET,  // UR 2.1.1 layout only
    INPUT_V2_SWAP_EXACT_IN_PATH_LENGTH,
    INPUT_V2_SWAP_EXACT_IN_PATH,

    // Parsing V2_SWAP_EXACT_OUT
    INPUT_V2_SWAP_EXACT_OUT_LENGTH,
    INPUT_V2_SWAP_EXACT_OUT_RECIPIENT,
    INPUT_V2_SWAP_EXACT_OUT_AMOUNT_OUT,
    INPUT_V2_SWAP_EXACT_OUT_AMOUNT_IN_MAX,
    INPUT_V2_SWAP_EXACT_OUT_PATH_OFFSET,
    INPUT_V2_SWAP_EXACT_OUT_PAYER_IS_USER,
    INPUT_V2_SWAP_EXACT_OUT_MINHOP_OFFSET,  // UR 2.1.1 layout only
    INPUT_V2_SWAP_EXACT_OUT_PATH_LENGTH,
    INPUT_V2_SWAP_EXACT_OUT_PATH,

    // Parsing V3_SWAP_EXACT_IN
    INPUT_V3_SWAP_EXACT_IN_LENGTH,
    INPUT_V3_SWAP_EXACT_IN_RECIPIENT,
    INPUT_V3_SWAP_EXACT_IN_AMOUNT_IN,
    INPUT_V3_SWAP_EXACT_IN_AMOUNT_OUT_MIN,
    INPUT_V3_SWAP_EXACT_IN_PATH_OFFSET,
    INPUT_V3_SWAP_EXACT_IN_PAYER_IS_USER,
    INPUT_V3_SWAP_EXACT_IN_MINHOP_OFFSET,  // UR 2.1.1 layout only
    INPUT_V3_SWAP_EXACT_IN_PATH_LENGTH,
    INPUT_V3_SWAP_EXACT_IN_PATH,

    // Parsing V3_SWAP_EXACT_OUT
    INPUT_V3_SWAP_EXACT_OUT_LENGTH,
    INPUT_V3_SWAP_EXACT_OUT_RECIPIENT,
    INPUT_V3_SWAP_EXACT_OUT_AMOUNT_OUT,
    INPUT_V3_SWAP_EXACT_OUT_AMOUNT_IN_MAX,
    INPUT_V3_SWAP_EXACT_OUT_PATH_OFFSET,
    INPUT_V3_SWAP_EXACT_OUT_PAYER_IS_USER,
    INPUT_V3_SWAP_EXACT_OUT_MINHOP_OFFSET,  // UR 2.1.1 layout only
    INPUT_V3_SWAP_EXACT_OUT_PATH_LENGTH,
    INPUT_V3_SWAP_EXACT_OUT_PATH,

    // Skipping the minHopPriceX36 array trailing a UR 2.1.1 V2 / V3 path
    // (shared epilogue: advances to the next command when done).
    INPUT_MINHOP_LENGTH,
    INPUT_MINHOP_SKIP,

    // Parsing SWEEP
    INPUT_SWEEP_LENGTH,
    INPUT_SWEEP_TOKEN,
    INPUT_SWEEP_RECIPIENT,
    INPUT_SWEEP_AMOUNT,

    // Parsing the V4_SWAP envelope: the command input is itself
    // abi.encode(bytes actions, bytes[] params) — a second-level program.
    INPUT_V4_LENGTH,
    INPUT_V4_ACTIONS_OFFSET,
    INPUT_V4_PARAMS_OFFSET,
    INPUT_V4_ACTIONS_LENGTH,
    INPUT_V4_ACTIONS_DATA,
    INPUT_V4_PARAMS_NUMBER,
    INPUT_V4_PARAMS_OFFSET_ITEM,

    // Parsing one V4 SWAP action's params (SWAP_EXACT_IN / _OUT, path form).
    INPUT_V4_SWAP_PARAM_LENGTH,
    INPUT_V4_SWAP_TUPLE_OFFSET,
    INPUT_V4_SWAP_FIRST_CURRENCY,
    INPUT_V4_SWAP_PATH_OFFSET,
    INPUT_V4_SWAP_MINHOP_OFFSET,  // UR 2.1.1 layout only
    INPUT_V4_SWAP_AMOUNT_SPECIFIED,
    INPUT_V4_SWAP_AMOUNT_LIMIT,
    INPUT_V4_SWAP_PATH_LENGTH,
    INPUT_V4_SWAP_PATH_OFFSET_ITEM,
    INPUT_V4_SWAP_PATHKEY_CURRENCY,
    INPUT_V4_SWAP_PATHKEY_FEE,
    INPUT_V4_SWAP_PATHKEY_TICK_SPACING,
    INPUT_V4_SWAP_PATHKEY_HOOKS,
    INPUT_V4_SWAP_PATHKEY_HOOKDATA_OFFSET,
    INPUT_V4_SWAP_PATHKEY_HOOKDATA_LENGTH,
    INPUT_V4_SWAP_PATHKEY_HOOKDATA_SKIP,
    // Skipping the minHopPriceX36 array trailing a UR 2.1.1 V4 path
    // (returns to the next V4 action when done).
    INPUT_V4_SWAP_MINHOP_LENGTH,
    INPUT_V4_SWAP_MINHOP_SKIP,

    // Parsing one V4 SETTLE / SETTLE_ALL param. A SETTLE with an explicit
    // amount carries its leg's input amount (the following swap uses
    // OPEN_DELTA); SETTLE_ALL and open / contract-balance settles are skipped.
    INPUT_V4_SETTLE_PARAM_LENGTH,
    INPUT_V4_SETTLE_CURRENCY,
    INPUT_V4_SETTLE_AMOUNT,
    INPUT_V4_SETTLE_SKIP,

    // Parsing one V4 TAKE / TAKE_ALL param (carries the leg-output recipient).
    INPUT_V4_TAKE_PARAM_LENGTH,
    INPUT_V4_TAKE_CURRENCY,
    INPUT_V4_TAKE_RECIPIENT,
    INPUT_V4_TAKE_SKIP,

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

// Sized from production routing data: multi-protocol split routes have been
// observed using up to 12 commands.
#define MAX_COMMANDS_HANDLED 16

// Max V4 actions in a single V4_SWAP command. Production V4_SWAP programs use 3
// ([SWAP, SETTLE, TAKE]); 8 leaves ample headroom and anything larger rejects.
#define MAX_V4_ACTIONS 8

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

    // ===== V4_SWAP nested-program parsing state =====
    // The V4_SWAP command input is abi.encode(bytes actions, bytes[] params): a
    // second-level program. We decode the action opcodes, then walk each action's
    // param struct, feeding token/amount data into the same input/output/intermediate
    // machinery used by V2/V3 so route collapse and recipient handling are reused.
    uint8_t v4_actions_number;
    uint8_t v4_current_action;
    uint8_t v4_actions[MAX_V4_ACTIONS];
    bool v4_leg_exact_in;  // direction of the SWAP action currently being parsed
    // Generic per-param word/item counter (param-offset array, PathKey-offset
    // array, static-param skips). Lifetimes never overlap within one param.
    uint16_t v4_item_read;
    uint8_t v4_pathkey_count;
    uint8_t v4_pathkey_index;
    uint16_t v4_hookdata_skip;  // hookData bytes left to skip in the current PathKey
    // The swap struct's leading currency (input for exact-in, output for exact-out),
    // stashed until amounts are read and reception can be fed.
    uint8_t v4_first_currency[ADDRESS_LENGTH];
    // An explicit SETTLE amount waiting for its swap: production routes carry
    // the leg input in the SETTLE when the swap itself uses OPEN_DELTA (0).
    bool v4_settle_pending;
    uint8_t v4_settle_amount[INT256_LENGTH];

    // The data for the input of the swap
    io_data_t input;
    // The data for the output of the swap
    io_data_t output;

    bool unwrap_sweep_received;
    // Set when a custody wrap / unwrap converts an intermediate between a native
    // V4 leg and a wrapped V2 / V3 leg: its amount word is plumbing, not the
    // displayed input / output.
    bool wrap_unwrap_plumbing;

    // ===== UR 2.1.1 layout (minHopPriceX36) =====
    // Set per swap command / V4 swap param when its path offset reveals the
    // UR 2.1.1 layout: one extra head word (the minHop array offset) and a
    // trailing uint256[] to skip after the path.
    bool leg_v211;
    uint16_t minhop_skip;  // minHop array words left to skip

    uint16_t pay_portion_amount;
    bool sweep_received;
    bool skip_sweep_once;
    uint8_t sweep_amount[INT256_LENGTH];

    bool recipient_set;
    // Set when the recipient was fixed by a wrap / unwrap / sweep command: it is
    // user-facing and a later swap leg may not silently replace it.
    bool recipient_sticky;
    uint8_t recipient[ADDRESS_LENGTH];

    // Recipient of the swap leg currently being parsed. Only committed as the
    // displayed recipient once the leg's output is resolved: legs whose output is
    // consumed by a later leg are internal plumbing (their recipient is a pool or
    // the router) and must not conflict with the user-facing recipient.
    bool leg_recipient_set;
    uint8_t leg_recipient[ADDRESS_LENGTH];

    intermediate_data_t intermediate;

    uint8_t own_address[ADDRESS_LENGTH];

    // Index + 1 into the per-chain wrapped-native table (see weth_token.c), selected from the
    // transaction's chain ID at init. 0 means the chain has no known wrapped-native token.
    uint8_t wrapped_native_idx;

    selector_t selectorIndex;
} context_t;

// Check that the plugin context structure will fit in the ethereum allocated memory.
// Do not remove this check.
ASSERT_SIZEOF_PLUGIN_CONTEXT(context_t);
