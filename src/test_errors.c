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
#include "config.h"

#define ASSERT_ERROR				\
  if ( err ){					\
    g_error_free ( err );			\
    err = NULL;					\
  } else {					\
    g_error ( NULL );				\
  }

#define ASSERT_NO_ERROR				\
  if ( err ) {					\
    g_error ( err->message );			\
  }						
  

int
main ()
{
  Token *token;
  GError *err;

  token = token_new ( "Test" );
  err = NULL;
    
  token_set_algorithm ( token, "foo", &err );
  ASSERT_ERROR;
  token_set_algorithm ( token, "HotP", &err );
  ASSERT_NO_ERROR;

  token_set_counter_type ( token, "time", &err );
  ASSERT_ERROR;
  token_set_counter_type ( token, "time-based", &err );
  ASSERT_NO_ERROR;

  token_set_counter ( token, "-12", &err );
  ASSERT_ERROR;
  token_set_counter ( token, "abc", &err );
  ASSERT_ERROR;
  token_set_counter ( token, "9999999999999999999999999999999999999", &err );
  ASSERT_ERROR;
  token_set_counter ( token, "999999999999999", &err );
  ASSERT_NO_ERROR;

  token_set_interval ( token, "abc", &err );
  ASSERT_ERROR;
  token_set_interval ( token, "-10", &err );
  ASSERT_ERROR;
  token_set_interval ( token, "0", &err );
  ASSERT_ERROR;
  token_set_interval ( token, "50", &err );
  ASSERT_NO_ERROR;

  token_set_disp_hex ( token, "oh yes", &err );
  ASSERT_ERROR;
  token_set_disp_hex ( token, "falSe", &err );
  ASSERT_NO_ERROR;
  token_set_disp_hex ( token, "Yes", &err );
  ASSERT_NO_ERROR;

  token_set_digits ( token, g_strdup_printf("%d", MAX_DIGITS+1), &err );
  ASSERT_ERROR;
  token_set_digits ( token, g_strdup_printf("%d", MIN_DIGITS-1), &err );
  ASSERT_ERROR;
  token_set_digits ( token, "-1", &err );
  ASSERT_ERROR;
  token_set_digits ( token, "abc", &err );
  ASSERT_ERROR;
  token_set_digits ( token, g_strdup_printf("%d", MIN_DIGITS), &err );
  ASSERT_NO_ERROR;

  token_set_pin_size ( token, g_strdup_printf("%d", MAX_PIN_SIZE+1), &err );
  ASSERT_ERROR;
  token_set_pin_size ( token, g_strdup_printf("%d", MIN_PIN_SIZE-1), &err );
  ASSERT_ERROR;
  token_set_pin_size ( token, "-1", &err );
  ASSERT_ERROR;
  token_set_pin_size ( token, "abc", &err );
  ASSERT_ERROR;
  token_set_pin_size ( token, g_strdup_printf("%d", MIN_PIN_SIZE), &err );
  ASSERT_NO_ERROR;

  return 0;
}
