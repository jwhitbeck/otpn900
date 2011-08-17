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


#include "config.h"
#include "pindialog.h"
#include "util.h"
#include "hildonize.h"

#define GENERAL_BACKSPACE "/usr/share/icons/hicolor/48x48/hildon/general_backspace.png"
#define DISPLAY_FONT_SIZE 30

static GtkDialogClass *parent_class = NULL;

static void pin_dialog_class_init ( PinDialogClass *klass );
static void pin_dialog_init ( PinDialog *self );
static void return_if_ok ( GtkWidget *widget, PinDialog *self );
static GtkWidget* buildKeyPad ( PinDialog *self );
static GtkWidget* buildBackKey ( PinDialog *self );
static GtkWidget* buildKey ( PinDialog *self, const gchar *number, const gchar *letters );


G_DEFINE_TYPE (PinDialog, pin_dialog, GTK_TYPE_DIALOG)


static void
pin_dialog_class_init ( PinDialogClass *klass )
{
  parent_class = gtk_type_class (gtk_widget_get_type ());
}


static void
pin_dialog_init ( PinDialog *self )
{
  GtkDialog *dialog;
  GtkWidget *ok;

  dialog = GTK_DIALOG (self);

  self->pin_size = DEFAULT_PIN_SIZE;

  /* Content area */
  self->display = hildon_entry_new (HILDON_SIZE_FINGER_HEIGHT);
  gtk_editable_set_editable( GTK_EDITABLE( self->display ), FALSE ) ;
  gtk_entry_set_alignment( GTK_ENTRY( self->display ), CENTER );
  set_widget_font_size ( self->display, DISPLAY_FONT_SIZE );
  gtk_widget_set_sensitive ( self->display, FALSE );
  gtk_entry_set_visibility ( GTK_ENTRY(self->display), FALSE );

  gtk_container_add ( GTK_CONTAINER(dialog->vbox), self->display );
  gtk_container_add ( GTK_CONTAINER(dialog->vbox), buildKeyPad(self) );
  gtk_widget_show_all( GTK_WIDGET(dialog->vbox) );

  /* Action area */
  ok = gtk_button_new_from_stock ( GTK_STOCK_OK );
  HILDONIZE_DIALOG_BUTTON(ok);
  g_signal_connect ( ok, "clicked", G_CALLBACK (return_if_ok), self );
  gtk_box_pack_end ( GTK_BOX (dialog->action_area), ok, FALSE, TRUE, 0);
  gtk_widget_show ( ok );
}


GtkWidget* 
pin_dialog_new ( GtkWindow *parent, const gchar *name, guint pin_size )
{  
  GtkWidget *pin_dialog;
  PinDialog *self;
  gchar *title;

  pin_dialog = g_object_new ( TYPE_PIN_DIALOG, NULL );
  self = PIN_DIALOG(pin_dialog);

  title = g_strdup_printf ( "Pin code for %s", name );
  gtk_window_set_title ( GTK_WINDOW (self), title );

  self->pin_size = pin_size;
  gtk_entry_set_max_length ( GTK_ENTRY(self->display), pin_size );

  return pin_dialog;
}


static void
backspace ( HildonButton *button, PinDialog *self )
{
  gint pos;
  pos = gtk_editable_get_position ( GTK_EDITABLE(self->display) );
  if ( pos > 0 )
    gtk_editable_delete_text ( GTK_EDITABLE(self->display), pos-1, pos );
}


static void 
append_char ( HildonButton *button, PinDialog *self )
{
  const gchar *value;

  value = hildon_button_get_title( button );
  gtk_entry_append_text ( GTK_ENTRY(self->display), value );
  gtk_entry_set_position ( GTK_ENTRY(self->display), -1 ); // move cursor to end
}


static GtkWidget* 
buildKey ( PinDialog *self, const gchar *number, const gchar *letters ) 
{
  GtkWidget *key;
  key = hildon_button_new_with_text ( HILDON_SIZE_THUMB_HEIGHT,
				      HILDON_BUTTON_ARRANGEMENT_VERTICAL,
				      number,
				      letters);
  hildon_button_set_title_alignment ( HILDON_BUTTON(key), CENTER, BOTTOM );
  hildon_button_set_value_alignment ( HILDON_BUTTON(key), CENTER, TOP );
  hildon_helper_set_logical_font ( key, "LargeSystemFont" );
  g_signal_connect ( key, "clicked", G_CALLBACK (append_char), self );

  return key;
}


static GtkWidget* 
buildBackKey ( PinDialog *self )
{
  GtkWidget *back;
  GtkWidget *img;
  back = hildon_button_new ( HILDON_SIZE_THUMB_HEIGHT,
			     HILDON_BUTTON_ARRANGEMENT_VERTICAL );
  img = gtk_image_new_from_file ( GENERAL_BACKSPACE );
  hildon_button_set_image ( HILDON_BUTTON(back), img );
  g_signal_connect ( back, "clicked", G_CALLBACK (backspace), self );
  return back;
}


#define XKEY(OBJ,ROW,COL) gtk_table_attach_defaults( GTK_TABLE(pad), OBJ, COL, COL+1, ROW, ROW+1)
#define KEY(N,LABEL,ROW,COL) XKEY( buildKey(self, N, LABEL), ROW, COL )
static GtkWidget* 
buildKeyPad ( PinDialog *self ) 
{
  GtkWidget *pad;
  GtkWidget *back;
  
  back = buildBackKey(self);
  pad = gtk_table_new( 3, 4, TRUE); // rows, cols
  KEY("1", "  ",   0, 0 ); KEY("2", "abc", 0, 1 ); KEY("3", "def",  0, 2); XKEY(back,0,3);
  KEY("4", "ghi",  1, 0 ); KEY("5", "jkl", 1, 1 ); KEY("6", "mno",  1, 2);
  KEY("7", "pqrs", 2, 0 ); KEY("8", "tuv", 2, 1 ); KEY("9", "wxyz", 2, 2); KEY("0", "  ", 2,3);

  return pad;
}


static void
return_if_ok ( GtkWidget *widget, PinDialog *self )
{
  gint n_chars;

  n_chars = gtk_entry_get_text_length ( GTK_ENTRY(self->display) );
  if ( n_chars == self->pin_size ){
    gtk_dialog_response ( GTK_DIALOG(self), GTK_RESPONSE_OK );
  } else {
    warn ( GTK_WIDGET( self ), "Pin must be %d digits long", self->pin_size );
  }
}


const gchar* 
pin_dialog_get_pin ( PinDialog *self )
{
  return gtk_entry_get_text( GTK_ENTRY ( self->display ) );
}


void
pin_dialog_clear_pin ( PinDialog *self )
{
  gtk_entry_set_text ( GTK_ENTRY ( self->display ), "" );
}
