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


#ifndef ET_DIALOG_H
#define ET_DIALOG_H

#include <hildon/hildon.h>
#include "otp.h"


#define TYPE_ET_DIALOG              (et_dialog_get_type ())
#define ET_DIALOG(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_ET_DIALOG, EtDialog))
#define ET_DIALOG_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_ET_DIALOG, EtDialogClass))
#define IS_ET_DIALOG(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_ET_DIALOG))
#define IS_ET_DIALOG_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_ET_DIALOG))
#define ET_DIALOG_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_ET_DIALOG, EtDialogClass))

typedef struct _EtDialog EtDialog;
typedef struct _EtDialogClass EtDialogClass;

struct _EtDialog
{
  GtkDialog dialog;
  
  Token *token;

  GtkWidget *key;
  GtkWidget *description;
  GtkWidget *disp_hex;
  GtkWidget *pin_size;
  GtkWidget *digits;
  GtkWidget *counter;
  GtkWidget *interval;
};


struct _EtDialogClass
{
  GtkDialogClass parent_class;
};


GtkWidget* et_dialog_new ( GtkWindow *parent, Token *token );
GType et_dialog_get_type ( void );

#endif
