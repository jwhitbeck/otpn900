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


#include <glib/gstdio.h>
#include "config.h"
#include "tokendb.h"

#define KEY_KEY        "key"
#define KEY_ALGORITHM  "algorithm"
#define KEY_COUNTER_T  "counter type"
#define KEY_DISP_HEX   "display hex"
#define KEY_DIGITS     "digits"
#define KEY_COUNTER    "counter value"
#define KEY_INTERVAL   "interval"
#define KEY_PIN_SIZE   "pin size"


#define TOKEN_DB_FILE_TITLE "# OTP tokens"


TokenDB* 
token_db_new ( GError **err ) 
{
  TokenDB *self;
  const gchar *home_dir, *dot_config_dir, *app_config_dir;
  gint retv;
  GError *tmp_err, *tmp_err2;

  self = g_malloc ( sizeof (TokenDB) );

  /* Define paths */
  home_dir = g_get_home_dir ();
  dot_config_dir = g_strjoin ( "/", home_dir, USER_CONFIG_DIR, NULL );
  app_config_dir = g_strjoin ( "/", dot_config_dir, PACKAGE, NULL );
  self->file_name = g_strjoin ( "/", app_config_dir, TOKEN_DB_FILE_NAME, NULL );
  
  /* init key file */
  self->key_file = g_key_file_new ();
  
  /* initially not modified */
  self->modified = FALSE;

  /* Make sure that we have *write* access to the config directory */
  retv = g_mkdir_with_parents ( dot_config_dir, DIR_PUBLIC_MODE );
  retv += g_mkdir_with_parents ( app_config_dir, DIR_PRIVATE_MODE );
  if ( retv != 0 ){
    g_set_error ( err, OTP_ERROR, OTP_ERROR_DB_READ,
		  "Could not create config directory %s",
		  app_config_dir);
  }

  /* Now try to read the tokens file */
  else {
    tmp_err = NULL;
    g_key_file_load_from_file ( self->key_file, self->file_name,
				G_KEY_FILE_KEEP_COMMENTS,
				&tmp_err );
    if ( tmp_err ){
      switch ( tmp_err->code )
	{
	case G_FILE_ERROR_NOENT : /* Could not find tokens file */
	  tmp_err2 = NULL;
	  token_db_save ( self, &tmp_err2 );
	  if ( tmp_err2 ) { /* error writing initial configuration file */
	    g_propagate_error ( err, tmp_err2 );
	  } else { /* successfully initalized tokens */
	    g_set_error ( err, OTP_ERROR, OTP_ERROR_DB_NOT_FOUND,
			  "Creating tokens file in %s",
			  self->file_name);
	  }
	  g_error_free ( tmp_err );
	  break;
	default : 
	  g_propagate_error ( err, tmp_err );
      };
    }
  }
  
  /* Clean up temporary strings */
  g_free ( (gpointer)dot_config_dir );
  g_free ( (gpointer)app_config_dir );
  g_free ( (gpointer)home_dir );

  return self;
}


void 
token_db_free ( TokenDB *self )
{
  g_free ( (gpointer)(self->file_name) );
  if ( self->key_file )
    g_key_file_free ( self->key_file );
  g_free ( self );
}


void 
token_db_save ( TokenDB *self, GError **err )
{
  gchar *data;
  gsize length;
  GError *tmp_error;
  
  data = g_key_file_to_data ( self->key_file, &length, NULL );

  tmp_error = NULL;
  if ( length == 0 ){ /* This is probably the creation of the initial tokens file */
    g_file_set_contents ( self->file_name, 
			  TOKEN_DB_FILE_TITLE, -1, &tmp_error );
  }  else {
    g_file_set_contents ( self->file_name, data, length, &tmp_error );
  }

  if ( tmp_error ){
    g_set_error ( err, OTP_ERROR, OTP_ERROR_DB_WRITE,
		  "Error writing tokens file %s: %s",
		  self->file_name, g_strdup(tmp_error->message) );
    g_error_free ( tmp_error );
  } else { // success !!
    self->modified = FALSE;
  }
  g_free ( data );
}


gboolean 
token_db_has_token ( TokenDB *self, const gchar *name )
{
  return g_key_file_has_group( self->key_file, name );
}


#define SET_STRING(key,value)  g_key_file_set_value ( self->key_file, token->name, key, value )
#define SET_INTEGER(key,value)  g_key_file_set_integer ( self->key_file, token->name, key, value )
#define SET_BOOLEAN(key,value)  g_key_file_set_boolean ( self->key_file, token->name, key, value )
void 
token_db_add_token ( TokenDB *self, Token *token )
{
  /* key */
  SET_STRING ( KEY_KEY, token_get_key_as_str ( token ) );

  /* algorithm and counter type */
  SET_STRING ( KEY_ALGORITHM, algorithm_labels[token->algorithm] );
  SET_STRING ( KEY_COUNTER_T, counter_type_labels[token->counter_t] );

  /* algorithm specific parameters */
  switch ( token->algorithm )
    {
    case HOTP:
      switch ( token->counter_t )
	{
	case EVENT: SET_STRING ( KEY_COUNTER, token_get_counter_as_str (token) ); break;
	case TIME: SET_INTEGER ( KEY_INTERVAL, token->interval ); break;
	default:;
	}
      SET_BOOLEAN ( KEY_DISP_HEX, token->disp_hex );
      SET_INTEGER ( KEY_DIGITS, token->digits );
      break;
      
    case MOTP:
      if ( token->counter_t == EVENT ){
	SET_STRING ( KEY_COUNTER, token_get_counter_as_str (token) );
      }
      SET_INTEGER ( KEY_PIN_SIZE, token->pin_size );
      break;

    default:;
  }

  self->modified = TRUE;
}


void
token_db_del_token ( TokenDB *self, const gchar *name )
{
  GError *err;
  err = NULL;
  g_key_file_remove_group ( self->key_file, name, &err );
  if ( err ){
    g_error_free ( err );
  }
  self->modified = TRUE;
}



#define SET_REQ_VALUE(KEY,FUNC)						\
  buffer = g_key_file_get_value ( self->key_file, name, KEY, &lerr );	\
  FUNC ( token, buffer, &lerr );					\
  if ( lerr ){								\
    g_propagate_error ( err, lerr );					\
    goto err;								\
  }									\
  g_free ( buffer );							\
  buffer = NULL;

#define SET_OPT_VALUE(KEY,FUNC)						\
  buffer = g_key_file_get_value ( self->key_file, name, KEY, &lerr );	\
  FUNC ( token, buffer, &lerr );					\
  if ( lerr ){								\
    if ( lerr->code != G_KEY_FILE_ERROR_KEY_NOT_FOUND ){		\
      g_propagate_error ( err, lerr );					\
      goto err;								\
    } else {								\
      g_error_free ( lerr );						\
    }									\
  }									\
  g_free ( buffer );							\
  buffer = NULL;


Token*
token_db_get_token ( TokenDB *self, const gchar *name, GError **err )
{
  Token *token;
  gchar *buffer;
  GError *lerr;

  buffer = NULL;
  lerr = NULL;
  
  token = token_new ( g_strdup ( name ) );

  /* get algorithm */
  SET_REQ_VALUE ( KEY_ALGORITHM, token_set_algorithm );

  /* get counter type */
  SET_REQ_VALUE ( KEY_COUNTER_T, token_set_counter_type );

  /* read key */
  SET_REQ_VALUE ( KEY_KEY, token_set_key );

  /* Read algorithm-specific params */
  switch ( token->algorithm )
    {
    case HOTP:
      /* get required parameters */
      switch ( token->counter_t )
	{
	case EVENT: SET_REQ_VALUE ( KEY_COUNTER, token_set_counter ); break;
	case TIME:  SET_REQ_VALUE ( KEY_INTERVAL, token_set_interval ); break;
	default:;
	}
      /* optional parameters */
      SET_OPT_VALUE ( KEY_DISP_HEX, token_set_disp_hex );
      SET_OPT_VALUE ( KEY_DIGITS, token_set_digits );
      break;

    case MOTP:
      if ( token->counter_t == EVENT ){
	SET_REQ_VALUE ( KEY_COUNTER, token_set_counter );
      }
      SET_OPT_VALUE ( KEY_PIN_SIZE, token_set_pin_size );
      break;

    default:;
    }

  return token;

 err:
  if ( buffer )
    g_free ( buffer );
  token_free ( token );
  return NULL;
      
}


gchar** 
token_db_get_names ( TokenDB *self, gsize *length )
{
  return g_key_file_get_groups ( self->key_file, length );
}

