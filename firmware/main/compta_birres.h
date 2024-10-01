/*
 * Created on Thu Sep 30 2024
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

#ifndef COMPTA_BIRRES_H_
#define COMPTA_BIRRES_H_

#include "time.h"

#include "display_7segment.h"
#include "muxbuttons.h"
#include "sdcard.h"

typedef struct {
  uint32_t _counter;
  char logfile[SDCARD_MAX_LINE_LENGTH];
  Display7Segment_t display;
  MuxButton_t mux_button;
  SDCard_t sdcard;
} ComptaBirres_t;

typedef struct {
  uint8_t channel;
  uint32_t counter;
  time_t timestamp;
} LogBirra_t;

/**
 * @brief Initialize Compta Birres and all of its components
 *
 * @return ESP_OK on success, ESP_FAIL otherwise
 */
esp_err_t cb_init(void);

/**
 * @brief Master task, responsible for polling the buttons and updating the
 * display, all the logging is passed to the logger task through a queue
 *
 * @param pvParameters Task parameters
 * @return esp_err_t
 */
void cb_master(void *pvParameters);

/**
 * @brief Logger task, responsible for logging the events to the SD card
 *
 * @param pvParameters Task parameters
 * @return esp_err_t
 */
void cb_logger(void *pvParameters);

#endif /* COMPTA_BIRRES_H_ */
