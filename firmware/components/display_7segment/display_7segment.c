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

#include "display_7segment.h"

/* Digit to Segment converter */
const uint8_t DISPLAY_7SEGMENT_D2S[16] = {
  DISPLAY_7SEGMENT_0, DISPLAY_7SEGMENT_1, DISPLAY_7SEGMENT_2,
  DISPLAY_7SEGMENT_3, DISPLAY_7SEGMENT_4, DISPLAY_7SEGMENT_5,
  DISPLAY_7SEGMENT_6, DISPLAY_7SEGMENT_7, DISPLAY_7SEGMENT_8,
  DISPLAY_7SEGMENT_9, DISPLAY_7SEGMENT_A, DISPLAY_7SEGMENT_B,
  DISPLAY_7SEGMENT_C, DISPLAY_7SEGMENT_D, DISPLAY_7SEGMENT_E,
  DISPLAY_7SEGMENT_F,
};

esp_err_t display_init(Display7Segment_t *display) {
  /* check struct is not null */
  if (display == NULL) {
    return ESP_FAIL;
  }

  return ic74hc595_init(&display->ic);
}

esp_err_t display_display(Display7Segment_t *display, uint32_t number) {
  /* check struct is not null */
  if (display == NULL) {
    return ESP_FAIL;
  }

  /* write numbers */
  esp_err_t err = ESP_OK;
  for (int i = 0; i < display->digits; i++) {
    uint8_t digit = number % 10;
    number /= 10;
    uint8_t data = DISPLAY_7SEGMENT_D2S[digit];

    if (display->type == DISPLAY_7SEGMENT_COMMON_ANODE) {
      data = ~data;
    }

    /* TODO: Since the data is latched for every digit if the serial clock
     * is slow enough it would appear as if the new number was sliding into
     * place and the old one out (could be nice for something if provoked?¿, ie
     * feature not bug) */
    err = ic74hc595_write(&display->ic, data);

    if (err != ESP_OK) {
      return err;
    }
  }

  return err;
}
