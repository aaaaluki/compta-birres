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
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "button.h"
#include "display_7segment.h"

#define DISPLAY_DIGITS (5U)

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
    unit        = 1;
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

/* Button samples */
void button_single(void *arg) {
  /* Initialize button */
  Button_t button = {
    .pin   = GPIO_NUM_34,
    .logic = BUTTON_LOGIC_ACTIVE_LOW,
    .config =
      {
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
      },
  };
  button_init(&button);

  /* Infinite loop */
  for (;;) {
    /* get ts */
    uint32_t ts = esp_timer_get_time();
    printf("[%lu] ", ts / 1000);

    /* read button state */
    bool state;
    button_read(&button, &state);
    printf("raw: %s; ", state ? "pressed" : "released");

    /* read button state with debouncing */
    button_state(&button, &state);
    printf("deb: %s; ", state ? "pressed" : "released");

    /* read button event */
    ButtonEvent_t event;
    button_event(&button, &event);
    switch (event) {
    case NO_PRESS: printf("No press\n"); break;
    case SINGLE_PRESS: printf("Single press\n"); break;
    case LONG_PRESS: printf("Long press\n"); break;
    case DOUBLE_PRESS: printf("Double press\n"); break;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

/* Counter samples */
void counter_button(void *arg) {
  /* Initialize counter */
  uint32_t counter     = 0;
  uint32_t old_counter = 0;

  /* Initialize button */
  Button_t button = {
    .pin   = GPIO_NUM_34,
    .logic = BUTTON_LOGIC_ACTIVE_LOW,
    .config =
      {
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
      },
  };
  button_init(&button);

  /* Infinite loop */
  for (;;) {
    /* read button event */
    ButtonEvent_t event;
    button_event(&button, &event);
    switch (event) {
    case NO_PRESS: break;
    case SINGLE_PRESS: counter += 1; break;
    case LONG_PRESS: counter = 0; break;
    case DOUBLE_PRESS: counter += 2; break;
    }

    if (counter != old_counter) {
      printf("Counter: %lu\n", counter);
      old_counter = counter;
    }

    vTaskDelay(pdMS_TO_TICKS(10)); // Yield for 10 ms
  }
}