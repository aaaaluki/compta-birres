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
#include "freertos/queue.h"
#include "freertos/task.h"

#include "button.h"
#include "display_7segment.h"

static const char *TAG = "Compta_Birres";

static QueueHandle_t counter_queue = NULL;

static void IRAM_ATTR counter_isr_handler(void *arg) {
  static uint32_t counter = 1;
  xQueueSendFromISR(counter_queue, &counter, NULL);
  counter++;
}

static void display_example(void *arg) {
  Display7Segment_t *display = (Display7Segment_t *)arg;

  /* start display with 0 */
  display_display(display, 0);

  /* wait for a button counter press */
  uint32_t counter = 0;
  for (;;) {
    if (xQueueReceive(counter_queue, &counter, portMAX_DELAY) != pdTRUE) {
      vTaskDelay(100 / portTICK_PERIOD_MS);
      continue;
    }
    printf("Displaying %lu...\n", counter);
    display_display(display, counter);
  }
}

void app_main(void) {
  /* Initialize display */
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

  /* Initialize and configure button */
  Button_t button = {
    .pin   = 34,
    .logic = BUTTON_LOGIC_ACTIVE_LOW,
    .config =
      {
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_NEGEDGE,
      },
  };
  button_init(&button);

  counter_queue = xQueueCreate(8, sizeof(uint32_t));

  button_configure_isr(&button, counter_isr_handler, NULL);

  xTaskCreate(display_example, "display_example", 3 * 1024, (void *)&display,
              10, NULL);

  /* Infinite loop */
  for (;;) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
