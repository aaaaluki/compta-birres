/*
 * Created on Thu Sep 26 2024
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

#include "iccd4051.h"

#include "esp_log.h"
#include <rom/ets_sys.h>

esp_err_t iccd4051_init(IC_CD4051_t *ic) {
  /* check struct is not NULL */
  if (!ic) {
    return ESP_FAIL;
  }

  /* zero-initialize the config structure */
  gpio_config_t in_io_conf  = {};
  gpio_config_t out_io_conf = {};
  esp_err_t ret;

  uint64_t mask = 0;
  mask |= (1ULL << ic->s0);
  mask |= (1ULL << ic->s1);
  mask |= (1ULL << ic->s2);

  /* configure output GPIOs */
  in_io_conf.mode         = GPIO_MODE_OUTPUT;
  in_io_conf.pin_bit_mask = mask;
  in_io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  in_io_conf.pull_up_en   = GPIO_PULLUP_DISABLE;

  ret = gpio_config(&in_io_conf);
  if (ret != ESP_OK) {
    return ret;
  }

  /* configure input GPIO */
  out_io_conf.pin_bit_mask = 1ULL << ic->out;
  out_io_conf.mode         = GPIO_MODE_INPUT;
  out_io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  out_io_conf.pull_up_en   = GPIO_PULLUP_DISABLE;
  out_io_conf.intr_type    = GPIO_INTR_DISABLE;

  ret = gpio_config(&out_io_conf);
  if (ret != ESP_OK) {
    return ret;
  }

  /* initialize pin level */
  gpio_set_level(ic->s0, 0);
  gpio_set_level(ic->s1, 0);
  gpio_set_level(ic->s2, 0);

  return ESP_OK;
}

esp_err_t iccd4051_select(IC_CD4051_t *ic, uint8_t channel) {
  /* check struct is not NULL */
  if (!ic) {
    return ESP_FAIL;
  }

  esp_err_t ret;

  /* set select lines */
  ret = gpio_set_level(ic->s0, (channel >> 0) & 0x01);
  if (ret != ESP_OK) {
    return ret;
  }

  ret = gpio_set_level(ic->s1, (channel >> 1) & 0x01);
  if (ret != ESP_OK) {
    return ret;
  }

  ret = gpio_set_level(ic->s2, (channel >> 2) & 0x01);
  if (ret != ESP_OK) {
    return ret;
  }

  return ESP_OK;
}

esp_err_t iccd4051_read(IC_CD4051_t *ic, bool *data) {
  /* check struct is not NULL */
  if (!ic) {
    return ESP_FAIL;
  }

  /* read data */
  *data = (bool)gpio_get_level(ic->out);

  return ESP_OK;
}

esp_err_t iccd4051_read_channel(IC_CD4051_t *ic, uint8_t channel, bool *data) {
  esp_err_t ret;

  /* select channel */
  ret = iccd4051_select(ic, channel);
  if (ret != ESP_OK) {
    return ret;
  }

  /* delay 5us for settling */
  /* according to the CD4051 datasheet the max delay time is around 800 ns */
  ets_delay_us(5);

  /* read data */
  ret = iccd4051_read(ic, data);
  if (ret != ESP_OK) {
    return ret;
  }

  return ESP_OK;
}
