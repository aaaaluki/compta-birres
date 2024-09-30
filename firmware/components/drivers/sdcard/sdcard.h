/*
 * Created on Sun Sep 22 2024
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

#ifndef SDCARD_H_
#define SDCARD_H_

#include "driver/gpio.h"

#define SDCARD_MOUNT_POINT "/sdcard"

#define SDCARD_MAX_LINE_LENGTH (256U)

typedef struct {
  gpio_num_t mosi;
  gpio_num_t miso;
  gpio_num_t clk;
  gpio_num_t cs;
} SDCard_t;

/**
 * @brief Initialize the SD card
 *
 * @param sdcard Pointer to the SDCard_t struct
 *
 * @return ESP_OK on success, ESP_FAIL otherwise
 */
esp_err_t sdcard_init(SDCard_t *sdcard);

/**
 * @brief List files in the SD card
 *
 * @param sdcard Pointer to the SDCard_t struct
 * @param path Path to the directory
 *
 * @return ESP_OK on success, ESP_FAIL otherwise
 */
esp_err_t sdcard_list_files(SDCard_t *sdcard, const char *path);

#endif /* SDCARD_H_ */
