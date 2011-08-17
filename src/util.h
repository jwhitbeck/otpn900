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


#ifndef UTIL_H
#define UTIL_H

#include <hildon/hildon.h>

/* Alignements for hildon buttons */
#define CENTER 0.5
#define TOP 0.0
#define BOTTOM 1.0
#define LEFT 0.0
#define RIGHT 1.0

void warn ( GtkWidget *parent, const gchar *fmt, ... );
void alert ( GtkWidget *parent, const gchar *fmt, ... );
gboolean confirm ( GtkWidget *parent, const gchar *fmt, ... );
void about ( GtkWindow *parent ) ;

void set_widget_font_size ( GtkWidget *widget, guint points );

#endif
