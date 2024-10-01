/*
 * Created on Thu Sep 26 2024
 *
 * Copyright (C) 2024 Lluc Ricart
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "muxbuttons.h"

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

esp_err_t mux_buttons_init(MuxButton_t *mux_button) {

  esp_err_t ret;

  /* Initialize CD4051 */
  ret = iccd4051_init(&mux_button->ic);
  if (ret != ESP_OK) {
    return ret;
  }

  /* Initialize buttons */
  /* NOTE: Not the most efficient initing the same input pin 1+8 times, there
   * could be a better way? only initing internal structures (no GPIO) on the
   * buttons */
  for (uint8_t i = 0; i < MAX_MUX_BUTTONS; i++) {
    mux_button->_buttons[i].pin   = mux_button->ic.out;
    mux_button->_buttons[i].logic = mux_button->logic;

    ret = button_init(&mux_button->_buttons[i]);
    if (ret != ESP_OK) {
      return ret;
    }
  }

  return ESP_OK;
}

esp_err_t mux_button_event(MuxButton_t *mux_button, uint8_t button,
                           ButtonEvent_t *event) {

  /* Check for null */
  if (!mux_button || !event) {
    return ESP_ERR_INVALID_ARG;
  }

  /* Check for button index */
  if (button >= MAX_MUX_BUTTONS) {
    return ESP_ERR_INVALID_ARG;
  }

  /* Set the CD4051 channel */
  iccd4051_select(&mux_button->ic, button);

  /* Delay for the CD4051 to settle */
  vTaskDelay(pdMS_TO_TICKS(1));

  /* Get the button event */
  return button_event(&mux_button->_buttons[button], event);
}
