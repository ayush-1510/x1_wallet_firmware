/**
 * @file    inheritance_recovery.c
 * @author  Cypherock X1 Team
 * @brief
 * @details

 * @copyright Copyright (c) 2023 HODL TECH PTE LTD
 * <br/> You may obtain a copy of license at <a href="https://mitcc.org/"
 * target=_blank>https://mitcc.org/</a>
 *
 ******************************************************************************
 * @attention
 *
 * (c) Copyright 2022 by HODL TECH PTE LTD
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
#include "inheritance_main.h"
#include "ui_core_confirm.h"
#include "ui_screens.h"

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
 * STATIC VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * GLOBAL VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * STATIC FUNCTION PROTOTYPES
 *****************************************************************************/

/*****************************************************************************
 * STATIC FUNCTIONS
 *****************************************************************************/

/*****************************************************************************
 * GLOBAL FUNCTIONS
 *****************************************************************************/

void inheritance_recovery(inheritance_query_t *query,
                          inheritance_result_t *response) {
  SecureData msgs[SESSION_MSG_MAX] = {0};
  uint32_t msg_count = 0;

  // TODO: remove after testing
  set_session();

  uint8_t packet[SESSION_PACKET_SIZE] = {0};
  size_t packet_size = query->recovery.encrypted_data.packet.size;
  memcpy(packet, &query->recovery.encrypted_data.packet.bytes, packet_size);

  if (!session_decrypt_packet(msgs, &msg_count, packet, &packet_size)) {
    LOG_CRITICAL("xxec %d", __LINE__);
    return;
  }

  if (!session_decrypt_secure_data(
          query->wallet_auth.initiate.wallet_id, msgs, msg_count)) {
    LOG_CRITICAL("xxec %d", __LINE__);
    return;
  }

  response->which_response = INHERITANCE_RESULT_RECOVERY_TAG;
  response->recovery.plain_data_count = msg_count;

  convert_msg_to_plaindata(response->recovery.plain_data, msgs, msg_count);

#if USE_SIMULATOR == 1
  printf("Inheritance Recovery Result: <Plain Data>\n");
  for (int j = 0; j < response->recovery.plain_data_count; j++) {
    printf("\nMessage #%d:\n", j);
    printf("%x", response->recovery.plain_data[j].message);
  }
  printf("\nEnd");
#else
  inheritance_send_result(&response);
#endif
}

void convert_msg_to_plaindata(inheritance_plain_data_t *plain_data,
                              SecureData *msgs,
                              uint8_t msg_count) {
  for (uint8_t i = 0; i < (msg_count); i++) {
    if (1 == msgs[i].plain_data[0])
      plain_data[i].is_private = true;

    memcpy(plain_data[i].message.bytes,
           msgs[i].plain_data + 1,
           msgs[i].plain_data_size - 1);
    plain_data[i].message.size = msgs[i].plain_data_size - 1;
  }
}