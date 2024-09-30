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

#ifndef SAMPLE_TASKS_H_
#define SAMPLE_TASKS_H_

/* Display samples */
void display_rotate_numbers(void *arg);

/* IC samples */
void iccd4051_rotation(void *arg);
void iccd4051_fixed(void *arg);

/* Button samples */
void button_single(void *arg);

/* Counter samples */
void counter_button(void *arg);

/* SD card samples */
void sdcard_example(void *arg);

#endif /* SAMPLE_TASKS_H_ */
