/**
 * @file    inheritance_priv.h
 * @author  Cypherock X1 Team
 * @brief   Support for inheritance app internal operations
 *          This file is defined to separate INHERITANCE's internal use
 * functions, flows, common APIs
 * @copyright Copyright (c) 2023 HODL TECH PTE LTD
 * <br/> You may obtain a copy of license at <a href="https://mitcc.org/"
 * target=_blank>https://mitcc.org/</a>
 */
#ifndef INHERITANCE_PRIV_H
#define INHERITANCE_PRIV_H
/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <inheritance/core.pb.h>
#include <stdint.h>

#include "inheritance_context.h"

/*****************************************************************************
 * TYPEDEFS
 *****************************************************************************/

typedef struct {
} inheritance_decryption_context_t;

/**
 * @brief Handler for inheritance message decryption
 * @details This flow expects INHERITANCE_QUERY_DECRYPT_TAG as initial query.
 * The function controls the complete data exchange with host, user prompts and
 * confirmations for decryption of inheritance data.
 *
 * @param query Reference to the decoded query struct from the host app
 */
void inheritance_decrypt_data(inheritance_query_t *query);
#endif /* INHERITANCE_PRIV_H */
