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


#include "tokenview.h"
#include "etdialog.h"
#include "util.h"
#include "otp.h"
#include "pindialog.h"

#define DISPLAY_FONT_SIZE 50


static HildonStackableWindowClass *parent_class = NULL;

static void token_view_class_init ( TokenViewClass *klass );
static void token_view_init ( TokenView *self );
static void init_menu ( TokenView *self );
static void edit_token ( GtkWidget *widget, TokenView *self );
static void update_header_counter ( TokenView *self );
static void update_header_epoch ( TokenView *self );
static void update_header ( TokenView *self );
static void generate ( GtkWidget *widget, TokenView *self );
static void start_epoch_thread ( TokenView *self );
static void stop_epoch_thread ( TokenView *self );
static void epoch_thread ( TokenView *self );

static gboolean run_epoch_thread = FALSE;


G_DEFINE_TYPE (TokenView, token_view, HILDON_TYPE_STACKABLE_WINDOW)


static void
token_view_class_init ( TokenViewClass *klass )
{
  parent_class = gtk_type_class (gtk_widget_get_type ());
}


static void
token_view_init ( TokenView *self )
{
  GtkWidget *vbox;
  GtkWidget *gen_button;
  
  init_menu ( self );

  vbox = gtk_vbox_new ( FALSE, 10 ); // homogeneous, spacing

  self->header = gtk_label_new ( NULL );
  gtk_box_pack_start ( GTK_BOX(vbox), self->header, FALSE, TRUE, 0 );

  self->display = gtk_label_new ( NULL );
  set_widget_font_size ( self->display, DISPLAY_FONT_SIZE );
  gtk_box_pack_start ( GTK_BOX(vbox), self->display, TRUE, TRUE, 0 );

  gen_button = hildon_button_new_with_text ( HILDON_SIZE_THUMB_HEIGHT, HILDON_BUTTON_ARRANGEMENT_HORIZONTAL,
					     "Generate", NULL );
  gtk_box_pack_start ( GTK_BOX(vbox), gen_button, FALSE, TRUE, 0 );
  g_signal_connect ( gen_button, "clicked", G_CALLBACK(generate), self );

  gtk_container_add ( GTK_CONTAINER(self), vbox );

  g_signal_connect_swapped ( G_OBJECT ( self->header ), "destroy", 
			     G_CALLBACK(stop_epoch_thread), NULL ); 

}


static void
init_menu ( TokenView *self )
{
  GtkWidget *menu;
  GtkWidget *edit_button;
  
  menu = hildon_app_menu_new();

  /* Edit button */
  edit_button = gtk_button_new_with_label ( "Edit" );
  g_signal_connect_after ( edit_button, "clicked", 
			   G_CALLBACK(edit_token),
			   self );
  hildon_app_menu_append ( HILDON_APP_MENU(menu), GTK_BUTTON(edit_button) );


  hildon_window_set_app_menu ( HILDON_WINDOW(self),
			       HILDON_APP_MENU(menu) );

  gtk_widget_show_all ( menu );
}


GtkWidget*
token_view_new ( Token *token )
{
  GtkWidget *token_view;
  TokenView *self;

  token_view = g_object_new ( TYPE_TOKEN_VIEW, NULL );
  self = TOKEN_VIEW ( token_view );
  self->token = token;

  gtk_window_set_title ( GTK_WINDOW(self), token->name );

  update_header ( self );
  if ( token->counter_t == TIME )
    start_epoch_thread ( self );

  if ( token->algorithm == MOTP ) {// preload pin_dialog
    self->pin_dialog = pin_dialog_new ( GTK_WINDOW(self), 
					self->token->name, self->token->pin_size );
    gtk_widget_hide ( self->pin_dialog );
    g_signal_connect_swapped ( self, "destroy", G_CALLBACK(gtk_widget_destroy), self->pin_dialog );
  }

  return token_view;
}


static void 
edit_token ( GtkWidget *widget, TokenView *self)
{
  GtkWidget *et_dialog;

  et_dialog = et_dialog_new ( GTK_WINDOW(self), self->token );
  gtk_dialog_run ( GTK_DIALOG(et_dialog) );
  gtk_widget_destroy ( et_dialog );

  update_header ( self );
}



static void 
generate ( GtkWidget *widget, TokenView *self )
{
  const gchar *otp;
  gint result;

  if ( self->token->algorithm == MOTP && ! self->token->pin ){
    result = gtk_dialog_run ( GTK_DIALOG(self->pin_dialog) );
    gtk_widget_hide ( self->pin_dialog );
    if ( result == GTK_RESPONSE_OK ){
      self->token->pin = g_strdup ( pin_dialog_get_pin ( PIN_DIALOG ( self->pin_dialog ) ) );
      pin_dialog_clear_pin ( PIN_DIALOG ( self->pin_dialog ) );
    }
    else {
      pin_dialog_clear_pin ( PIN_DIALOG ( self->pin_dialog ) );
      return;
    }
  }
  
  otp = token_otp ( self->token );
  gtk_label_set_text ( GTK_LABEL ( self->display ), otp );

  if ( self->token->counter_t == EVENT ){
    self->token->counter += 1; // increment counter
    self->token->modified = TRUE;
    update_header_counter ( self );
  }
  
}


static void 
update_header_counter ( TokenView *self )
{
  const gchar *header;
  header = g_strdup_printf ( "Counter value: %lu ", (gulong)self->token->counter );
  gtk_label_set_text ( GTK_LABEL ( self->header ), header );
}


static void 
update_header_epoch ( TokenView *self )
{
  const gchar *header;
  header = g_strdup_printf ( "Epoch: %lu ", (gulong) token_epoch ( self->token ) );
  gtk_label_set_text ( GTK_LABEL ( self->header ), header );
}


static void
update_header ( TokenView *self )
{
  switch ( self->token->counter_t )
    {
    case EVENT: update_header_counter ( self ); break;
    case TIME:  update_header_epoch ( self ); break;
    default:;
    }
}



static void
epoch_thread ( TokenView *self )
{
  while ( run_epoch_thread ){
    update_header_epoch ( self );
    g_usleep ( G_USEC_PER_SEC ); // 1 sec
  }
}


static void
start_epoch_thread ( TokenView *self )
{
  if ( ! g_thread_supported () )
    g_thread_init ( NULL );
  run_epoch_thread = TRUE;
  g_thread_create ( (GThreadFunc)epoch_thread, self, FALSE, NULL );
}


static void
stop_epoch_thread ( TokenView *self )
{
  run_epoch_thread = FALSE;
}
