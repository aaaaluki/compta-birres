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

#ifndef DISPLAY_7SEGMENT_H_
#define DISPLAY_7SEGMENT_H_

#include <stddef.h>

#include "ic74hc595.h"

/* Digit/Character to display in CC mode */
#define DISPLAY_7SEGMENT_0  (0b01111110)
#define DISPLAY_7SEGMENT_1  (0b00110000)
#define DISPLAY_7SEGMENT_2  (0b01101101)
#define DISPLAY_7SEGMENT_3  (0b01111001)
#define DISPLAY_7SEGMENT_4  (0b00110011)
#define DISPLAY_7SEGMENT_5  (0b01011011)
#define DISPLAY_7SEGMENT_6  (0b01011111)
#define DISPLAY_7SEGMENT_7  (0b01110000)
#define DISPLAY_7SEGMENT_8  (0b01111111)
#define DISPLAY_7SEGMENT_9  (0b01111011)
#define DISPLAY_7SEGMENT_A  (0b01110111)
#define DISPLAY_7SEGMENT_B  (0b00011111)
#define DISPLAY_7SEGMENT_C  (0b01001110)
#define DISPLAY_7SEGMENT_D  (0b00111101)
#define DISPLAY_7SEGMENT_E  (0b01001111)
#define DISPLAY_7SEGMENT_F  (0b01000111)
#define DISPLAY_7SEGMENT_DP (0b10000000)

typedef enum {
  DISPLAY_7SEGMENT_COMMON_ANODE   = 0,
  DISPLAY_7SEGMENT_COMMON_CATHODE = 1,
} Display7SegmentType_t;

typedef struct {
  const uint8_t digits;
  Display7SegmentType_t type;
  IC_74HC595_t ic;
} Display7Segment_t;

/**
 * @brief Initialize 7-segment multi display
 *
 * @param display 7-segment display
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t display_init(Display7Segment_t *display);

/**
 * @brief Display number to 7-segment multi display
 *
 * @param display 7-segment display
 * @param number Number to display
 * @param dp Decimal point enable
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t display_display(Display7Segment_t *display, uint32_t number);

#endif /* DISPLAY_7SEGMENT_H_ */
