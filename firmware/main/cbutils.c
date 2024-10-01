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

#include "cbutils.h"

#include "esp_random.h"
#include "esp_system.h"

#include <sys/time.h>
#include <time.h>

void set_date_time_manually(void) {
  struct tm timeinfo = {0};
  timeinfo.tm_year   = 2024 - 1900; // Year since 1900
  timeinfo.tm_mon    = 10 - 1;      // Month, 0-11
  timeinfo.tm_mday   = 2;           // Day of the month
  timeinfo.tm_hour   = 11;          // Hour (0-23)
  timeinfo.tm_min    = 0;           // Minutes (0-59)
  timeinfo.tm_sec    = 0;           // Seconds (0-59)

  // Convert struct tm to time_t
  time_t now = mktime(&timeinfo);

  // Set the system time
  struct timeval tv;
  tv.tv_sec  = now; // seconds
  tv.tv_usec = 0;   // microseconds
  settimeofday(&tv, NULL);

  printf("Date and time set to: %s", asctime(&timeinfo));
}

void generate_uuid(char *uuid) {
  const char *characters = "89ab"; // Variant bits
  const char *hex_chars  = "0123456789abcdef";

  for (int i = 0; i < 36; i++) {
    if (i == 8 || i == 13 || i == 18 || i == 23) {
      uuid[i] = '-'; // Insert hyphens
    } else if (i == 14) {
      uuid[i] = '4'; // Version 4
    } else if (i == 19) {
      uuid[i] = characters[esp_random() % 4]; // Variant bits
    } else {
      uuid[i] = hex_chars[esp_random() % 16]; // Random hex digit
    }
  }
  uuid[36] = '\0'; // Null-terminate the string
}

void generate_unique_filename(char *buffer, char *prefix, char *suffix,
                              size_t size) {
  // Create a unique filename using a random number
  uint32_t random_number = esp_random();
  snprintf(buffer, size, "%s%08lx%s", prefix, random_number, suffix);
}