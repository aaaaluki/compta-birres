/*
 * Created on Sun Aug 03 2024
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

#ifndef IC74HC595_H_
#define IC74HC595_H_

#include "driver/gpio.h"

typedef struct {
  gpio_num_t oe_pin;
  gpio_num_t rclk_pin;
  gpio_num_t srclr_pin;
  gpio_num_t srclk_pin;
  gpio_num_t ser_pin;
} IC_74HC595_t;

/**
 * @brief Initialize 74HC595
 *
 * @param ic 74HC595 IC
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t ic74hc595_init(IC_74HC595_t *ic);

/**
 * @brief Write data to 74HC595
 *
 * @param ic 74HC595 IC
 * @param data Data to write
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t ic74hc595_write(IC_74HC595_t *ic, uint8_t data);

#endif /* IC74HC595_H_ */
