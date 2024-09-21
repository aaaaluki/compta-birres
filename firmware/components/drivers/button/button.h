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

#ifndef BUTTON_H_
#define BUTTON_H_

#include "driver/gpio.h"

#define DEBOUNCE_MICROS       (5U * 1000U)
#define DOUBLE_GAP_MICROS_MAX (0U * 1000U) /* disabling double press */
#define LONG_MICROS_MIN       (1000U * 1000U)

typedef enum {
  BUTTON_LOGIC_ACTIVE_HIGH,
  BUTTON_LOGIC_ACTIVE_LOW,
} ButtonLogic_t;

typedef enum { NO_PRESS, SINGLE_PRESS, LONG_PRESS, DOUBLE_PRESS } ButtonEvent_t;

typedef struct {
  gpio_num_t pin;
  ButtonLogic_t logic;
  bool state;
  bool double_pending;
  bool long_press_pending;
  bool down;
  uint32_t down_ts;
  uint32_t up_ts;
  int64_t state_ts;
  gpio_config_t config;
} Button_t;

/**
 * @brief Initialize button
 *
 * @param button Button
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t button_init(Button_t *button);

/**
 * @brief Configure button ISR
 *
 * @param button Button
 * @param isr_handler ISR handler
 * @param args ISR handler arguments
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t button_configure_isr(Button_t *button, gpio_isr_t isr_handler,
                               void *args);

/**
 * @brief Read raw button state
 *
 * @param button Button
 * @param state Button state
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t button_read(Button_t *button, bool *state);

/**
 * @brief Get button state with debouncing
 *
 * @param button Button
 * @param state Button state
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t button_state(Button_t *button, bool *state);

/**
 * @brief Get button event
 *
 * @param button Button
 * @param event Button event ButtonEvent_t
 * @see ButtonEvent_t
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t button_event(Button_t *button, ButtonEvent_t *event);

#endif /* BUTTON_H_ */
