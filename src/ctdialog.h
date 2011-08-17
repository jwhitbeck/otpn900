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


#ifndef CT_DIALOG_H
#define CT_DIALOG_H

#include <hildon/hildon.h>
#include "otp.h"
#include "tokendb.h"


#define TYPE_CT_DIALOG              (ct_dialog_get_type ())
#define CT_DIALOG(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_CT_DIALOG, CtDialog))
#define CT_DIALOG_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_CT_DIALOG, CtDialogClass))
#define IS_CT_DIALOG(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_CT_DIALOG))
#define IS_CT_DIALOG_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_CT_DIALOG))
#define CT_DIALOG_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_CT_DIALOG, CtDialogClass))

typedef struct _CtDialog CtDialog;
typedef struct _CtDialogClass CtDialogClass;

struct _CtDialog
{
  GtkDialog dialog;
  
  TokenDB *token_db;

  GtkWidget *name;
  GtkWidget *algorithm;
  GtkWidget *counter_t;
  GtkWidget *key;

  Token *token;
};

struct _CtDialogClass
{
  GtkDialogClass parent_class;
};

GtkWidget* ct_dialog_new ( GtkWindow *parent, TokenDB *token_db );
GType ct_dialog_get_type ( void );

#endif
