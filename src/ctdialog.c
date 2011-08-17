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
#include "ctdialog.h"
#include "util.h"
#include "hildonize.h"
#include "strconv.h"

#define TITLE "New Auth Token"

#define NAME_PLACEHOLDER "Enter name"
#define KEY_PLACEHOLDER  "Enter key or generate a new one"

#define NAME_LABEL      "Name "
#define KEY_LABEL       "Key "

#define DEFAULT_ALGORITHM HOTP
#define DEFAULT_COUNTER_T EVENT


static GtkDialogClass *parent_class = NULL;

static void ct_dialog_class_init ( CtDialogClass *klass );
static void ct_dialog_init ( CtDialog *self );
static void gen_key ( GtkWidget *widget, CtDialog *self );
static void return_if_ok ( GtkWidget *widget, CtDialog *self );
static void change_algorithm ( GtkWidget *widget, CtDialog *self);
static void init_token ( CtDialog *self, GError **err );



G_DEFINE_TYPE (CtDialog, ct_dialog, GTK_TYPE_DIALOG)


static void
ct_dialog_class_init ( CtDialogClass *klass )
{
  parent_class = gtk_type_class (gtk_widget_get_type ());
}

#define NEW_BOX box = gtk_hbox_new ( FALSE, 0 ); // homogenous, spacing
#define APPEND(obj)							\
  gtk_container_add ( GTK_CONTAINER(dialog->vbox), GTK_WIDGET(obj) );	\
  gtk_widget_show_all ( obj );
static void
ct_dialog_init ( CtDialog *self )
{
  GtkDialog *dialog = GTK_DIALOG (self);
  HildonTouchSelector *selector;
  GtkWidget *generate;
  gint i;
  GtkWidget *ok;
  GtkWidget *box;

  /* TITLE */
  gtk_window_set_title ( GTK_WINDOW (self), TITLE );


  /* CONTENT AREA */
  //table = GTK_TABLE ( gtk_table_new ( 4, 2, FALSE ) ); // rows, cols, homogenous

  /* Enter name */
  NEW_BOX;
  gtk_box_pack_start ( GTK_BOX(box), gtk_label_new (NAME_LABEL), FALSE, TRUE, 0 );

  self->name = hildon_entry_new (HILDON_SIZE_HALFSCREEN_WIDTH);
  hildon_entry_set_placeholder ( HILDON_ENTRY(self->name), NAME_PLACEHOLDER );
  gtk_box_pack_start ( GTK_BOX(box), self->name, TRUE, TRUE, 0 );
  APPEND(box);
  
  /* Choose algorithm */
  self->algorithm = hildon_picker_button_new ( HILDON_SIZE_HALFSCREEN_WIDTH | HILDON_SIZE_FINGER_HEIGHT, 
					       HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);

  selector = HILDON_TOUCH_SELECTOR( hildon_touch_selector_new_text () );
  for ( i=0; i < N_OTP_ALGORITHMS ; ++i ){
    hildon_touch_selector_append_text ( selector, algorithm_labels[i] );
  }
  hildon_picker_button_set_selector ( HILDON_PICKER_BUTTON(self->algorithm), selector );
  hildon_picker_button_set_active ( HILDON_PICKER_BUTTON(self->algorithm), DEFAULT_ALGORITHM );
  hildon_button_set_title ( HILDON_BUTTON(self->algorithm), "Algorithm" );
  g_signal_connect ( self->algorithm, "value_changed", G_CALLBACK(change_algorithm), self );
  APPEND(self->algorithm);
  
  /* Choose algorithm */
  self->counter_t = hildon_picker_button_new ( HILDON_SIZE_HALFSCREEN_WIDTH | HILDON_SIZE_FINGER_HEIGHT, 
					       HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);

  selector = HILDON_TOUCH_SELECTOR( hildon_touch_selector_new_text () );
  for ( i=0; i < N_OTP_COUNTER_TYPES ; ++i ){
    hildon_touch_selector_append_text ( selector, counter_type_labels[i] );
  }
  hildon_picker_button_set_selector ( HILDON_PICKER_BUTTON(self->counter_t), selector );
  hildon_picker_button_set_active ( HILDON_PICKER_BUTTON(self->counter_t), DEFAULT_COUNTER_T );
  hildon_button_set_title ( HILDON_BUTTON(self->counter_t), "Counter type" );
  APPEND(self->counter_t);


  /* Key */
  NEW_BOX;
  gtk_box_pack_start ( GTK_BOX(box), gtk_label_new (KEY_LABEL), FALSE, TRUE, 0 );

  self->key = hildon_entry_new (HILDON_SIZE_HALFSCREEN_WIDTH);
  hildon_entry_set_placeholder ( HILDON_ENTRY(self->key), KEY_PLACEHOLDER );
  gtk_box_pack_start ( GTK_BOX(box), self->key, TRUE, TRUE, 0 );
  APPEND(box);
  

  /* Generate button */
  generate = hildon_button_new_with_text( HILDON_SIZE_FINGER_HEIGHT,
					  HILDON_BUTTON_ARRANGEMENT_VERTICAL,
					  "Generate new key", "" );
  hildon_button_set_title_alignment ( HILDON_BUTTON(generate), CENTER, BOTTOM );
  hildon_button_set_value_alignment ( HILDON_BUTTON(generate), CENTER, TOP );
  g_signal_connect ( generate, "clicked", G_CALLBACK (gen_key), self );  
  APPEND(generate);

  /* ACTION AREA */
  ok = gtk_button_new_from_stock ( GTK_STOCK_OK );
  HILDONIZE_DIALOG_BUTTON(ok);
  g_signal_connect ( ok, "clicked", G_CALLBACK (return_if_ok), self );
  gtk_box_pack_end (GTK_BOX (dialog->action_area), ok, FALSE, TRUE, 0);
  gtk_widget_show ( ok );

}


GtkWidget* 
ct_dialog_new ( GtkWindow *parent, TokenDB *token_db )
{
  GtkWidget *self;
  self = g_object_new ( TYPE_CT_DIALOG, NULL );
  CT_DIALOG(self)->token_db = token_db;
  return self;
}


static void 
init_token ( CtDialog *self, GError **err )
{
  const gchar *name;
  const gchar *key_str;
  Algorithm algorithm;
  CounterType counter_t;

  name = g_strdup ( gtk_entry_get_text ( GTK_ENTRY( self->name ) ) );

  self->token = token_new ( name );
  
  algorithm = hildon_picker_button_get_active ( HILDON_PICKER_BUTTON ( self->algorithm ) );
  token_set_algorithm ( self->token, algorithm_labels[algorithm], NULL );

  counter_t = hildon_picker_button_get_active ( HILDON_PICKER_BUTTON ( self->counter_t ) );
  self->token->counter_t = counter_t;
  
  key_str =  gtk_entry_get_text ( GTK_ENTRY( self->key ) );
  token_set_key ( self->token, key_str, NULL );
}


static void 
gen_key ( GtkWidget *widget, CtDialog *self )
{
  const gchar *key;
  Algorithm algorithm;

  algorithm = hildon_picker_button_get_active ( HILDON_PICKER_BUTTON ( self->algorithm ) );
  key = token_key_new_as_str ( algorithm );
  gtk_entry_set_text ( GTK_ENTRY( self->key ), key );
}


static void
return_if_ok ( GtkWidget *widget, CtDialog *self )
{
  guint16 l_key, l_name;
  const gchar *name;
  GError *err;
  
  name = gtk_entry_get_text ( GTK_ENTRY( self->name ) );
  l_name = gtk_entry_get_text_length ( GTK_ENTRY( self->name ) );
  l_key = gtk_entry_get_text_length ( GTK_ENTRY( self->key ) );

  if ( ! (l_name > 0 && l_key > 0) ){
    warn ( GTK_WIDGET( self ), "A name and a key are required" );
  } else if ( token_db_has_token ( self->token_db, name ) ){ 
    warn ( GTK_WIDGET(self), "%s already exists. ", name );
  } else {
    err = NULL;
    init_token ( self, &err );
    if ( err ){
      warn ( GTK_WIDGET(self), g_strdup( err->message ));
      g_error_free ( err );
    } else {
      gtk_dialog_response ( GTK_DIALOG(self), GTK_RESPONSE_OK );
    }
  }
}


static void
change_algorithm ( GtkWidget *widget, CtDialog *self )
{
  Algorithm algorithm;

  algorithm = hildon_picker_button_get_active ( HILDON_PICKER_BUTTON ( self->algorithm ) );
  
  switch ( algorithm )
    {
    case HOTP:
      hildon_picker_button_set_active ( HILDON_PICKER_BUTTON ( self->counter_t ), EVENT ); break;

    case MOTP:
      hildon_picker_button_set_active ( HILDON_PICKER_BUTTON ( self->counter_t ), TIME ); break;

    default: ;
    }
}
