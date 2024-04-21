/**
 * @file    tron_sign_txn.c
 * @author  Cypherock X1 Team
 * @brief   Tron chain transaction signing logic
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

#include "reconstruct_wallet_flow.h"
#include "tron_api.h"
#include "sha2.h"
#include "tron_priv.h"
#include "status_api.h"
#include "ui_core_confirm.h"
#include "ui_screens.h"
#include "wallet_list.h"
#include "curves.h"
#include <tron/sign_txn.pb.h>
#include <tron/contract.pb.h>
#include <tron/tron.pb.h>
#include <google/protobuf/any.pb.h>
#include <tron_txn_helpers.h>
#include "hasher.h"
#include <pb_decode.h>
#include "base58.h"

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
 * @brief Checks if the provided query contains expected request.
 * @details The function performs the check on the request type and if the check
 * fails, then it will send an error to the host bitcoin app and return false.
 *
 * @param query Reference to an instance of tron_query_t containing query
 * received from host app
 * @param which_request The expected request type enum
 *
 * @return bool Indicating if the check succeeded or failed
 * @retval true If the query contains the expected request
 * @retval false If the query does not contain the expected request
 */
static bool check_which_request(const tron_query_t *query,
                                pb_size_t which_request);

/**
 * @brief Validates the derivation path received in the request from host
 * @details The function validates the provided account derivation path in the
 * request. If invalid path is detected, the function will send an error to the
 * host and return false.
 *
 * @param request Reference to an instance of tron_sign_txn_request_t
 * @return bool Indicating if the verification passed or failed
 * @retval true If all the derivation path entries are valid
 * @retval false If any of the derivation path entries are invalid
 */
static bool validate_request_data(const tron_sign_txn_request_t *request);

/**
 * @brief The function prepares and sends empty responses
 *
 * @param which_response Constant value for the response type to be sent
 */
static void send_response(pb_size_t which_response);

/**
 * @brief Takes already received and decoded query for the user confirmation.
 * @details The function will verify if the query contains the TRON_SIGN_TXN
 * type of request. Additionally, the wallet-id is validated for sanity and the
 * derivation path for the account is also validated. After the validations,
 * user is prompted about the action for confirmation. The function returns true
 * indicating all the validation and user confirmation was a success.
 *
 * @param query Constant reference to the decoded query received from the host
 *
 * @return bool Indicating if the function actions succeeded or failed
 * @retval true If all the validation and user confirmation was positive
 * @retval false If any of the validation or user confirmation was negative
 */
STATIC bool tron_handle_initiate_query(const tron_query_t *query);

/**
 * @brief Fetches complete raw transaction to be signed for verification
 * @details The function will try to fetch the transaction by referring to the
 * declared size in tron_txn_context. The function will store complete
 * transaction into tron_txn_context.transaction.
 *
 * @param query Reference to an instance of tron_query_t for storing the
 * transient transaction chunks.
 *
 * @return bool Indicating if the whole transaction received and verified
 * @retval true If all the transaction was fetched and verified
 * @retval false If the transaction failed verification or wasn't fetched
 */
STATIC bool tron_fetch_valid_transaction(tron_query_t *query);

/**
 * @brief Aggregates user consent for the transaction info
 * @details The function decodes the receiver address along with the
 * corresponding transfer value in SOL.
 *
 *
 * @return bool Indicating if the user confirmed the transaction
 * @retval true If user confirmed the receiver address along with
 * the corresponding value.
 * @retval false Immediate return if any of the confirmation is disapproved
 */
STATIC bool tron_get_user_verification();

/**
 * @brief Fetches seed for generating public and private key
 * @details The function internally calls wallet reconstruction sub-flow to get
 * access to the seed.
 *
 * @param query Reference to an instance of tron_query_t to store transient
 * request from the host
 * @param seed_out seed recontructed from wallet
 * @return bool Indicating if signature for the provided transaction was
 * successfully generated
 * @retval true If all the seed is generated without any error
 * @retval false If seed failed to generate
 */
STATIC bool fetch_seed(tron_query_t *query, uint8_t *seed_out);

/**
 * @brief Sends the generated signature to the host
 * @details The function internally updates the unsigned transaction with a
 * recent blockhash and signs the transaction before sending to the host
 *
 * @param query Reference to an instance of tron_query_t to store transient
 * request from the host
 * @param seed seed reconstructed from wallet
 * @param sig Reference to the struct having the signature components
 * @return bool Indicating if the signature is sent to the host
 * @retval true If the signature was sent to host successfully
 * @retval false If the host responded with unknown/wrong query
 */
static bool send_signature(tron_query_t *query,
                           uint8_t *seed,
                           tron_sign_txn_signature_response_t *sig);

/*****************************************************************************
 * STATIC VARIABLES
 *****************************************************************************/

STATIC tron_txn_context_t *tron_txn_context = NULL;

/*****************************************************************************
 * GLOBAL VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * STATIC FUNCTIONS
 *****************************************************************************/

static bool check_which_request(const tron_query_t *query,
                                pb_size_t which_request) {
  if (which_request != query->sign_txn.which_request) {
    tron_send_error(ERROR_COMMON_ERROR_CORRUPT_DATA_TAG,
                      ERROR_DATA_FLOW_INVALID_REQUEST);
    return false;
  }

  return true;
}

static bool validate_request_data(const tron_sign_txn_request_t *request) {
  bool status = true;

  if (!tron_derivation_path_guard(request->initiate.derivation_path,
                                    request->initiate.derivation_path_count)) {
    tron_send_error(ERROR_COMMON_ERROR_CORRUPT_DATA_TAG,
                      ERROR_DATA_FLOW_INVALID_DATA);
    status = false;
  }

  return status;
}

static void send_response(const pb_size_t which_response) {
  tron_result_t result = init_tron_result(TRON_RESULT_SIGN_TXN_TAG);
  result.sign_txn.which_response = which_response;
  tron_send_result(&result);
}

STATIC bool tron_handle_initiate_query(const tron_query_t *query) {
  char wallet_name[NAME_SIZE] = "";
  char msg[100] = "";

  if (!check_which_request(query, TRON_SIGN_TXN_REQUEST_INITIATE_TAG) ||
      !validate_request_data(&query->sign_txn) ||
      !get_wallet_name_by_id(query->sign_txn.initiate.wallet_id,
                             (uint8_t *)wallet_name,
                             tron_send_error)) {
    return false;
  }

  snprintf(msg,
           sizeof(msg),
           UI_TEXT_SEND_TOKEN_PROMPT,
           TRON_LUNIT,
           TRON_NAME,
           wallet_name);
  // Take user consent to sign the transaction for the wallet
  if (!core_confirmation(msg, tron_send_error)) {
    return false;
  }

  set_app_flow_status(TRON_SIGN_TXN_STATUS_CONFIRM);
  send_response(TRON_SIGN_TXN_RESPONSE_CONFIRMATION_TAG);
  // show processing screen for a minimum duration (additional time will add due
  // to actual processing)
  delay_scr_init(ui_text_processing, DELAY_SHORT);
  return true;
}

STATIC bool tron_fetch_valid_transaction(tron_query_t *query) {
  uint32_t size = 0;
  tron_result_t response = init_tron_result(TRON_RESULT_SIGN_TXN_TAG);
  uint32_t total_size = tron_txn_context->init_info.transaction_size;
  const tron_sign_txn_data_t *txn_data = &query->sign_txn.txn_data;
  const common_chunk_payload_t *payload = &txn_data->chunk_payload;
  const common_chunk_payload_chunk_t *chunk = &txn_data->chunk_payload.chunk;

  // allocate memory for storing transaction
  tron_txn_context->transaction = (uint8_t *)malloc(total_size);
  while (1) {
    if (!tron_get_query(query, TRON_QUERY_SIGN_TXN_TAG) ||
        !check_which_request(query, TRON_SIGN_TXN_REQUEST_TXN_DATA_TAG)) {
      return false;
    }

    if (!txn_data->has_chunk_payload ||
        payload->chunk_index >= payload->total_chunks ||
        size + payload->chunk.size > total_size) {
      tron_send_error(ERROR_COMMON_ERROR_CORRUPT_DATA_TAG,
                        ERROR_DATA_FLOW_INVALID_DATA);
      return false;
    }

    memcpy(&tron_txn_context->transaction[size], chunk->bytes, chunk->size);
    size += chunk->size;
    // Send chunk ack to host
    response.sign_txn.which_response =
        TRON_SIGN_TXN_RESPONSE_DATA_ACCEPTED_TAG;
    response.sign_txn.data_accepted.has_chunk_ack = true;
    response.sign_txn.data_accepted.chunk_ack.chunk_index =
        payload->chunk_index;
    tron_send_result(&response);

    if (0 == payload->remaining_size ||
        payload->chunk_index + 1 == payload->total_chunks) {
      break;
    }
  }

  // make sure all chunks were received
  if (size != total_size) {
    tron_send_error(ERROR_COMMON_ERROR_CORRUPT_DATA_TAG,
                      ERROR_DATA_FLOW_INVALID_DATA);
    return false;
  }

  // decode and verify the received transaction
  if (0 != tron_byte_array_to_raw_txn(
                    tron_txn_context->transaction,
                    total_size,
                    tron_txn_context->raw_txn) ||
      0 !=
          tron_validate_unsigned_txn(tron_txn_context->raw_txn)) {
    return false;
  }

  return true;
}

STATIC bool extract_contract_info(tron_transaction_raw_t *raw_txn, 
                                  int64_t *amount, 
                                  uint8_t *to_address, 
                                  uint8_t *owner_address) {
    if (!(raw_txn->contract_count > 0)){
      tron_send_error(ERROR_COMMON_ERROR_CORRUPT_DATA_TAG,
                      ERROR_DATA_FLOW_INVALID_DATA);
      return false;
    }
    
    tron_transaction_contract_t contract = raw_txn->contract[0];  // Example for the first contract

    tron_transfer_contract_t transfer_contract = TRON_TRANSFER_CONTRACT_INIT_DEFAULT;
    google_protobuf_any_t any = contract.parameter;
    int state = memcmp(any.type_url, "type.googleapis.com/protocol.TransferContract", 64);
    // TODO: Add switch-cases for more contract types
    switch (state) {
      // case protocol_Transaction_Contract_ContractType_TransferContract:
      case 0:
      {
        pb_istream_t stream = pb_istream_from_buffer(any.value.bytes, any.value.size);

        if(!pb_decode(&stream, TRON_TRANSFER_CONTRACT_FIELDS, &transfer_contract)){
          tron_send_error(ERROR_COMMON_ERROR_CORRUPT_DATA_TAG, 
                          ERROR_DATA_FLOW_DECODING_FAILED);
          return false;
        }

        amount = &transfer_contract.amount;
        to_address = (uint8_t *)transfer_contract.to_address;
        owner_address = (uint8_t *)transfer_contract.owner_address;
      }

      default:
          tron_send_error(ERROR_COMMON_ERROR_UNKNOWN_ERROR_TAG,
                          ERROR_DATA_FLOW_INVALID_REQUEST);
          return false;
        }

      

      return true;
}



STATIC bool tron_get_user_verification() {
  uint8_t to_address[TRON_ACCOUNT_ADDRESS_LENGTH] = {0};
  uint8_t owner_address[TRON_ACCOUNT_ADDRESS_LENGTH] = {0};
  int64_t amount = 0;
  char address[TRON_ACCOUNT_ADDRESS_LENGTH + 1] = {0};
  // extract raw
  if(!extract_contract_info(tron_txn_context->raw_txn,
                            &amount,
                            to_address,
                            owner_address)){
    tron_send_error(ERROR_COMMON_ERROR_CORRUPT_DATA_TAG, 
                          ERROR_DATA_FLOW_DECODING_FAILED);
    return false;

  }
  // verify recipient address;
  if (!base58_encode_check(to_address,
                           1 + 20, 
                           HASHER_SHA2D, 
                           address,
                           TRON_ACCOUNT_ADDRESS_LENGTH + 1)) {
    tron_send_error(ERROR_COMMON_ERROR_UNKNOWN_ERROR_TAG, 2);
    return false;
  }

  if (!core_scroll_page(ui_text_verify_address, address, tron_send_error)) {
    return false;
  }

  // verify recipient amount
  uint8_t amount_hex[8];
  char amount_string[40] = {'\0'}, amount_decimal_string[30] = {'\0'};
  char display[100] ={'\0'};

  dec_to_hex(amount, amount_hex, 8);
  if(!convert_byte_array_to_decimal_string( 8,
                                            0,
                                            amount_string,
                                            amount_decimal_string,
                                            sizeof(amount_decimal_string))){
    tron_send_error(ERROR_COMMON_ERROR_UNKNOWN_ERROR_TAG, 1);
    return false;                                  
  }
  snprintf(display,
           sizeof(display),
           UI_TEXT_VERIFY_AMOUNT,
           amount_decimal_string,
           TRON_LUNIT);
  if (!core_confirmation(display, tron_send_error)) {
    return false;
  }

  set_app_flow_status(TRON_SIGN_TXN_STATUS_VERIFY);
  return true;
}

STATIC bool fetch_seed(tron_query_t *query, uint8_t *seed_out) {
  if (!tron_get_query(query, TRON_QUERY_SIGN_TXN_TAG) ||
      !check_which_request(query, TRON_SIGN_TXN_REQUEST_VERIFY_TAG)) {
    return false;
  }

  if (!reconstruct_seed(tron_txn_context->init_info.wallet_id,
                        seed_out,
                        tron_send_error)) {
    memzero(seed_out, sizeof(seed_out));
    return false;
  }
  set_app_flow_status(TRON_SIGN_TXN_STATUS_SEED_GENERATED);
  send_response(TRON_SIGN_TXN_RESPONSE_VERIFY_TAG);
  return true;
}

static bool send_signature() {
  
  return true;
}

/*****************************************************************************
 * GLOBAL FUNCTIONS
 *****************************************************************************/

void tron_sign_transaction(tron_query_t *query) {
  tron_txn_context =
      (tron_txn_context_t *)malloc(sizeof(tron_txn_context_t));
  memzero(tron_txn_context, sizeof(tron_txn_context_t));
  tron_sign_txn_signature_response_t sig = {0};
  uint8_t seed[64] = {0};

  if (tron_handle_initiate_query(query) &&
      tron_fetch_valid_transaction(query) && tron_get_user_verification() &&
      fetch_seed(query, seed) && send_signature(query, seed, &sig)) {
    delay_scr_init(ui_text_check_cysync, DELAY_TIME);
  }

  memzero(seed, sizeof(seed));

  if (NULL != tron_txn_context->transaction) {
    free(tron_txn_context->transaction);
  }
  if (NULL != tron_txn_context) {
    free(tron_txn_context);
    tron_txn_context = NULL;
  }
}