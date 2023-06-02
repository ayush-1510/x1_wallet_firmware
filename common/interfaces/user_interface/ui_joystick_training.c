/**
 * @file    ui_joystick_training.c
 * @author  Cypherock X1 Team
 * @brief   UI component to train user for joystick.
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

#include "ui_joystick_training.h"

#include "ui_events_priv.h"
#ifdef DEV_BUILD
#include "dev_utils.h"
#endif

/*****************************************************************************
 * EXTERN VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * PRIVATE MACROS AND DEFINES
 *****************************************************************************/

/*****************************************************************************
 * PRIVATE TYPEDEFS
 *****************************************************************************/

/**
 * @brief struct to store message data
 * @details
 *
 * @see
 * @since v1.0.0
 *
 * @note Add constant
 */
struct Message_Data {
  char *message;
};

/**
 * @brief struct to store message lvgl object
 * @details
 *
 * @see
 * @since v1.0.0
 *
 * @note
 */
struct Message_Object {
  lv_obj_t *message;
};

/*****************************************************************************
 * STATIC FUNCTION PROTOTYPES
 *****************************************************************************/

/**
 * @brief Create message screen
 * @details
 *
 * @see
 * @since v1.0.0
 */
static void joystick_train_create();

/**
 * @brief Clear screen
 * @details
 *
 * @param
 *
 * @return
 * @retval
 *
 * @see
 * @since v1.0.0
 *
 * @note
 */
static void joystick_train_destructor();

/**
 * @brief Next button event handler.
 * @details
 *
 * @param next_btn Next button lvgl object.
 * @param event Type of event.
 *
 * @return
 * @retval
 *
 * @see
 * @since v1.0.0
 *
 * @note
 */
static void next_btn_event_handler(lv_obj_t *obj, const lv_event_t event);

/*****************************************************************************
 * STATIC VARIABLES
 *****************************************************************************/

static struct Message_Data *data = NULL;
static struct Message_Object *obj = NULL;
static joystick_actions_e action = JOYSTICK_ACTION_CENTER;

/*****************************************************************************
 * GLOBAL VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * STATIC FUNCTIONS
 *****************************************************************************/

static void joystick_train_destructor() {
  if (data != NULL) {
    memzero(data, sizeof(struct Message_Data));
    free(data);
    data = NULL;
  }
  if (obj != NULL) {
    free(obj);
    obj = NULL;
  }
}

static void next_btn_event_handler(lv_obj_t *obj, const lv_event_t event) {
  if (event == LV_EVENT_KEY && action == lv_indev_get_key(ui_get_indev())) {
    ui_set_confirm_event();
  } else if (event == LV_EVENT_DELETE) {
    /* Destruct object and data variables in case the object is being deleted
     * directly using lv_obj_clean() */
    joystick_train_destructor();
  }
}

static void joystick_train_create() {
  ASSERT(data != NULL);
  ASSERT(obj != NULL);

  obj->message = lv_label_create(lv_scr_act(), NULL);
  ui_paragraph(obj->message, data->message, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(obj->message, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_group_add_obj(ui_get_group(), obj->message);
  lv_obj_set_event_cb(obj->message, next_btn_event_handler);
}
/*****************************************************************************
 * GLOBAL FUNCTIONS
 *****************************************************************************/

void joystick_train_init(const char *message, joystick_actions_e act) {
  ASSERT(message != NULL);

  /* Clear screen before populating any data, this will clear any UI component
   * and it's corresponding objects. Important thing to note here is that the
   * screen will be updated only when lv_task_handler() is called.
   * This call will ensure that there is no object present in the currently
   * active screen in case data from previous screen was not cleared */
  lv_obj_clean(lv_scr_act());
  action = act;

  data = malloc(sizeof(struct Message_Data));
  obj = malloc(sizeof(struct Message_Object));

  if (data != NULL) {
    data->message = (char *)message;
  }
#ifdef DEV_BUILD
  ekp_enqueue(act, DEFAULT_DELAY);
#endif
  joystick_train_create();
}