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

#include <stdio.h>

#include "esp_system.h"
#include "freertos/FreeRTOS.h"

#include "display_7segment.h"

void app_main(void) {
  Display7Segment_t display = {
    .digits = 5,
    .type   = DISPLAY_7SEGMENT_COMMON_ANODE,
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

  for (int i = 0; i < 10; i++) {
    printf("Displaying %d...\n", i);
    display_display(&display, i);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  for (int i = 0; i < 3; i++) {
    printf("Restarting in %d s...\n", i);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  esp_restart();
}
