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


#include <stdarg.h>
#include "util.h"


void 
alert (GtkWidget *parent, const gchar *fmt, ... )
{
  GtkWidget *alert;
  const gchar* msg;
  va_list args;

  va_start ( args, fmt );
  msg = g_strdup_vprintf ( fmt, args );
  va_end ( args );

  alert = hildon_note_new_information ( GTK_WINDOW(parent), msg );
  gtk_dialog_run ( GTK_DIALOG( alert ) );
  gtk_widget_destroy ( alert ); 
}


void 
warn ( GtkWidget *parent, const gchar *fmt, ... )
{
  const gchar *msg;
  va_list args;

  va_start ( args, fmt );
  msg = g_strdup_vprintf ( fmt, args );
  va_end ( args );

  hildon_banner_show_information ( parent, NULL, msg );
}


gboolean
confirm ( GtkWidget *parent, const gchar *fmt, ... )
{
  GtkWidget *confirm;
  gboolean ok;
  gint result;
  va_list args;
  const gchar *msg;

  va_start ( args, fmt );
  msg = g_strdup_vprintf ( fmt, args );
  va_end ( args );

  confirm = hildon_note_new_confirmation ( GTK_WINDOW(parent), msg );
  result = gtk_dialog_run ( GTK_DIALOG ( confirm ) );
  ok = ( result == GTK_RESPONSE_OK )? TRUE : FALSE;
  gtk_widget_destroy ( confirm );
  return ok;
}


void 
about ( GtkWindow *parent ) 
{
  gtk_show_about_dialog ( parent, 
			  "program-name", PACKAGE,
			  "version", VERSION,
			  "copyright", "Sean Whitbeck",
			  "website", "http://otpn900.sourceforge.net/",
			  NULL );
}


void
set_widget_font_size ( GtkWidget *widget, guint points )
{
  GtkRcStyle *rc_style;

  rc_style = gtk_widget_get_modifier_style ( widget );
  rc_style->font_desc = pango_font_description_new ();
  pango_font_description_set_size ( rc_style->font_desc, points * PANGO_SCALE );
  gtk_widget_modify_style ( widget, rc_style );
}
