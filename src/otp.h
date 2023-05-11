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

#ifndef OTP_H
#define OTP_H

#include <glib.h>

/* Error domain */
#define OTP_ERROR otp_error_quark ()
GQuark otp_error_quark ( void );

/* errors */
enum {
  OTP_ERROR_INVALID_TOKEN,
  OTP_ERROR_DB_READ,
  OTP_ERROR_DB_WRITE,
  OTP_ERROR_DB_NOT_FOUND
};

/* OTP algorithms */
typedef enum {
  HOTP,
  MOTP,
  N_OTP_ALGORITHMS
} Algorithm;


/* OTP Counter types */
typedef enum {
  EVENT,
  TIME,
  N_OTP_COUNTER_TYPES
} CounterType;

/* Supported key types */
typedef enum {
  HEX,
  ASCII,
  N_OTP_KEY_TYPES
} KeyType;


extern const gchar* algorithm_labels[N_OTP_ALGORITHMS];
extern const gchar* counter_type_labels[N_OTP_COUNTER_TYPES];


/* OTP Tokens */
typedef struct {

  /* variables stored in config file */
  const gchar *name;
  const guint8 *key;
  
  Algorithm algorithm;
  CounterType counter_t;

  guint64 counter;
  guint interval;

  guint pin_size; // MOTP specific

  gboolean disp_hex; // HOTP specific
  guint digits; // HOTP specific
  
  /* Execution-time variables */
  const gchar *pin; // MOTP specific
  gboolean modified;

  guint key_l;
  KeyType key_t;

} Token;


Token* token_new ( const gchar *name );
void token_free ( Token *token );

const gchar* token_otp ( Token *token );
const gchar* token_hotp ( Token *token );
const gchar* token_motp ( Token *token );

void token_set_key ( Token *token, const gchar *str, GError **err );
void token_set_algorithm ( Token *token, const gchar *str, GError **err );
void token_set_counter_type ( Token *token, const gchar *str, GError **err );
void token_set_counter ( Token *token, const gchar *str, GError **err );
void token_set_interval ( Token *token, const gchar *str, GError **err );
void token_set_disp_hex ( Token *token, const gchar *str, GError **err );
void token_set_digits ( Token *token,  const gchar *str, GError **err );
void token_set_pin_size ( Token *token,  const gchar *str, GError **err );
void token_set_pin ( Token *token,  const gchar *str, GError **err );

const gchar* token_get_key_as_str ( Token *token );
const gchar* token_get_counter_as_str ( Token *token );

const guint8* token_key_new ( Algorithm algorithm, guint *length );
const gchar* token_key_new_as_str ( Algorithm algorithm );
guint64 token_epoch ( Token *token );
gboolean token_key_cmp ( const guint8 *key1, guint l1, const guint8 *key2, guint l2 );
void hmac_sha1 ( guint8 *hmac, const guint8 *key, guint lk, const guint8 *data, guint ld );

#endif
