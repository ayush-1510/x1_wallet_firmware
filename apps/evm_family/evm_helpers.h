/**
 * @file    evm_helpers.h
 * @author  Cypherock X1 Team
 * @brief   Utilities api definitions for EVM chains
 * @copyright Copyright (c) 2023 HODL TECH PTE LTD
 * <br/> You may obtain a copy of license at <a href="https://mitcc.org/"
 * target=_blank>https://mitcc.org/</a>
 */
#ifndef EVM_HELPERS_H
#define EVM_HELPERS_H

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*****************************************************************************
 * MACROS AND DEFINES
 *****************************************************************************/

#define EVM_DRV_LEGACY_DEPTH 4
#define EVM_DRV_BIP44_DEPTH 5
#define EVM_DRV_ACCOUNT_DEPTH 5

#define EVM_PUB_KEY_SIZE 65
#define EVM_SHORT_PUB_KEY_SIZE 33

#define EVM_DRV_ACCOUNT 0x80000000

/*****************************************************************************
 * TYPEDEFS
 *****************************************************************************/

/*****************************************************************************
 * EXPORTED VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 *****************************************************************************/

/**
 * @brief Verifies the derivation path.
 * @details The function supports checking derivation paths for HD wallets
 * Types of derivations:
 * legacy        : m/44'/60'/0'/x
 * bip44         : m/44'/60'/0'/0/x
 * account model : m/44'/60'/x'/0/0
 *
 * @param[in] path      The derivation path as an uint32 array
 * @param[in] depth     The number of levels in the derivation path
 *
 * @return bool Indicates if the provided derivation path is valid
 * @retval true if the derivation path is valid
 * @retval false otherwise
 */
bool evm_derivation_path_guard(const uint32_t *path, uint32_t depth);

#endif /* EVM_HELPERS_H */