/*
 * Created on Sun Aug 04 2024
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

#include "button.h"

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

esp_err_t button_init(Button_t *button) {
  /* check for null */
  if (!button) {
    return ESP_FAIL;
  }

  /* configure pin */
  gpio_config_t config;
  config.pin_bit_mask = 1ULL << button->pin;
  config.mode         = GPIO_MODE_INPUT;
  config.pull_up_en   = GPIO_PULLUP_DISABLE;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  config.intr_type    = GPIO_INTR_DISABLE;

  /* set start state as disabled */
  button->_state_ts = 0;
  button->_state = (button->logic == BUTTON_LOGIC_ACTIVE_HIGH) ? false : true;

  button->_double_pending     = false;
  button->_long_press_pending = false;
  button->_down               = false;
  button->_down_ts            = 0;
  button->_up_ts              = 0;

  return gpio_config(&config);
}

esp_err_t button_configure_isr(Button_t *button, gpio_isr_t isr_handler,
                               void *args) {
  /* check for null */
  if (!button) {
    return ESP_FAIL;
  }

  esp_err_t ret;

  /* install ISR */
  ret = gpio_install_isr_service(0);
  if (ret != ESP_OK) {
    return ret;
  }

  /* hook isr handler */
  ret = gpio_isr_handler_add(button->pin, isr_handler, args);
  return ret;
}

esp_err_t button_read(Button_t *button, bool *state) {
  /* check for null */
  if (!button || !state) {
    return ESP_FAIL;
  }

  /* read button state */
  if (button->logic == BUTTON_LOGIC_ACTIVE_HIGH) {
    *state = gpio_get_level(button->pin);
  } else {
    *state = !gpio_get_level(button->pin);
  }
  return ESP_OK;
}

esp_err_t button_state(Button_t *button, bool *state) {
  /* Implementation taken from https://stackoverflow.com/a/66824589 and modified
   * to work with ESP32 */

  /* check for null */
  if (!button || !state) {
    return ESP_FAIL;
  }

  /* get button state with debouncing */
  uint64_t now_ts = esp_timer_get_time();
  bool now_state;
  if (now_ts - button->_state_ts > DEBOUNCE_MICROS) {
    button_read(button, &now_state);
    if (button->_state != now_state) {
      button->_state    = !button->_state;
      button->_state_ts = now_ts;
    }
  }

  *state = button->_state;
  return ESP_OK;
}

esp_err_t button_event(Button_t *button, ButtonEvent_t *event) {
  /* Implementation taken from https://stackoverflow.com/a/66824589 and modified
   * to work with ESP32 */

  /* check for null */
  if (!button || !event) {
    return ESP_FAIL;
  }

  *event          = NO_PRESS;
  uint32_t now_ts = esp_timer_get_time();

  // If state changed...
  bool state;
  button_state(button, &state);
  if (button->_down != state) {
    button->_down = !button->_down;
    if (button->_down) {
      // Timestamp button-down
      button->_down_ts = now_ts;

    } else {
      // Timestamp button-up
      button->_up_ts = now_ts;

      if (button->_long_press_pending) {
        // If a long press was pending, no further action is needed (clear
        // flags)
        button->_long_press_pending = false;
        button->_double_pending     = false;

      } else if (button->_double_pending) {
        // Check for potential double press
        *event                  = DOUBLE_PRESS;
        button->_double_pending = false;

      } else {
        // If no long press or double press is detected, set double press
        // pending
        button->_double_pending = true;
      }

      // Cancel any long press pending on button release
      button->_long_press_pending = false;
    }
  }

  // If button-up and double-press gap time expired, it was a single press
  if (!button->_down && button->_double_pending &&
      (now_ts - button->_up_ts > DOUBLE_GAP_MICROS_MAX)) {
    button->_double_pending = false;
    *event                  = SINGLE_PRESS;

  } else if (button->_down && !button->_long_press_pending &&
             (now_ts - button->_down_ts > LONG_MICROS_MIN)) {
    // else check for long press
    *event                      = LONG_PRESS;
    button->_long_press_pending = true;
    button->_double_pending     = false;
  }

  return ESP_OK;
}