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


#include "otp.h"

/*** MOTP test vectors  ***
 *
 * Based on runs with the otptool that comes with the mod_authn_otp apache module
 */

int
main ()
{
  gboolean ok;
  guint i;
  Token *token;
  const gchar *otp;

  static const gchar *key = "0123456789abcdef";

  static const guint64 epochs[] = {
    129612120,
    129612130,
    0,
    4,
    129612244,
    129612253
  };

  static const gchar *pins[] = {
    "6666",
    "6666",
    "1",
    "1",
    "77777777",
    "77777777"
  };

  static const gchar *otps[] = {
    "6ed4e4",
    "502a59",
    "bd94a4",
    "fb596e",
    "7abf75",
    "4d4ac4"
  };

  token = token_new ( "Test" );
  token_set_algorithm ( token, "MOTP", NULL );
  token_set_counter_type ( token, "time-based", NULL );
  token_set_key ( token, key, NULL );

  for ( i=0; i<6; ++i ){
    token->counter = epochs[i];
    token->pin = g_strdup( pins[i] );
    otp = token_motp( token );
    g_free ( (gpointer)token->pin );
    ok = ( g_strcmp0 (otp, otps[i] ) == 0 );
    if ( !ok )
      g_error ( "Error in test %d: Expected '%s', got '%s'\n", i, otps[i]);
  }

  token_free ( token );


  return 0;
}
