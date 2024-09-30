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

#ifndef MUXBUTTONS_H_
#define MUXBUTTONS_H_

#include "button.h"
#include "iccd4051.h"

#define MAX_MUX_BUTTONS (8U)

typedef struct {
  IC_CD4051_t ic;
  ButtonLogic_t logic;
  Button_t _buttons[MAX_MUX_BUTTONS];
} MuxButton_t;

/**
 * @brief Initialize the MuxButton_t struct
 *
 * @param mux_button Pointer to the MuxButton_t struct
 * @param pin GPIO pin of the mux button
 * @param logic Logic of the mux button
 * @return esp_err_t
 */
esp_err_t mux_buttons_init(MuxButton_t *mux_button);

/**
 * @brief Get the event of the specified button
 *
 * @param mux_button Pointer to the MuxButton_t struct
 * @param button Button index
 * @param event Resulting event
 * @return esp_err_t
 */
esp_err_t mux_button_event(MuxButton_t *mux_button, uint8_t button,
                           ButtonEvent_t *event);

#endif /* MUXBUTTONS_H_ */