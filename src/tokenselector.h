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


#ifndef TOKEN_SELECTOR_H
#define TOKEN_SELECTOR_H

#include <hildon/hildon.h>
#include "tokendb.h"

#define TYPE_TOKEN_SELECTOR                  (token_selector_get_type ())
#define TOKEN_SELECTOR(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TOKEN_SELECTOR, TokenSelector))
#define TOKEN_SELECTOR_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TOKEN_SELECTOR, TokenSelectorClass))
#define IS_TOKEN_SELECTOR(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TOKEN_SELECTOR))
#define IS_TOKEN_SELECTOR_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TOKEN_SELECTOR))
#define TOKEN_SELECTOR_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TOKEN_SELECTOR, TokenSelectorClass))

typedef struct _TokenSelector TokenSelector;
typedef struct _TokenSelectorClass TokenSelectorClass;

struct _TokenSelector
{
  HildonStackableWindow window;

  TokenDB *token_db;

  HildonTouchSelector *selector;
  GtkWidget *delete_window;
};

struct _TokenSelectorClass
{
  HildonStackableWindowClass parent_class;
};


GtkWidget* token_selector_new ( TokenDB *token_db );
GType token_selector_get_type ( void );

#endif
