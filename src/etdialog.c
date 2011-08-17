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
#include "etdialog.h"
#include "util.h"
#include "hildonize.h"

static GtkDialogClass *parent_class = NULL;

static void et_dialog_class_init ( EtDialogClass *klass );
static void et_dialog_init ( EtDialog *self );
static GtkWidget* integer_selector_new ( guint min, guint max );
static void return_if_ok ( GtkWidget *widget, EtDialog *self );


G_DEFINE_TYPE (EtDialog, et_dialog, GTK_TYPE_DIALOG)


static void
et_dialog_class_init ( EtDialogClass *klass )
{
  parent_class = gtk_type_class (gtk_widget_get_type ());
}


#define LABELED_ENTRY(LABEL,VAR)					\
  box = gtk_hbox_new ( FALSE, 0 );					\
  gtk_box_pack_start ( GTK_BOX(box), gtk_label_new (LABEL), FALSE, TRUE, 0 ); \
  VAR = hildon_entry_new ( HILDON_SIZE_AUTO ) ;				\
  gtk_box_pack_start ( GTK_BOX(box), VAR, TRUE, TRUE, 0 );		\
  APPEND(box)

#define APPEND(obj)							\
  gtk_container_add ( GTK_CONTAINER(dialog->vbox), GTK_WIDGET(obj) );	\
  gtk_widget_show_all ( obj );

static void
et_dialog_init ( EtDialog *self )
{
  GtkDialog *dialog;
  GtkWidget *box;
  GtkWidget *ok;

  dialog = GTK_DIALOG (self);

  /* CONTENT AREA */

  /* Description */
  self->description = gtk_label_new ( NULL );
  APPEND ( self->description );
  
  /* Key */
  LABELED_ENTRY ( "Key ", self->key );
  //  hildon_gtk_entry_set_input_mode ( GTK_ENTRY ( self->key ), 
  //			    HILDON_GTK_INPUT_MODE_HEXA );

  /* initialize the rest to NULL */
  self->disp_hex = NULL;
  self->counter = NULL;
  self->interval = NULL;
  self->pin_size = NULL;
  self->digits = NULL;

  /* ACTION AREA */
  ok = gtk_button_new_from_stock ( GTK_STOCK_OK );
  HILDONIZE_DIALOG_BUTTON(ok);
  g_signal_connect ( ok, "clicked", G_CALLBACK (return_if_ok), self );
  gtk_box_pack_end (GTK_BOX (dialog->action_area), ok, FALSE, TRUE, 0);
  gtk_widget_show ( ok );
}



static GtkWidget*
integer_selector_new ( guint min, guint max )
{
  GtkWidget *button;
  HildonTouchSelector *selector;
  guint i;
  gchar label[3];

  button =hildon_picker_button_new ( HILDON_SIZE_FINGER_HEIGHT, 
				     HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
  selector = HILDON_TOUCH_SELECTOR ( hildon_touch_selector_new_text () );
  for ( i=min; i<=max; ++i ){
    g_snprintf ( label , 3 , "%d", i );
    hildon_touch_selector_append_text ( selector, label );
  }
  hildon_picker_button_set_selector ( HILDON_PICKER_BUTTON(button), selector );
  
  return button;
  
}


GtkWidget* 
et_dialog_new ( GtkWindow *parent, Token *token )
{
  GtkWidget *et_dialog;
  EtDialog *self;
  const gchar *description;
  GtkDialog *dialog;
  GtkWidget *box;


  et_dialog = g_object_new ( TYPE_ET_DIALOG, NULL );
  self = ET_DIALOG(et_dialog);
  dialog = GTK_DIALOG (self);
  
  self->token = token;

  /* title */
  gtk_window_set_title ( GTK_WINDOW (self), token->name );
  
  /* description */
  description = g_strjoin ( " ", algorithm_labels[token->algorithm],
			    counter_type_labels[token->counter_t],
			    "token", NULL );
  gtk_label_set_text ( GTK_LABEL ( self->description ), description );
  
  /* key */
  gtk_entry_set_text ( GTK_ENTRY( self->key ), token_get_key_as_str ( token ) );

  /* Algorithm specific parameters */
  switch ( self->token->algorithm )
    {
    case HOTP:
      switch ( self->token->counter_t )
	{
	case EVENT: 
	  LABELED_ENTRY ( "Counter value", self->counter );  
	  gtk_entry_set_text ( GTK_ENTRY ( self->counter ), 
			       g_strdup_printf("%lu", (gulong)token->counter ) );
	  hildon_gtk_entry_set_input_mode ( GTK_ENTRY ( self->counter ), 
					    HILDON_GTK_INPUT_MODE_NUMERIC );
	  break;
	  
	case TIME:  
	  LABELED_ENTRY ( "Interval (seconds) ", self->interval);
	  gtk_entry_set_text ( GTK_ENTRY ( self->interval ), 
			       g_strdup_printf("%u", token->interval ) );
	  hildon_gtk_entry_set_input_mode ( GTK_ENTRY ( self->interval ),
					    HILDON_GTK_INPUT_MODE_NUMERIC );
	  break;

	default:;
	}
      /* disp hex */
      self->disp_hex = hildon_check_button_new ( HILDON_SIZE_FINGER_HEIGHT );
      gtk_button_set_label ( GTK_BUTTON ( self->disp_hex ), "Display Hexadecimal" );
      hildon_check_button_set_active ( HILDON_CHECK_BUTTON(self->disp_hex), token->disp_hex );
      APPEND ( self->disp_hex );
      /* Digits */
      self->digits = integer_selector_new ( MIN_DIGITS, MAX_DIGITS );
      hildon_button_set_title ( HILDON_BUTTON(self->digits), "Digits" );
      hildon_touch_selector_set_active ( hildon_picker_button_get_selector ( HILDON_PICKER_BUTTON(self->digits) ),
					 0, (token->digits - MIN_DIGITS) );
      APPEND ( self->digits );  
      break;
      
    case MOTP:
      if ( self->token->counter_t == EVENT ){
	LABELED_ENTRY ( "Counter", self->counter );
	gtk_entry_set_text ( GTK_ENTRY ( self->counter ), 
			     g_strdup_printf("%lu", (gulong)token->counter ) );
      }
      self->pin_size = integer_selector_new ( MIN_PIN_SIZE, MAX_PIN_SIZE );
      hildon_button_set_title ( HILDON_BUTTON(self->pin_size), "PIN length" );
      hildon_touch_selector_set_active ( hildon_picker_button_get_selector ( HILDON_PICKER_BUTTON(self->pin_size) ),
					 0, (token->pin_size - MIN_PIN_SIZE) );
      APPEND ( self->pin_size );
      break;

    default:;
    }

  return et_dialog;
}


#define WARN_IF_ERROR				\
  if ( err ){					\
    warn ( GTK_WIDGET(self), err->message );	\
    g_error_free ( err );			\
    return;					\
  }

#define SET_IF_CHANGED(VAR)			\
  if ( VAR != token->VAR ){			\
    token->VAR = VAR;				\
    token->modified = TRUE;			\
  }

static void 
return_if_ok ( GtkWidget *widget, EtDialog *self )
{
  Token *token;
  const gchar *text;
  gboolean disp_hex;
  GError *err;
  guint pin_size, digits;

  token = self->token;
  err = NULL;

  /* update the intervals and counter */
  if ( self->counter ){
    text = gtk_entry_get_text ( GTK_ENTRY ( self->counter ) );
    token_set_counter ( token, text, &err );
    WARN_IF_ERROR;
  } else if ( self->interval ){
    text = gtk_entry_get_text ( GTK_ENTRY ( self->interval ) );
    token_set_interval ( token, text, &err );
    WARN_IF_ERROR;
  }
  
  /* Check if key has changed */
  text = gtk_entry_get_text ( GTK_ENTRY ( self->key ) );
  token_set_key ( token, text, &err );
  WARN_IF_ERROR;

  /* Check if digits changed */
  if ( self->digits ){
    digits = hildon_picker_button_get_active ( HILDON_PICKER_BUTTON(self->digits) );
    digits += MIN_DIGITS; // values start at MIN_DIGITS not 0
    token->digits = digits;
  }

  /* Display hex */
  if ( self->disp_hex ){
    disp_hex = hildon_check_button_get_active ( HILDON_CHECK_BUTTON(self->disp_hex) );
    token->disp_hex = disp_hex;
  }

  /* update pin size */
  if ( self->pin_size ){
    pin_size = hildon_picker_button_get_active ( HILDON_PICKER_BUTTON(self->pin_size) );
    pin_size += MIN_PIN_SIZE;
    if ( pin_size != token->pin_size ){ 
      token->pin_size = pin_size;
      token_set_pin ( token, NULL, NULL ); // if modified, clear pin
    }
  }

  token->modified = TRUE;
  
  gtk_dialog_response ( GTK_DIALOG(self), GTK_RESPONSE_OK );
}
