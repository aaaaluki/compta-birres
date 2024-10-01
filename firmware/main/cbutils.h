/*
 * Created on Thu Oct 1 2024
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

#ifndef CBUTILS_H_
#define CBUTILS_H_

#include <stddef.h>

/**
 * @brief Set the date and time manually, no NTP server connection
 *
 */
void set_date_time_manually(void);

/**
 * @brief Generate a UUID and store it in the buffer, the buffer must be at
 * least 37 bytes long
 *
 * @param uuid Buffer to store the UUID
 */
void generate_uuid(char *uuid);

/**
 * @brief Generate a unique filename with the given prefix and suffix
 *
 * @param buffer Buffer to store the filename
 * @param prefix Prefix of the filename
 * @param suffix Suffix of the filename
 * @param size Size of the buffer
 */
void generate_unique_filename(char *buffer, char *prefix, char *suffix,
                              size_t size);
#endif /* CBUTILS_H_ */
