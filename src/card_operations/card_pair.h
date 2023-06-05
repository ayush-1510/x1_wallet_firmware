/**
 * @file    card_pair.h
 * @author  Cypherock X1 Team
 * @brief   Pair card handler
 *
 * @copyright Copyright (c) 2023 HODL TECH PTE LTD
 * <br/> You may obtain a copy of license at <a href="https://mitcc.org/"
 * target=_blank>https://mitcc.org/</a>
 */
#ifndef CARD_PAIR_H
#define CARD_PAIR_H

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "card_operations.h"
#include "stdbool.h"
#include "stdint.h"

/*****************************************************************************
 * MACROS AND DEFINES
 *****************************************************************************/

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
 * @brief This function performs pairing operation for specified card number
 * If card tapped is not already paired, pairing keys are generated and are
 * saved to flash. This flow saves the family id of the card tapped after
 * pairing if family id on flash was empty.
 *
 * @param card_number card number expected [1, 4]
 * @param heading Heading for instruction screen to be displayed when an error
 * occurs
 * @param message Message for instruction screen to be displayed when an error
 * occurs
 * @return true if either already paired or pairing keys were saved to flash
 * else fasle
 */
bool pair_card_operation(uint8_t card_number, char *heading, char *message);
#endif