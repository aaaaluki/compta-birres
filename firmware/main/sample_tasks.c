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

#include "sample_tasks.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "button.h"
#include "display_7segment.h"
#include "iccd4051.h"
#include "sdcard.h"

#define DISPLAY_DIGITS (5U)

const char *TAG = "CB_sample_tasks";

/* Display samples */
void display_rotate_numbers(void *arg) {
  /* Initialize display */
  Display7Segment_t display = {
    .digits = CONFIG_CB_DISPLAY_DIGITS,
    .type   = CONFIG_CB_DISPLAY_COMMON_NODE,
    .ic =
      {
        .oe_pin    = CONFIG_CB_DISPLAY_PIN_OE,
        .rclk_pin  = CONFIG_CB_DISPLAY_PIN_RCLK,
        .srclr_pin = CONFIG_CB_DISPLAY_PIN_SRCLR,
        .srclk_pin = CONFIG_CB_DISPLAY_PIN_SRCLK,
        .ser_pin   = CONFIG_CB_DISPLAY_PIN_SER,
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

/* IC samples */
void iccd4051_rotation(void *arg) {
  const uint8_t channels         = 8;
  const uint8_t channels2read[3] = {0, 1, 2};

  /* Initialize CD4051 */
  IC_CD4051_t ic = {
    .s0  = CONFIG_CB_BUTTONS_PIN_SEL0,
    .s1  = CONFIG_CB_BUTTONS_PIN_SEL1,
    .s2  = CONFIG_CB_BUTTONS_PIN_SEL2,
    .out = CONFIG_CB_BUTTONS_PIN_BUT,
  };
  iccd4051_init(&ic);

  /* Infinite loop */
  for (;;) {
    for (uint8_t i = 0; i < channels; i++) {
      bool data;
      iccd4051_read_channel(&ic, i, &data);
      printf("Channel %u: %s", i, data ? "HIGH" : "LOW");
      printf(" ; raw: %s\n", gpio_get_level(ic.out) ? "HIGH" : "LOW");
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
}

void iccd4051_fixed(void *arg) {
  /* Initialize CD4051 */
  IC_CD4051_t ic = {
    .s0  = CONFIG_CB_BUTTONS_PIN_SEL0,
    .s1  = CONFIG_CB_BUTTONS_PIN_SEL1,
    .s2  = CONFIG_CB_BUTTONS_PIN_SEL2,
    .out = CONFIG_CB_BUTTONS_PIN_BUT,
  };
  iccd4051_init(&ic);

  /* Select channel 0 */
  iccd4051_select(&ic, 0);

  /* Infinite loop */
  for (;;) {
    bool data;
    iccd4051_read(&ic, &data);
    printf("Data: %s\n", data ? "HIGH" : "LOW");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

/* Button samples */
void button_single(void *arg) {
  /* Initialize button */
  Button_t button = {
    .pin   = CONFIG_CB_BUTTONS_PIN_BUT,
    .logic = CONFIG_CB_BUTTONS_PIN_BUT_LOGIC,
  };
  button_init(&button);

  /* Infinite loop */
  ButtonEvent_t event;
  for (;;) {
    /* get ts */
    uint32_t ts = esp_timer_get_time();

    /* read button event */
    button_event(&button, &event);
    switch (event) {
    case NO_PRESS: break;
    case SINGLE_PRESS: printf("[%lu] Single press\n", ts / 1000); break;
    case LONG_PRESS: printf("[%lu] Long press\n", ts / 1000); break;
    case DOUBLE_PRESS: printf("[%lu] Double press\n", ts / 1000); break;
    }

    vTaskDelay(pdMS_TO_TICKS(10)); // Yield for 10 ms
  }
}

/* Counter samples */
void counter_button(void *arg) {
  /* Initialize counter */
  uint32_t counter     = 0;
  uint32_t old_counter = 0;

  /* Initialize button */
  Button_t button = {
    .pin   = CONFIG_CB_BUTTONS_PIN_BUT,
    .logic = CONFIG_CB_BUTTONS_PIN_BUT_LOGIC,
  };
  button_init(&button);

  /* Infinite loop */
  printf("Counter: %lu\n", counter);
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

/* SD card samples */
void sdcard_example(void *arg) {
  esp_err_t ret;
  SDCard_t sdcard = {
    .mosi = CONFIG_CB_SDCARD_PIN_MOSI,
    .miso = CONFIG_CB_SDCARD_PIN_MISO,
    .clk  = CONFIG_CB_SDCARD_PIN_CLK,
    .cs   = CONFIG_CB_SDCARD_PIN_CS,
  };

  /* Initialize SD card */
  ret = sdcard_init(&sdcard);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize SD card\n");
  }

  /* List files in the SD card */
  ret = sdcard_list_files(&sdcard, "");
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to list files in SD card\n");
  }

  /* Infinite loop */
  for (;;) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
