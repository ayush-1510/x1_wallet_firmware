/**
 * @file    starknet_helpers.c
 * @author  Cypherock X1 Team
 * @brief   Utilities specific to Starknet chains
 * @copyright Copyright (c) 2023 HODL TECH PTE LTD
 * <br/> You may obtain a copy of license at <a href="https://mitcc.org/"
 *target=_blank>https://mitcc.org/</a>
 *
 ******************************************************************************
 * @attention
 *
 * (c) Copyright 2023 by HODL TECH PTE LTD
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 * "Commons Clause" License Condition v1.0
 *
 * The Software is provided to you by the Licensor under the License,
 * as defined below, subject to the following condition.
 *
 * Without limiting other conditions in the License, the grant of
 * rights under the License will not include, and the License does not
 * grant to you, the right to Sell the Software.
 *
 * For purposes of the foregoing, "Sell" means practicing any or all
 * of the rights granted to you under the License to provide to third
 * parties, for a fee or other consideration (including without
 * limitation fees for hosting or consulting/ support services related
 * to the Software), a product or service whose value derives, entirely
 * or substantially, from the functionality of the Software. Any license
 * notice or attribution required by the License must also include
 * this Commons Clause License Condition notice.
 *
 * Software: All X1Wallet associated files.
 * License: MIT
 * Licensor: HODL TECH PTE LTD
 *
 ******************************************************************************
 */

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/

#include "starknet_helpers.h"

#include <error.pb.h>

#include "coin_utils.h"
#include "starknet_api.h"
#include "starknet_context.h"

/*****************************************************************************
 * EXTERN VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * PRIVATE MACROS AND DEFINES
 *****************************************************************************/

/*****************************************************************************
 * PRIVATE TYPEDEFS
 *****************************************************************************/

/*****************************************************************************
 * STATIC FUNCTION PROTOTYPES
 *****************************************************************************/

/**
 * @brief Grind starknet private from provided 32-byte seed
 */
bool grind_key(const uint8_t *grind_seed, uint8_t *out);

/*****************************************************************************
 * STATIC VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * GLOBAL VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * STATIC FUNCTIONS
 *****************************************************************************/

bool get_stark_child_node(const uint32_t *path,
                                 const size_t path_length,
                                 const char *curve,
                                 const uint8_t *seed,
                                 const uint8_t seed_len,
                                 HDNode *hdnode) {
  hdnode_from_seed(seed, seed_len, curve, hdnode);
  for (size_t i = 0; i < path_length; i++) {
    if (0 == hdnode_private_ckd(hdnode, path[i])) {
      // hdnode_private_ckd returns 1 when the derivation succeeds
      return false;
    }
  }
  hdnode_fill_public_key(hdnode);
  return true;
}

#include "mini-gmp.h"
#include "mini-gmp-helpers.h"
// mini-GMP
bool grind_key(const uint8_t *grind_seed, uint8_t *out) {
    uint8_t key[32] = {0};
    mpz_t strk_limit;
    mpz_t strk_key;
    mpz_t stark_order;

    // Initialize stark_order
    mpz_init_set_str(stark_order, "0800000000000010FFFFFFFFFFFFFFFFB781126DCAE7B2321E66A241ADC64D2F", 16);

    // Initialize strk_limit
    mpz_init_set_str(strk_limit, "F80000000000020EFFFFFFFFFFFFFFF738A13B4B920E9411AE6DA5F40B0358B1", 16);

    SHA256_CTX ctx = {0};
    mpz_init(strk_key);
    for (uint8_t itr = 0; itr < 200; itr++) {
        sha256_Init(&ctx);
        sha256_Update(&ctx, grind_seed, 32);
        sha256_Update(&ctx, &itr, 1);
        sha256_Final(&ctx, key);

        byte_array_to_mpz(strk_key, key, 32);
        if (mpz_cmp(strk_key, strk_limit) == -1) {
            mpz_t f_key;
            mpz_init(f_key);
            mpz_mod(f_key, strk_key, stark_order);
            mpz_to_byte_array(f_key, out, 32);
            return true;
        }
    }
    
    starknet_send_error(ERROR_COMMON_ERROR_UNKNOWN_ERROR_TAG, 0);
    printf("ERROR: grind 200 iterations failed\n");
    LOG_CRITICAL("grind 200 failed");
    return false;
}

/*****************************************************************************
 * GLOBAL FUNCTIONS
 *****************************************************************************/

bool starknet_derivation_path_guard(const uint32_t *path, uint8_t levels) {
  bool status = false;
  if (STARKNET_IMPLICIT_ACCOUNT_DEPTH != levels) {
    return status;
  }

  uint32_t purpose = path[0], coin = path[1], account = path[2],
           change = path[3], address = path[4];

  // m/44'/9004'/0'/0/i
  status = (STARKNET_PURPOSE_INDEX == purpose && STARKNET_COIN_INDEX == coin &&
            STARKNET_ACCOUNT_INDEX == account &&
            STARKNET_CHANGE_INDEX == change && is_non_hardened(address));

  return status;
}

bool starknet_derive_bip32_node(const uint8_t *seed, uint8_t *private_key) {
  uint32_t eth_acc0_path[] = {
      STARKNET_PURPOSE_INDEX, ETHEREUM, 0x80000000, 0, 0};
  HDNode strkSeedNode = {0};

  // derive node at m/44'/60'/0'/0/0
  if (!derive_hdnode_from_path(
          eth_acc0_path, 5, SECP256K1_NAME, seed, &strkSeedNode)) {
    return false;
  }

  memcpy(
      private_key, strkSeedNode.private_key, sizeof(strkSeedNode.private_key));

  return true;
}

#include <time.h>
bool starknet_derive_key_from_seed(const uint8_t *seed_key,
                                   const uint32_t *path,
                                   uint32_t path_length,
                                   uint8_t *key) {
  HDNode starkChildNode = {0};
  char hex[100];

  // derive node at m/44'/9004'/0'/0/i
  // send hdKey1.private_key as seed
  if (!get_stark_child_node(
          path, path_length, SECP256K1_NAME, seed_key, 32, &starkChildNode)) {
    // send unknown error; unknown failure reason
    starknet_send_error(ERROR_COMMON_ERROR_UNKNOWN_ERROR_TAG, 1);
    memzero(&starkChildNode, sizeof(HDNode));
    return false;
  }

  printf("\necdsaPrivateKey: ");
  for (size_t i = 0; i < 32; i++) {
  printf("%02x", starkChildNode.private_key[i]);
  }
  printf("\n");

  uint8_t stark_private_key[32];
  stark_point p;
  stark_point_init(&p);
  // uint8_t stark_public_key[32];

  struct timespec start, end;
  double time_taken;

  // Start time measurement
  clock_gettime(CLOCK_MONOTONIC, &start);

  if (!grind_key(starkChildNode.private_key, stark_private_key)) {
    return false;
  }

  // End time measurement
  clock_gettime(CLOCK_MONOTONIC, &end);
  
  printf("starkPrivateKey: ");
  for (size_t i = 0; i < 32; i++) {
  printf("%02x", stark_private_key[i]);
  }
  printf("\n");

  char str[100];
  starknet_init();
  mpz_t priv_key;
  mpz_init(priv_key);
  byte_array_to_mpz(priv_key, stark_private_key, 32);
  stark_point_multiply(starkCurve, priv_key, &starkCurve->G, &p);
  mpz_get_str(str, 16, p.x);
  printf("\nstarkPubKey x: %s\n", str);
  mpz_get_str(str, 16, p.y);
  printf("\nstarkPubKey y: %s\n", str);

  printf("\n");

  byte_array_to_hex_string(seed_key, 32, hex, 32 * 2 + 1);
  print_hex_array("seed_key", seed_key, 32);

  byte_array_to_hex_string(starkChildNode.private_key, 32, hex, 32 * 2 + 1);
  print_hex_array("starkChildNode.private_key", starkChildNode.private_key, 32);

  byte_array_to_hex_string(stark_private_key, 32, hex, 32 * 2 + 1);
  print_hex_array("stark_private_key", stark_private_key, 32);

  memzero(key, 33);
  memcpy(key, stark_private_key, 32);

  // Calculate the elapsed time in seconds
  time_taken = (end.tv_sec - start.tv_sec) + 
               (end.tv_nsec - start.tv_nsec) / 1e9;
  printf("\nTime taken by grind_key: %f seconds\n\n", time_taken);

  return true;
}
