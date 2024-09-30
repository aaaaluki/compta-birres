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

#ifndef ICCD4051_H_
#define ICCD4051_H_

#include "driver/gpio.h"

typedef struct {
  gpio_num_t s0;
  gpio_num_t s1;
  gpio_num_t s2;
  gpio_num_t out;
} IC_CD4051_t;

/**
 * @brief Initialize CD4051
 *
 * @param ic CD4051 IC
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t iccd4051_init(IC_CD4051_t *ic);

/**
 * @brief Select channel on CD4051
 *
 * @param ic CD4051 IC
 * @param channel Channel to select
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t iccd4051_select(IC_CD4051_t *ic, uint8_t channel);

/**
 * @brief Read data from CD4051
 *
 * @param ic CD4051 IC
 * @param data Data to read
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t iccd4051_read(IC_CD4051_t *ic, bool *data);

/**
 * @brief Read data from given channel
 *
 * @param ic CD4051 IC
 * @param channel Channel to read
 * @param data Data to read
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t iccd4051_read_channel(IC_CD4051_t *ic, uint8_t channel, bool *data);

#endif /* ICCD4051_H_ */