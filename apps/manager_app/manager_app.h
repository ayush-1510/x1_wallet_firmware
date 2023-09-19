/**
 * @file    manager_app.h
 * @author  Cypherock X1 Team
 * @brief
 * @details

 * @copyright Copyright (c) 2023 HODL TECH PTE LTD
 * <br/> You may obtain a copy of license at <a href="https://mitcc.org/"
 * target=_blank>https://mitcc.org/</a>
 *
 */

#ifndef MANAGER_APP_H
#define MANAGER_APP_H

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <manager/core.pb.h>
#include <manager/get_device_info.pb.h>
#include <stddef.h>
#include <stdint.h>

#include "events.h"

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
 * @brief Entry point for the manager application of the X1 vault. It is invoked
 * by the X1 vault firmware, as soon as there is a USB request raised for the
 * manager app.
 *
 * @param usb_evt The USB event which triggered invocation of the manager app
 */
void manager_app_main(usb_event_t usb_evt);

/**
 * @brief Restricted Entry point for the manager application of the X1 vault. It
 * is invoked by the X1 vault firmware, as soon as there is a USB request raised
 * for the manager app.
 * @note It only allows some functionality such as get device info, device
 * authentication and firmware update
 *
 * @param usb_evt The USB event which triggered invocation of the manager app
 */
void manager_app_restricted_main(usb_event_t usb_evt);

#endif /* MANAGER_APP_H */
