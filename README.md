[![Ensure compliance with Ledger guidelines](https://github.com/LedgerHQ/app-plugin-uniswap/actions/workflows/guidelines_enforcer.yml/badge.svg?branch=develop)](https://github.com/LedgerHQ/app-plugin-uniswap/actions/workflows/guidelines_enforcer.yml)
[![Compilation & tests](https://github.com/LedgerHQ/app-plugin-uniswap/actions/workflows/build_and_functional_tests.yml/badge.svg?branch=develop)](https://github.com/LedgerHQ/app-plugin-uniswap/actions/workflows/build_and_functional_tests.yml)


# app-plugin-uniswap

This is the plugin to blind sign uniswap smart contract on Ledger device.

## Ethereum SDK

Ethereum plugins need the [Ethereum SDK](https://github.com/LedgerHQ/ethereum-plugin-sdk).
You can use the `ETHEREUM_PLUGIN_SDK` variable to point to the directory where you cloned
this repository. By default, the `Makefile` expects it to be at the root directory of this
plugin repository by the `ethereum-plugin-sdk` name.

You can see that this [CI](https://github.com/LedgerHQ/app-plugin-uniswap/blob/develop/.github/workflows/check_sdk.yml) workflow
verifies that the SDK used is either on the latest `master` or `develop` references. This ensures
the code is compiled and tested on the latest version of the SDK.

## Documentation

The documentation about the plugin shall be added in [PLUGIN_SPECIFICATON.md](https://github.com/LedgerHQ/app-plugin-uniswap/blob/develop/PLUGIN_SPECIFICATION.md). It shall includes at least the smart contracts and functions supported by the plugin.

## Formatting

The C source code is expected to be formatted with `clang-format` 11.0.0 or higher.
