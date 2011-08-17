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


#include "tokenselector.h"
#include "ctdialog.h"
#include "tokenview.h"
#include "util.h"


#define TOKEN_SELECTOR_TITLE "OTP"


static HildonStackableWindowClass *parent_class = NULL;

static void token_selector_class_init ( TokenSelectorClass *klass );
static void token_selector_init ( TokenSelector *self );
static void init_menu ( TokenSelector *self );
static void display_about ( GtkWidget *parent, TokenSelector *self );
static void handle_token ( GtkWidget *parent, TokenSelector *self );
static void delete_tokens ( GtkWidget *parent, TokenSelector *self );
static void create_token ( GtkWidget *parent, TokenSelector *self );
static void select_token ( HildonTouchSelector *selector, gint column, TokenSelector *self );
static void init_touch_selector ( TokenSelector *self );
static void delete_button_clicked ( GtkWidget *parent, TokenSelector *self );

G_DEFINE_TYPE (TokenSelector, token_selector, HILDON_TYPE_STACKABLE_WINDOW)


static void
token_selector_class_init ( TokenSelectorClass *klass )
{
  parent_class = gtk_type_class (gtk_widget_get_type ());
}


static void
token_selector_init ( TokenSelector *self )
{
  /* set window title */
  gtk_window_set_title ( GTK_WINDOW( self), TOKEN_SELECTOR_TITLE );
  
  /* create menu */
  init_menu ( self );

  /* create selector */
  self->selector = HILDON_TOUCH_SELECTOR( hildon_touch_selector_new_text () );
  g_signal_connect (G_OBJECT (self->selector), "changed",
		    G_CALLBACK (select_token), self);
  gtk_container_add ( GTK_CONTAINER(self),
		      GTK_WIDGET(self->selector) );
}


static void 
create_token ( GtkWidget *widget, TokenSelector *self ){
  Token *token;
  GtkWidget *ctd;
  gint result;

  ctd = ct_dialog_new ( GTK_WINDOW(self), self->token_db );
  result = gtk_dialog_run ( GTK_DIALOG(ctd) );
  if ( result == GTK_RESPONSE_OK ) {
    token = CT_DIALOG(ctd)->token ;
    token_db_add_token ( self->token_db, token );
    hildon_touch_selector_append_text ( self->selector, token->name );
    token_free ( token );
  } 
  gtk_widget_destroy ( GTK_WIDGET(ctd) );
}


static void 
display_about ( GtkWidget *parent, TokenSelector *self )
{
  about ( GTK_WINDOW ( self ) );
}


static void 
select_token ( HildonTouchSelector *selector, gint column, TokenSelector *self )
{
  const gchar *name;
  GtkWidget *token_view;
  Token *token;
  GError *err;

  name = hildon_touch_selector_get_current_text ( selector );
  if ( name ){
    err = NULL;
    token = token_db_get_token ( self->token_db, name, &err );
    if ( err ){
      alert ( GTK_WIDGET(self), err->message );
      g_error_free ( err );
    } else {
      token_view = token_view_new ( token );
      g_signal_connect_after ( G_OBJECT(token_view), "destroy", G_CALLBACK(handle_token), self );
      gtk_widget_show_all ( token_view );
    }
  }
}


static void 
init_menu ( TokenSelector *self )
{
  GtkWidget *menu;
  GtkWidget *new_button;
  GtkWidget *about_button;
  GtkWidget *delete_button;
  
  menu = hildon_app_menu_new();

  /* New button */
  new_button = gtk_button_new_with_label ( "Create New Token" );
  g_signal_connect_after ( new_button, "clicked", 
			   G_CALLBACK(create_token),
			   self );
  hildon_app_menu_append ( HILDON_APP_MENU(menu), GTK_BUTTON(new_button) );

  /* Delete button */
  delete_button = gtk_button_new_with_label ( "Delete Tokens" );
  g_signal_connect_after ( delete_button, "clicked", 
			   G_CALLBACK(delete_tokens),
			   self );
  hildon_app_menu_append ( HILDON_APP_MENU(menu), GTK_BUTTON(delete_button) );


  /* About button */
  about_button = gtk_button_new_with_label ( "About" );
  g_signal_connect_after ( about_button, "clicked", 
			   G_CALLBACK(display_about),
			   self );
  hildon_app_menu_append ( HILDON_APP_MENU(menu), GTK_BUTTON(about_button) );

  hildon_window_set_app_menu ( HILDON_WINDOW(self),
			       HILDON_APP_MENU(menu) );

  gtk_widget_show_all ( menu );
}


static void 
handle_token ( GtkWidget *parent, TokenSelector *self )
{
  Token *token;

  token = TOKEN_VIEW(parent)->token;
  if ( token->modified ){
    token_db_add_token ( self->token_db, token );
  }
  token_free ( token );
}


GtkWidget* 
token_selector_new ( TokenDB *token_db )
{
  GtkWidget *self;
  self = g_object_new ( TYPE_TOKEN_SELECTOR, NULL );

  TOKEN_SELECTOR(self)->token_db = token_db;
  init_touch_selector ( TOKEN_SELECTOR(self) );

  return self;
}


static void
delete_tokens ( GtkWidget *parent, TokenSelector *self )
{
  GtkWidget *toolbar;
  HildonTouchSelector *to_delete;
  GtkTreeModel *model;

  /* Block selection signals */
  g_signal_handlers_block_by_func ( self->selector, G_CALLBACK(select_token), self );

  model = hildon_touch_selector_get_model ( self->selector, 0 );

  self->delete_window = hildon_stackable_window_new ();
  
  /* create selector with multiple selection mode */
  to_delete = HILDON_TOUCH_SELECTOR ( hildon_touch_selector_new_text () );
  hildon_touch_selector_set_model ( to_delete, 0,  model );
  hildon_touch_selector_set_hildon_ui_mode ( to_delete, HILDON_UI_MODE_EDIT );
  hildon_touch_selector_set_column_selection_mode ( to_delete, HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE );
  hildon_touch_selector_unselect_all ( to_delete , 0);
  
  /* toolbar */
  toolbar = hildon_edit_toolbar_new_with_text ( "Choose tokens to delete", "Delete" );
  hildon_window_set_edit_toolbar ( HILDON_WINDOW ( self->delete_window ), 
				   HILDON_EDIT_TOOLBAR ( toolbar ) );

  /* signals */
  g_signal_connect ( toolbar, "button-clicked", G_CALLBACK ( delete_button_clicked ), self );
  g_signal_connect_swapped ( toolbar, "arrow-clicked", G_CALLBACK ( gtk_widget_destroy ), self->delete_window );
		     

  gtk_container_add ( GTK_CONTAINER ( self->delete_window ), GTK_WIDGET ( to_delete ) );
  gtk_widget_show_all ( self->delete_window );
  gtk_window_fullscreen ( GTK_WINDOW ( self->delete_window ) );
}


static void
init_touch_selector ( TokenSelector *self )
{
  GtkTreeModel *model;
  gsize length, i;
  gchar **names;

  model = hildon_touch_selector_get_model( self->selector, 0 );

  names = token_db_get_names ( self->token_db , &length );
  for ( i=0; i < length; ++i ) {
    hildon_touch_selector_append_text ( self->selector, names[i] );
  }
  g_strfreev ( names );

}


static void
delete_button_clicked ( GtkWidget *parent, TokenSelector *self )
{
  GList *selected_rows, *list, *row_references;
  GtkTreePath *path;
  GtkTreeRowReference *ref;
  GtkTreeModel *model;
  GtkTreeIter iter;
  HildonTouchSelector *to_delete;
  const gchar *name;
  
  to_delete = HILDON_TOUCH_SELECTOR ( gtk_bin_get_child ( GTK_BIN(self->delete_window) ) );
  model = hildon_touch_selector_get_model ( to_delete, 0 );
  selected_rows = hildon_touch_selector_get_selected_rows ( to_delete, 0 );
  
  /* get list of rows to delete */
  row_references = NULL;
  for ( list = selected_rows; list; list = g_list_next ( list ) ){
    path = list->data;
    ref = gtk_tree_row_reference_new ( model, path );
    row_references = g_list_append ( row_references, ref );
  }

  if ( confirm ( GTK_WIDGET( self ), "Delete selected tokens?" ) ){
      /* remove */
      for ( list = row_references; list; list = g_list_next ( list ) ){
	path = gtk_tree_row_reference_get_path ( (GtkTreeRowReference *) (list->data) );
	gtk_tree_model_get_iter ( model, &iter, path );
	gtk_tree_model_get ( GTK_TREE_MODEL ( model ), &iter, 0, &name, -1 );
	token_db_del_token ( self->token_db, name ); // from TokenDB
	gtk_list_store_remove ( GTK_LIST_STORE (model), &iter ); // from model
	gtk_tree_path_free ( path );
      }
  }

  /* Clean up */
  g_list_free ( selected_rows );
  g_list_free ( row_references );
  g_list_free ( list );

  gtk_widget_destroy ( self->delete_window );

  /* Reenable selection signals */
  hildon_touch_selector_set_active ( self->selector, 0, -1 );
  g_signal_handlers_unblock_by_func ( self->selector, G_CALLBACK(select_token), self );


}
