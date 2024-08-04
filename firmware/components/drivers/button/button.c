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

esp_err_t button_init(Button_t *button) {
  /* check for null */
  if (!button) {
    return ESP_FAIL;
  }

  /* configure pin */
  button->config.pin_bit_mask = 1ULL << button->pin;

  return gpio_config(&button->config);
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
