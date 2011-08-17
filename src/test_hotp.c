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

/*** HOTP test vectors from RFC4226 ***/

int
main ()
{
  static const gchar key[] = "3132333435363738393031323334353637383930";
  static const gchar *decimal_hotps[] = {
    "755224", 
    "287082",
    "359152",
    "969429",
    "338314",
    "254676",
    "287922",
    "162583",
    "399871",
    "520489"
  };

  Token *token;
  guint i;
  const gchar *otp;
  gboolean ok;

  token = token_new ( "Test" );
  token_set_algorithm ( token, "HOTP", NULL );
  token_set_counter_type ( token, "event-based", NULL );
  token_set_key ( token, key, NULL );
  token->disp_hex = FALSE;

  for ( i=0; i<10; ++i ){
    token->counter = i;
    otp = token_hotp( token );
    ok = ( g_strcmp0 (otp, decimal_hotps[i] ) == 0 );
    if ( ! ok )
      g_error ( "Error for counter %d: Expected '%s', got '%s'\n", i, decimal_hotps[i] );
  }

  token_free ( token );

  return 0;
}
