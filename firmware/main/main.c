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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sample_tasks.h"

static const char *TAG = "Compta_Birres";

void app_main(void) {
  // xTaskCreate(display_rotate_numbers, "display_rotate_numbers", 3 * 1024,
  // NULL, 10, NULL);
  // xTaskCreate(button_single, "button_single", 3 * 1024, NULL, 10, NULL);
  xTaskCreate(counter_button, "counter_button", 3 * 1024, NULL, 10, NULL);

  /* Infinite loop */
  for (;;) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
