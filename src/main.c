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


#include <hildon/hildon.h>
#include <libosso.h>

#include "util.h"
#include "tokendb.h"
#include "tokenselector.h"


void 
save_and_quit ( GtkWidget *widget, TokenDB *token_db )
{
  GError *err;
  if ( token_db->modified ){
    err = NULL;
    token_db_save ( token_db, &err );
    if ( err ){
      alert ( widget, err->message );
      g_error_free ( err );
    }
  }
  gtk_main_quit ();
}


int 
main ( int argc, char *argv[] ) 
{
  osso_context_t *osso_context;
  TokenDB *token_db;
  GError *err = NULL;
  GtkWidget *token_selector;
  gboolean ok;
  

  /* initalize osso context */
  osso_context = osso_initialize ( PACKAGE, VERSION, FALSE, NULL );
  if ( ! osso_context ) {
    g_error ( "Failed to initialize LibOSSO\n" );
  }
 
  /* initalize the program */
  hildon_gtk_init ( &argc, &argv );

  /* load tokens */
  ok = TRUE;
  token_db = token_db_new ( &err );
  if ( err ){
    switch ( err->code ){
    case OTP_ERROR_DB_NOT_FOUND : 
      warn ( NULL, err->message ); break;
    default :
      alert ( NULL, "Error reading tokens file %s: %s", token_db->file_name, err->message );
      ok = FALSE;
    }
    g_error_free ( err );
  } 
  /* start the main loop if token_db was successfully loaded/initialized */
  if ( ok ) { 
    token_selector = token_selector_new ( token_db );
    g_signal_connect(G_OBJECT (token_selector), "destroy", 
		     G_CALLBACK(save_and_quit), token_db );
    gtk_widget_show_all ( token_selector );
    gtk_main ();
  }

  token_db_free ( token_db );

  /* close down osso before quitting */
  osso_deinitialize ( osso_context );

  return 0;
}
