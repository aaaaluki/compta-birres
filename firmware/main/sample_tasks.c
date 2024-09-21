/*
 * Created on Sat Sep 21 2024
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

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "display_7segment.h"

static const uint8_t DISPLAY_DIGITS = 5;

/* Display samples */
void display_rotate_numbers(void *arg) {
  /* Initialize display */
  Display7Segment_t display = {
    .digits = DISPLAY_DIGITS,
    .type   = DISPLAY_7SEGMENT_COMMON_CATHODE,
    .ic =
      {
        .oe_pin    = GPIO_NUM_21,
        .rclk_pin  = GPIO_NUM_26,
        .srclr_pin = GPIO_NUM_25,
        .srclk_pin = GPIO_NUM_22,
        .ser_pin   = GPIO_NUM_27,
      },
  };
  display_init(&display);

  /* start display with 0 */
  display_display(&display, 0);

  /* make each digit on the display show the same number on repeat */
  uint32_t counter = 0;
  uint32_t display_num, unit;
  for (;;) {
    display_num = 0;
    unit = 1;
    for (uint8_t d = 0; d < DISPLAY_DIGITS; d++) {
        display_num += counter * unit;
        unit *= 10;
    }

    printf("Displaying %0*lu...\n", DISPLAY_DIGITS, display_num);
    display_display(&display, display_num);
    
    /* advance counter */
    counter++;
    counter = counter % 10;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
