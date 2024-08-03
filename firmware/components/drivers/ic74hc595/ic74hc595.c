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

#include "ic74hc595.h"

esp_err_t ic74hc595_init(IC_74HC595_t *ic) {
  /* check struct is not NULL */
  if (!ic) {
    return ESP_FAIL;
  }

  /* zero-initialize the config structure */
  gpio_config_t io_conf = {};

  uint64_t mask = 0;
  mask |= (1 << ic->oe_pin);
  mask |= (1 << ic->rclk_pin);
  mask |= (1 << ic->srclr_pin);
  mask |= (1 << ic->srclk_pin);
  mask |= (1 << ic->ser_pin);

  /* configure GPIO */
  io_conf.intr_type    = GPIO_INTR_DISABLE;
  io_conf.mode         = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = mask;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en   = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);

  /* initialize pin level */
  gpio_set_level(ic->oe_pin, 0);
  gpio_set_level(ic->srclr_pin, 1);

  return ESP_OK;
}

esp_err_t ic74hc595_write(IC_74HC595_t *ic, uint8_t data) {
  /* check struct is not NULL */
  if (!ic) {
    return ESP_FAIL;
  }

  /* HACK: we are not taking into account the timing restrictions from the IC,
   * we are just gonna suppose that the uC will be "slow" enough to "wait" for
   * the setup and hold times */

  /* write data */
  for (int i = 0; i < 8; i++) {
    gpio_set_level(ic->ser_pin, (data >> i) & 0x01);
    gpio_set_level(ic->srclk_pin, 1);
    gpio_set_level(ic->srclk_pin, 0);
  }

  /* latch data */
  gpio_set_level(ic->rclk_pin, 1);
  gpio_set_level(ic->rclk_pin, 0);

  return ESP_OK;
}
