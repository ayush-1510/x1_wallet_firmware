/**
 * @file    bittensor_helpers.h
 * @author  Cypherock X1 Team
 * @brief   Utilities api definitions for Bittensor chains
 * @copyright Copyright (c) 2023 HODL TECH PTE LTD
 * <br/> You may obtain a copy of license at <a href="https://mitcc.org/"
 * target=_blank>https://mitcc.org/</a>
 */
#ifndef BITTENSOR_HELPERS_H
#define BITTENSOR_HELPERS_H

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "coin_utils.h"

/*****************************************************************************
 * MACROS AND DEFINES
 *****************************************************************************/
#define BITTENSOR_NAME "Bittensor"
#define BITTENSOR_LUNIT "SOL"
#define BITTENSOR_PUB_KEY_SIZE 32
// Derivation path reference :
// https://docs.bittensor.com/wallet-guide/paper-wallet#hierarchical-derivation
// Taking 3 levels of depth similar to ledger
#define BITTENSOR_PURPOSE_INDEX 0x8000002C
#define BITTENSOR_COIN_INDEX 0x800001F5
#define BITTENSOR_ACCOUNT_INDEX 0x80000000
#define BITTENSOR_CHANGE_INDEX 0x80000000

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
 * address: m/44'/501'
 * address: m/44'/501'/i'
 * address: m/44'/501'/i'/0'
 *
 * @param[in] path      The derivation path as an uint32 array
 * @param[in] depth     The number of levels in the derivation path
 *
 * @return bool Indicates if the provided derivation path is valid
 * @retval true if the derivation path is valid
 * @retval false otherwise
 */
bool bittensor_derivation_path_guard(const uint32_t *path, uint8_t levels);

#endif    // BITTENSOR_HELPERS_H