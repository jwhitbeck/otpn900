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


#ifndef TOKEN_VIEW_H
#define TOKEN_VIEW_H

#include <hildon/hildon.h>
#include "otp.h"

#define TYPE_TOKEN_VIEW                  (token_view_get_type ())
#define TOKEN_VIEW(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TOKEN_VIEW, TokenView))
#define TOKEN_VIEW_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TOKEN_VIEW, TokenViewClass))
#define IS_TOKEN_VIEW(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TOKEN_VIEW))
#define IS_TOKEN_VIEW_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TOKEN_VIEW))
#define TOKEN_VIEW_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TOKEN_VIEW, TokenViewClass))

typedef struct _TokenView TokenView;
typedef struct _TokenViewClass TokenViewClass;

struct _TokenView
{
  HildonStackableWindow window;

  Token *token;

  GtkWidget *header;
  GtkWidget *display;

  GtkWidget *pin_dialog;
};

struct _TokenViewClass
{
  HildonStackableWindowClass parent_class;
};


GtkWidget* token_view_new ( Token *token );
GType token_view_get_type ( void );

#endif
