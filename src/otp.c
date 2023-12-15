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

#include <stdlib.h>
#include <stdio.h>
#include <baseencode.h>

#include <time.h>
#include <string.h>
#include "otp.h"
#include "strconv.h"
#include "config.h"

#define SHA_DIGESTSIZE 20
#define SHA_BLOCKSIZE 64


/* Error quarks */
GQuark 
otp_error_quark ( void )
{
  return g_quark_from_static_string ("otp-error-quark");
}


/* Labels */
const gchar* algorithm_labels[] = { "HOTP", "MOTP" };
const gchar* counter_type_labels[] = { "event-based", "time-based" };


/* Tokens */
Token* 
token_new ( const gchar *name )
{
  Token *token;
  token = g_malloc ( sizeof ( Token ) );
  token->name = g_strdup ( name );
  token->key = NULL ;
  token->key_l = 0;
  token->pin = NULL;
  token->pin_size = DEFAULT_PIN_SIZE;
  token->digits = DEFAULT_DIGITS;
  token->disp_hex = DEFAULT_DISP_HEX;
  token->modified = FALSE;
  return token;
}


void 
token_free ( Token *token )
{
  g_free ( (gpointer)token->name );
  if ( token->key )
    g_free ( (gpointer)token->key );
  if ( token->pin )
    g_free ( (gpointer)token->pin );
  g_free ( token );
}


const guint8* 
token_key_new ( Algorithm algorithm, guint *length )
{
  const guint8 *key;
  GRand *rng;
  guint32 *buf;
  guint i, il=0;

  rng = g_rand_new ();

  switch ( algorithm )
    {
    case HOTP: il = DEFAULT_HOTP_KEY_LENGTH / 32; break;
    case MOTP: il = DEFAULT_MOTP_KEY_LENGTH / 32; break;
    default:;
    }

  buf = g_malloc ( il * 32 );
  for ( i=0; i < il ; ++ i )
    buf[i] = g_rand_int ( rng );

  key = (guint8*)buf;
  *length = il * 4; // 4 guint8's per guint32

  g_rand_free ( rng );

  return key;
}

const gchar*
token_key_new_as_str ( Algorithm algorithm )
{
  const guint8 *hex_key;
  const gchar *str_key;
  guint l;

  hex_key = token_key_new ( algorithm, &l );
  str_key = hex_keytostr ( hex_key, l );
  
  g_free ( (gpointer)hex_key );

  return str_key;
}


const gchar*
token_otp ( Token *token )
{
  const gchar *otp;

  if ( token->counter_t == TIME ) {
    token->counter = token_epoch ( token );
  }

  otp = NULL;
  switch ( token->algorithm )
    {
    case HOTP: otp = token_hotp ( token ); break;
    case MOTP: otp = token_motp ( token ); break;
    default:;
    }

  return otp;
}


const gchar*
token_motp ( Token *token )
{
  GChecksum *md5;
  const gchar *otp;
  const gchar *key_str;
  guchar *buffer;

  md5 = g_checksum_new ( G_CHECKSUM_MD5 );

  key_str = token_get_key_as_str ( token );
  
  buffer = (guchar*)g_strdup_printf ( "%lu%s%s", (gulong)token->counter, key_str , token->pin );

  g_checksum_update ( md5, buffer, -1 );

  otp = g_strndup ( g_checksum_get_string ( md5 ), token->digits );

  g_checksum_free ( md5 );
  g_free ( (gpointer)key_str );
  g_free ( buffer );
  return otp;
}


const gchar*
token_hotp ( Token *token )
{
  guint8 C[8];
  gint i, lk, ld;
  guint64 counter;
  guint8 hmac[SHA_DIGESTSIZE];
  const gchar *otp;
  guint8 *buffer;
  guint32 mask;
  guint64 modulo;
  guint32 offset, value;

  /* Encode counter */
  counter = token->counter;
  ld = sizeof(C);
  for ( i=ld-1; i >= 0; --i ){
    C[i] = counter & 0xff;
    counter >>= 8;
  }
  
  lk = token->key_l;

  /* Compute HMAC-SHA1 */
  hmac_sha1 ( hmac, token->key, lk, C, ld );

  /* Truncate to extract a 31 bit integer value */
  offset = hmac[SHA_DIGESTSIZE - 1] & 0x0f;
  value = ((hmac[offset] & 0x7f) << 24) | ((hmac[offset + 1] & 0xff) << 16)
    | ((hmac[offset + 2] & 0xff) << 8) | (hmac[offset + 3] & 0xff);

  /* create the otp */
  buffer = g_malloc ( sizeof(guint8) * ( token->digits + 1 ) ); // add an extra char for the null character
  if ( token->disp_hex ){ /* return hexadecimal */
    mask = ( token->digits < 8 ) ? ((1 << (4 * token->digits)) - 1) : -1 ;
    g_snprintf ( (gchar*)buffer, token->digits+1, "%0*x", token->digits, value & mask );
  } else { /* return decimal */ 
    modulo = 1;
    for ( i = 0; i < token->digits; ++i ) modulo *= 10;
    g_snprintf ( (gchar*)buffer, token->digits+1, "%0*u", token->digits, (guint)(value % modulo) );
  }
  buffer[token->digits] = '\0';

  otp = (gchar*)buffer;
  return otp;
}


void
hmac_sha1 ( guint8 *hmac, const guint8 *key, guint lk, const guint8 *data, guint ld )
{
  GChecksum *sha1;
  guint i;
  gsize dl;
  guint8 k_buf[SHA_DIGESTSIZE];
  guint8 buf[SHA_BLOCKSIZE];
  guint8 isha[SHA_DIGESTSIZE];
  const guint8 *k;


  sha1 = g_checksum_new ( G_CHECKSUM_SHA1 );

  /* make sure key isn't too long */
  if ( lk > SHA_BLOCKSIZE ) {
    g_checksum_reset ( sha1 );
    g_checksum_update ( sha1, key, lk );
    dl = SHA_DIGESTSIZE;
    g_checksum_get_digest ( sha1, k_buf, &dl);
    lk = SHA_DIGESTSIZE;
    k = k_buf;
  } else {
    k = key;
  }

  /**** Inner digest ****/

  g_checksum_reset ( sha1 );

   /* Pad the key for inner digest */
  for ( i=0; i < lk; ++i ) buf[i] = k[i] ^ 0x36;
  for ( i=lk; i < SHA_BLOCKSIZE; ++i ) buf[i] = 0x36;

  g_checksum_update ( sha1, buf, SHA_BLOCKSIZE );
  g_checksum_update ( sha1, data, ld );
  dl = SHA_DIGESTSIZE;
  g_checksum_get_digest ( sha1, isha, &dl);

  /**** Outer digest ****/

  g_checksum_reset ( sha1 );

  /* Pad the key for outer digest */
  for ( i=0; i < lk; ++i ) buf[i] = k[i] ^ 0x5C;
  for ( i=lk; i < SHA_BLOCKSIZE; ++i ) buf[i] = 0x5C;

  g_checksum_update ( sha1, buf, SHA_BLOCKSIZE );
  g_checksum_update ( sha1, isha, SHA_DIGESTSIZE );

  /* copy over the final result */
  dl = SHA_DIGESTSIZE;
  g_checksum_get_digest ( sha1, hmac, &dl);

   /* cleanup */
  g_checksum_free ( sha1 );
  
}


gboolean
token_key_cmp ( const guint8 *key1, guint l1, const guint8 *key2, guint l2 )
{
  guint i;

  if ( l1 != l2 )
    return FALSE;

  for ( i=0; i<l1; ++i)
    if ( key1[i] != key2[i] )
      return FALSE;
  
  return TRUE;
}


guint64
token_epoch ( Token *token )
{
  if ( token->counter_t == TIME )
    return ((guint64)( time ( NULL )) / token->interval );
  return 0;
}

static char* bin2hex(char* bin) {
  int len = strlen(bin);
  char output[(len * 2) + 1];
  for (int i = 0; i < len; i++) {
    sprintf(&output[i*2], "%02X", bin[i]);
  }
  return strdup(output);
}

void 
token_set_key ( Token *token, const gchar *str, GError **err )
{
  GError *lerr;
  const guint8 *key;
  KeyType key_t;
  guint key_l;

  baseencode_error_t berr;
  char* base32_decoded = NULL;


  key_l = 0;
  key = NULL;
  key_t = 0;
  
  switch ( token->algorithm )
    {
    case HOTP: /* HOTP passwords can be base32, binary or ascii */

      /* Check if it is base32 encoded, if it is, use the hex path */
      base32_decoded = base32_decode(str, strlen(str), &berr);
      if (berr == SUCCESS) {
        char* hex = bin2hex(base32_decoded);
        free(base32_decoded);

        lerr = NULL;
        key_t = HEX;
        key = hex_strtokey ( hex, &key_l, &lerr );
        free(hex);

        if ( !lerr ){ /* otherwise, try path below */
          break;
        }
      }
      /* first try to interpret as a hex string */
      lerr = NULL;
      key_t = HEX;
      key = hex_strtokey ( str, &key_l, &lerr );
      if ( lerr ){ /* otherwise, use key as raw ascii */
	g_error_free ( lerr );
	key = (guint8*)g_strdup ( str );
	key_l = strlen ( str );
	key_t = ASCII;
      }
      break;

    case MOTP: /* MOTP passwords are always ascii */
      key = (guint8*)g_strdup ( str );
      key_l = strlen ( str );
      key_t = ASCII;
      break;
      
    default:;
    }

  /* if there is an existing key, free it */
  if ( token->key )
    g_free ( (gpointer)token->key );

  token->key = key;
  token->key_l = key_l;
  token->key_t = key_t;
}


void
token_set_algorithm ( Token *token, const gchar *str, GError **err)
{
  gchar* str_up;
  Algorithm alg;
  
  str_up = g_ascii_strup ( str, -1 );
  for ( alg=0; alg < N_OTP_ALGORITHMS; ++alg ){
    if ( g_strcmp0( algorithm_labels[alg], str_up ) == 0 )
      break;
  }
  g_free ( str_up );
  if ( alg == N_OTP_ALGORITHMS ){
    g_set_error ( err, OTP_ERROR, OTP_ERROR_INVALID_TOKEN,
		  "Invalid algorithm '%s'.", str );
    return;
  }
  
  token->algorithm = alg;
  switch ( alg )
    {
    case HOTP: 
      token->interval = DEFAULT_HOTP_INTERVAL;
      token->counter = DEFAULT_COUNTER; 
      break;
      
    case MOTP:
	token->interval = DEFAULT_MOTP_INTERVAL;
	token->counter = DEFAULT_COUNTER;
	break;

    default:;
    }

}


void
token_set_counter_type ( Token *token, const gchar *str, GError **err )
{
  gchar *str_up, *label_up;
  CounterType counter_t;

  str_up = g_ascii_strup ( str, -1 );
  for ( counter_t=0; counter_t < N_OTP_COUNTER_TYPES; ++counter_t ){
    label_up = g_ascii_strup ( counter_type_labels[counter_t], -1 );
    if ( g_strcmp0( label_up, str_up ) == 0 ){
      g_free ( label_up );
      break;
    } else {
      g_free ( label_up );
    }
  }
  g_free ( str_up );
  if ( counter_t == N_OTP_COUNTER_TYPES ){
    g_set_error ( err, OTP_ERROR, OTP_ERROR_INVALID_TOKEN,
		  "Invalid counter type '%s'.", str );
    return;
  }

  token->counter_t = counter_t;
}


void
token_set_interval ( Token *token, const gchar *str, GError **err )
{
  guint interval;
  GError *lerr;

  lerr = NULL;
  interval = strtouint ( str, &lerr );
  if ( lerr ){
    g_propagate_error ( err, lerr );
    return;
  }

  if ( interval == 0 ){ /* prevent 0 intervals */
    g_set_error ( err, OTP_ERROR, OTP_ERROR_INVALID_TOKEN,
		  "Interval must be greater than 1" );
    return;
  }

  token->interval = interval;
}


void
token_set_counter ( Token *token, const gchar *str, GError **err )
{
  guint64 counter;
  GError *lerr;

  lerr = NULL;
  counter = strtouint64 ( str, &lerr );
  if ( lerr ){
    g_propagate_error ( err, lerr );
    return;
  }

  token->counter = counter;
}


void 
token_set_disp_hex ( Token *token, const gchar *str, GError **err )
{
  guint i;
  gchar *str_lower;

  const static gchar *OK[] = { "true", "yes", "1" };
  const static gchar *NOK[] = { "false", "no", "0" };

  for ( i=0; i<3; ++i ){
    str_lower = g_ascii_strdown ( str, -1 );
    if ( ( g_strcmp0 ( str_lower, OK[i] ) ) == 0 ){
      token->disp_hex = TRUE;
      return;
    }
  }
  for ( i=0; i<3; ++i ){
    str_lower = g_ascii_strdown ( str, -1 );
    if ( ( g_strcmp0 ( str_lower, NOK[i] ) ) == 0 ){
      token->disp_hex = FALSE;
      return;
    }
  }
  g_set_error ( err, OTP_ERROR, OTP_ERROR_INVALID_TOKEN,
		"'%s' is not a valid boolean", str );
}


void
token_set_digits ( Token *token,  const gchar *str, GError **err )
{
  guint digits;
  GError *lerr; 

  lerr = NULL;
  digits = strtouint ( str, &lerr );
  if ( lerr ){
    g_propagate_error ( err, lerr );
    return;
  }

  if ( digits < MIN_DIGITS || digits > MAX_DIGITS ){
    g_set_error ( err, OTP_ERROR, OTP_ERROR_INVALID_TOKEN,
		  "Number of digits must be between %d and %d",
		  MIN_DIGITS, MAX_DIGITS );
    return;
  }

  token->digits = digits;
}


void
token_set_pin_size ( Token *token,  const gchar *str, GError **err )
{
  guint pin_size;
  GError *lerr; 

  lerr = NULL;
  pin_size = strtouint ( str, &lerr );
  if ( lerr ){
    g_propagate_error ( err, lerr );
    return;
  }

  if ( pin_size < MIN_PIN_SIZE || pin_size > MAX_PIN_SIZE ){
    g_set_error ( err, OTP_ERROR, OTP_ERROR_INVALID_TOKEN,
		  "Pin size must be between %d and %d",
		  MIN_PIN_SIZE, MAX_PIN_SIZE );
    return;
  }

  token->pin_size = pin_size;
}


void
token_set_pin ( Token *token,  const gchar *str, GError **err )
{
  if ( token->pin )
    g_free ( (gpointer)token->pin );
  token->pin = str;
}


const gchar*
token_get_key_as_str ( Token *token )
{
  if ( token->key_t == HEX )
    return hex_keytostr ( token->key, token->key_l );
  return g_strdup ( (gchar*)token->key );
}


const gchar*
token_get_counter_as_str ( Token *token )
{
  return uint64tostr ( token->counter );
}
