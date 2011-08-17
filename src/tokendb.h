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


#ifndef TOKENDB_H
#define TOKENDB_H

#include <glib.h>
#include "otp.h"


typedef struct {
  const gchar *file_name;
  GKeyFile *key_file;
  gboolean modified;
} TokenDB;


TokenDB* token_db_new ( GError **err );
void token_db_free ( TokenDB *self );
void token_db_save ( TokenDB *self, GError **err );
gboolean token_db_has_token ( TokenDB *self, const gchar *name );
void token_db_add_token ( TokenDB *self, Token *token );
void token_db_del_token ( TokenDB *self, const gchar *name );
Token* token_db_get_token ( TokenDB *self, const gchar *name, GError **err );
gchar** token_db_get_names ( TokenDB *self, gsize *length );

#endif
