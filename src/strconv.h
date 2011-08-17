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


#include <glib.h>

enum {
  STRCONV_ERROR_PARSE,
  STRCONV_ERROR_RANGE,
  STRCONV_ERROR_NEGATIVE
};


guint64 strtouint64 ( const gchar *s, GError **err );
guint strtouint ( const gchar *s, GError **err );
const gchar* uint64tostr ( guint64 long_value );
const guint8* hex_strtokey ( const gchar *hex_str, guint *lk, GError **err );
const gchar* hex_keytostr ( const guint8 *key , guint lk );
