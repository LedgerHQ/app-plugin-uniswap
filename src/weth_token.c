#include "weth_token.h"

#define WETH_ADDRESS                                                \
    {                                                               \
        0xc0, 0x2a, 0xaa, 0x39, 0xb2, 0x23, 0xfe, 0x8d, 0x0a, 0x0e, \
        0x5c, 0x4f, 0x27, 0xea, 0xd9, 0x08, 0x3c, 0x75, 0x6c, 0xc2, \
    };
const uint8_t weth_address[ADDRESS_LENGTH] = WETH_ADDRESS;

bool token_is_weth(const uint8_t token[ADDRESS_LENGTH]) {
    return (memcmp(weth_address, token, ADDRESS_LENGTH) == 0);
}
