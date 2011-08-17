/********************************************************************************
 * This file is part of the OTP Generator for N900.                             *
 *                                                                              *
 * OTP is a Hildon program thats generates one-time passwords. It supports      *
 * OAUTH tokens (HOTP, TOTP) as weel as the Mobile-OTP algorithms.              *
 * Copyright (C) 2011  Sean Whitbeck <sean@neush.net>                           *
 *                                                                              *
 * This program is free software: you can redistribute it and/or modify         *
 * it under the terms of the GNU General Public License as published by         *
 * the Free Software Foundation, either version 3 of the License, or            *
 * (at your option) any later version.                                          *
 *                                                                              *
 * This program is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
 * GNU General Public License for more details.                                 *
 *                                                                              *
 * You should have received a copy of the GNU General Public License            *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.        *
 ********************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#define DIR_PUBLIC_MODE 0755
#define DIR_PRIVATE_MODE 0700
#define FILE_PRIVATE_MODE 0600

#define USER_CONFIG_DIR ".config"
#define TOKEN_DB_FILE_NAME "tokens"

#define DEFAULT_DISP_HEX TRUE
#define DEFAULT_DIGITS 6
#define DEFAULT_COUNTER 0
#define DEFAULT_PIN_SIZE 4
#define DEFAULT_HOTP_INTERVAL 30 // seconds
#define DEFAULT_MOTP_INTERVAL 10 // seconds

#define MIN_DIGITS 6
#define MAX_DIGITS 10

#define MIN_PIN_SIZE 4
#define MAX_PIN_SIZE 10

#define DEFAULT_HOTP_KEY_LENGTH 160  // bits
#define DEFAULT_MOTP_KEY_LENGTH 64   // bits

#endif
