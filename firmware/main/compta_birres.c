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

#include "compta_birres.h"

#include "cbutils.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <time.h>
#include <unistd.h>

static const char *TAG = "cb_master";

static ComptaBirres_t compta_birres;

static const char *uuids_file_name   = "/sdcard/uuids";
static const char *counter_file_name = "/sdcard/cs16";
static const char *logbase_file_name = "/sdcard/cb";

esp_err_t cb_init(void) {
  esp_err_t ret = ESP_OK;

  /* Fill compta_birres struct with the pins */
  compta_birres.display.digits       = CONFIG_CB_DISPLAY_DIGITS;
  compta_birres.display.type         = CONFIG_CB_DISPLAY_COMMON_NODE;
  compta_birres.display.ic.oe_pin    = CONFIG_CB_DISPLAY_PIN_OE;
  compta_birres.display.ic.rclk_pin  = CONFIG_CB_DISPLAY_PIN_RCLK;
  compta_birres.display.ic.srclr_pin = CONFIG_CB_DISPLAY_PIN_SRCLR;
  compta_birres.display.ic.srclk_pin = CONFIG_CB_DISPLAY_PIN_SRCLK;
  compta_birres.display.ic.ser_pin   = CONFIG_CB_DISPLAY_PIN_SER;

  compta_birres.mux_button.ic.s0  = CONFIG_CB_BUTTONS_PIN_SEL0;
  compta_birres.mux_button.ic.s1  = CONFIG_CB_BUTTONS_PIN_SEL1;
  compta_birres.mux_button.ic.s2  = CONFIG_CB_BUTTONS_PIN_SEL2;
  compta_birres.mux_button.ic.out = CONFIG_CB_BUTTONS_PIN_BUT;
  compta_birres.mux_button.logic  = CONFIG_CB_BUTTONS_PIN_BUT_LOGIC;

  compta_birres.sdcard.mosi = CONFIG_CB_SDCARD_PIN_MOSI;
  compta_birres.sdcard.miso = CONFIG_CB_SDCARD_PIN_MISO;
  compta_birres.sdcard.clk  = CONFIG_CB_SDCARD_PIN_CLK;
  compta_birres.sdcard.cs   = CONFIG_CB_SDCARD_PIN_CS;

  compta_birres._counter = 0;

  /* Initialize each submodule and exit on error */
  ESP_LOGI(TAG, "Initializing display...");
  ret = display_init(&compta_birres.display);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize display\n");
    return ret;
  }

  ESP_LOGI(TAG, "Initializing mux buttons...");
  ret = mux_buttons_init(&compta_birres.mux_button);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize mux button\n");
    return ret;
  }

  ESP_LOGI(TAG, "Initializing uSD card...");
  ret = sdcard_init(&compta_birres.sdcard);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize SD card\n");
    return ret;
  }

  /* read the counter file if exists */
  FILE *counter_file = fopen(counter_file_name, "r");
  if (counter_file != NULL) {
    fscanf(counter_file, "%lu", &compta_birres._counter);
    fclose(counter_file);
  }

  /* Create uuids file if it does not exist */
  if (access(uuids_file_name, F_OK) != 0) {
    ESP_LOGI(TAG, "Creating new uuid file");
    FILE *uuids_file = fopen(uuids_file_name, "w");
    if (uuids_file == NULL) {
      ESP_LOGE(TAG, "Failed to create uuid file");
      return ESP_FAIL;
    }
    fclose(uuids_file);
  }

  /* generate new uuid for this boot */
  char uuid[37];
  generate_uuid(uuid);

  FILE *uuids_file = fopen(uuids_file_name, "a");
  fprintf(uuids_file, "%s\n", uuid);
  fclose(uuids_file);

  /* Create new log file with the generated uuid */
  snprintf(compta_birres.logfile, sizeof(compta_birres.logfile), "%s%05lu",
           logbase_file_name, compta_birres._counter);
  ESP_LOGI(TAG, "Log file: %s\n", compta_birres.logfile);
  if (access(compta_birres.logfile, F_OK) != 0) {
    ESP_LOGI(TAG, "Log file does not exists");
    FILE *fp = fopen(compta_birres.logfile, "w");
    if (fp == NULL) {
      ESP_LOGE(TAG, "Failed to create log file");
      return ESP_FAIL;
    }
    fclose(fp);
  }

  /* list files on mounted directory */
  ESP_LOGI(TAG, "Listing files on /sdcard");
  sdcard_list_files(&compta_birres.sdcard, "/sdcard");

  /* HACK: Set the date and time manually, since there is no developing time for
   * coding the connection to an NTP server, we hardcode the event date */
  set_date_time_manually();

  return ESP_OK;
}

void cb_master(void *pvParameters) {
  uint32_t old_counter = 0;
  ButtonEvent_t event;
  LogBirra_t log_birra;

  QueueHandle_t log_queue = (QueueHandle_t)pvParameters;

  ESP_LOGI(TAG, "Master task started");

  /* Initialize all the submodules */
  cb_init();

  /* endless loop of alcoholism*/
  for (;;) {
    /* read all the buttons for event */
    for (uint8_t channel = 0; channel < MAX_MUX_BUTTONS; channel++) {
      mux_button_event(&compta_birres.mux_button, channel, &event);
      switch (event) {
      case NO_PRESS: break;
      case SINGLE_PRESS: compta_birres._counter++; break;
      case LONG_PRESS: compta_birres._counter++; break;
      case DOUBLE_PRESS: compta_birres._counter += 2; break;
      }

      /* Display the counter if it has changed */
      if (compta_birres._counter != old_counter) {
        ESP_LOGI(TAG, "Counter: %lu", compta_birres._counter);
        display_display(&compta_birres.display, compta_birres._counter);
        old_counter = compta_birres._counter;

        /* Send the counter data to the logger */
        log_birra.channel   = channel;
        log_birra.counter   = compta_birres._counter;
        log_birra.timestamp = time(NULL);
        xQueueSend(log_queue, &log_birra, 0);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10)); // Yield for 10 ms
  }
}

void cb_logger(void *pvParameters) {
  QueueHandle_t log_queue = (QueueHandle_t)pvParameters;
  LogBirra_t log_rcv;

  char timestamp[72];
  struct tm tm;

  ESP_LOGI(TAG, "Logger task started");
  for (;;) {
    /* Wait for a message from the master poller */
    if (xQueueReceive(log_queue, &log_rcv, portMAX_DELAY) != pdTRUE) {
      continue;
    }

    ESP_LOGI(TAG, "Logging counter: %lu", log_rcv.counter);

    /* get the time information form the press timestamp */
    localtime_r(&log_rcv.timestamp, &tm);

    /* Format the timestamp as ISO8601 */
    snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02dT%02d:%02d:%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
             tm.tm_min, tm.tm_sec);

    /* Log the press to the logfile */
    FILE *log_file = fopen(compta_birres.logfile, "a");
    if (log_file == NULL) {
      ESP_LOGE(TAG, "Failed to open log file");
      continue;
    }
    fprintf(log_file, "%s,%u,%lu\n", timestamp, log_rcv.channel,
            log_rcv.counter);
    fclose(log_file);

    /* update the counter file */
    FILE *counter_file = fopen(counter_file_name, "w");
    if (counter_file == NULL) {
      ESP_LOGE(TAG, "Failed to open counter file");
      continue;
    }
    fprintf(counter_file, "%lu", compta_birres._counter);
    fclose(counter_file);

    vTaskDelay(pdMS_TO_TICKS(10)); // Yield for 10 ms
  }
}