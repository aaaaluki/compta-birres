/*
 * Created on Sun Sep 22 2024
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

#include "sdcard.h"

#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

static const char *TAG = "CB_mod_SDCard";

esp_err_t sdcard_init(SDCard_t *sdcard) {
  esp_err_t ret;

  /* check for NULL */
  if (sdcard == NULL) {
    ESP_LOGE(TAG, "sdcard is NULL");
    return ESP_ERR_INVALID_ARG;
  }

  /* Options for mounting the filesystem */
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    .format_if_mount_failed = false,
    .max_files              = 4,
  };

  sdmmc_card_t *card;
  const char mount_point[] = SDCARD_MOUNT_POINT;
  ESP_LOGI(TAG, "Initializing SD card");

  // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
  // For setting a specific frequency, use host.max_freq_khz (range 400kHz -
  // 20MHz for SDSPI) Example: for fixed frequency of 10MHz, use
  // host.max_freq_khz = 10000;
  sdmmc_host_t host = SDSPI_HOST_DEFAULT();

  spi_bus_config_t bus_cfg = {
    .mosi_io_num     = sdcard->mosi,
    .miso_io_num     = sdcard->miso,
    .sclk_io_num     = sdcard->clk,
    .quadwp_io_num   = GPIO_NUM_NC,
    .quadhd_io_num   = GPIO_NUM_NC,
    .max_transfer_sz = 4000,
  };

  ESP_LOGI(TAG, "Initializing SPI bus");
  ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CH_AUTO);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize bus.");
    return ret;
  }

  // This initializes the slot without card detect (CD) and write protect (WP)
  // signals.
  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot_config.gpio_cs               = sdcard->cs;
  slot_config.host_id               = host.slot;
  // sdspi_host_init_device(&slot_config, &out_handle);

  ESP_LOGI(TAG, "Mounting filesystem");
  ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config,
                                &card);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(TAG, "Failed to mount filesystem. "
                    "If you want the card to be formatted, set the "
                    "CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
    } else {
      ESP_LOGE(TAG,
               "Failed to initialize the card (%s). "
               "Make sure SD card lines have pull-up resistors in place.",
               esp_err_to_name(ret));
    }
    return ret;
  }
  ESP_LOGI(TAG, "Filesystem mounted");

  // Card has been initialized, print its properties
  sdmmc_card_print_info(stdout, card);

  return ESP_OK;
}

esp_err_t sdcard_list_files(SDCard_t *sdcard, const char *path) {
  struct dirent *de;

  ESP_LOGI(TAG, "Listing files in %s", path);
  DIR *dr = opendir(path);

  if (dr == NULL) {
    ESP_LOGE(TAG, "Could not open current directory");
    return ESP_ERR_NOT_FOUND;
  }

  while ((de = readdir(dr)) != NULL) {
    ESP_LOGI(TAG, "%s", de->d_name);
  }

  closedir(dr);
  return ESP_OK;
}

esp_err_t sdcard_cat_file(SDCard_t *sdcard, const char *path) {
  FILE *fp = fopen(path, "r");
  if (fp == NULL) {
    ESP_LOGE(TAG, "Failed to open file for reading");
    return ESP_FAIL;
  }

  // Read and display the file contents
  char buffer[256];
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    printf("%s", buffer);
  }
  fclose(fp);

  return ESP_OK;
}