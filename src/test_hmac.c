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
#include "strconv.h"

/*** Test cases for HMAC-SHA-1 from RFC2202 ****/

#define SHA_DIGESTSIZE 20
#define SHA_BLOCKSIZE 64

int
main ()
{
  guint n, lk;
  guint8 hmac[SHA_DIGESTSIZE];
  gboolean ok;
  Token *tkey, *tdata;
  const guint8 *digest;
  
  static const gchar *keys[] = {
    "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b",
    "Jefe",
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
    "0102030405060708090a0b0c0d0e0f10111213141516171819",
    "0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c",
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
  };

  static const gchar *data[] = {
    "Hi There",
    "what do ya want for nothing?",
    "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
    "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd",
    "Test With Truncation",
    "Test Using Larger Than Block-Size Key - Hash Key First",
    "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data"
  };

  static const guint lks[] = { 20, 4, 20, 25, 20, 80, 80 };
  static const guint lds[] = { 8, 28, 50, 50, 20, 54, 73 };

  static const gchar *digests[] = {
    "b617318655057264e28bc0b6fb378c8ef146be00",
    "effcdf6ae5eb2fa2d27416d5f184df9c259a7c79",
    "125d7342b9ac11cd91a39af48aa17b4f63f175d3",
    "4c9007f4026250c6bc8414f9bf50c86c2d7235da",
    "4c1a03424b55e07fe7f27be1d58bb9324a9a5a04",
    "aa4ae5e15272d00e95705637ce8a3b55ed402112",
    "e8e99d0f45237d786d6bbaa7965c7808bbff1a91"
  };


  tkey = token_new ( "Key" );
  tdata = token_new ( "Data" );
  token_set_algorithm ( tkey, "HOTP", NULL );
  token_set_algorithm ( tdata, "HOTP", NULL );

  for ( n=0; n<7; ++n ){
    token_set_key ( tkey, keys[n], NULL );
    token_set_key ( tdata, data[n], NULL );

    hmac_sha1 ( hmac, tkey->key, lks[n], tdata->key, lds[n] );

    digest = hex_strtokey ( digests[n], &lk, NULL );

    ok = token_key_cmp ( hmac, SHA_DIGESTSIZE, digest, SHA_DIGESTSIZE );

    if ( ! ok )
      g_error ( "Error in test case %d: Expected '%s', got '%s'\n", 
		n+1, hex_keytostr(digest,SHA_DIGESTSIZE), 
		hex_keytostr(hmac,SHA_DIGESTSIZE) );

    g_free ( (gpointer)digest );
  }

  token_free ( tkey );
  token_free ( tdata );

  return 0;
}


