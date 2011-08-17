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


#include <errno.h>
#include <string.h>
#include "strconv.h"
#include "otp.h"

static const char *hex_chars = "0123456789abcdef";


guint64 
strtouint64 ( const gchar *s, GError **err )
{
  guint64 long_value;
  gchar *end;
  
  
  long_value = g_ascii_strtoull ( s, &end, 10 ); // base 10
  
  // make sure number was properly understood
  if ( *s == '\0' || *end != '\0' ){
    g_set_error (err, OTP_ERROR, STRCONV_ERROR_PARSE,
		 "Value '%s' cannot be interpreted as a number.", s);
    return 0;
  }

  // then check if it is in range
  if ( errno == ERANGE ){
    errno = 0;
    g_set_error (err, OTP_ERROR, STRCONV_ERROR_RANGE,
		 "Integer value '%s' out of range.", s);
    return 0;
  }

  // finally make sure that the user has not entered a negative number
  if ( s[0] == '-' ){
    g_set_error (err, OTP_ERROR,
		 STRCONV_ERROR_NEGATIVE,
		 "%ld is a negative number.", (gulong)long_value);
    return 0;
  }
  
  return long_value;
}


guint 
strtouint ( const gchar *s, GError **err )
{
  guint value;
  guint64 long_value;
  GError *lerr;

  lerr = NULL;
  long_value = strtouint64 ( s, &lerr );
  if ( lerr ){
    g_propagate_error ( err, lerr );
    return 0;
  }

  value = (guint) long_value;
  if ( value != long_value ){
    g_set_error (err, OTP_ERROR, STRCONV_ERROR_RANGE,
		 "Integer value '%lu' out of range.", (gulong)long_value );
  }

  return value;
}


const gchar* 
uint64tostr ( guint64 long_value )
{
  return g_strdup_printf ( "%lu", (gulong)long_value );
}


const guint8* 
hex_strtokey ( const gchar *str, guint *lk, GError **err )
{
  const guint8 *key;
  guint8 *buffer;
  gchar c1, c2;
  guint8 v1, v2;
  guint i,ls;

  ls = strlen(str);
  if ( (ls & 1) == 1 ){ // ls is odd
    g_set_error ( err, OTP_ERROR, STRCONV_ERROR_PARSE,
		  "Key '%s' must have an even number of hex digits.", str );
    return NULL;
  }

  *lk = ls / 2;
  buffer = g_malloc ( sizeof(guint8) * (*lk) );

  for ( i=0; i<*lk; ++i){
    c1 = str[2*i]; 
    c2 = str[2*i+1];
    if ( ! ( g_ascii_isxdigit(c1) && g_ascii_isxdigit(c2) ) ){
      g_set_error ( err, OTP_ERROR, STRCONV_ERROR_PARSE,
		    "Key '%s' contains non-hexadecimal digits.", str );
      g_free ( buffer );
      return NULL;
    }
    v1 = (guint8)g_ascii_xdigit_value ( c1 ); 
    v2 = (guint8)g_ascii_xdigit_value ( c2 );
    buffer[i] = ( v1 << 4 ) | ( v2 & 0x0f );
  }

  key = buffer;
  return key;
}


const gchar* 
hex_keytostr ( const guint8 *key, guint lk )
{
  const gchar *str;
  gchar *buffer;
  guint i, ls;
  guint8 v;
  
  ls = 2 * lk + 1; // the returned string will have double the length, plus a null char at the end
  buffer = g_malloc ( sizeof(gchar) * ls );

  for (i = 0; i < ls-1; i++) {
    v = key [i / 2];
    if ((i & 1) == 0) // if i is even
      v >>= 4;
    v &= 0x0f; // keep only 4 last bits of v
    buffer[i] = hex_chars[v];
  }
  buffer[ls-1] = '\0';

  str = buffer;
  return str;
}
