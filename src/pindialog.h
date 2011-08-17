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


#ifndef PIN_DIALOG_H
#define PIN_DIALOG_H

#include <hildon/hildon.h>

#define TYPE_PIN_DIALOG              (pin_dialog_get_type ())
#define PIN_DIALOG(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_PIN_DIALOG, PinDialog))
#define PIN_DIALOG_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_PIN_DIALOG, PinDialogClass))
#define IS_PIN_DIALOG(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_PIN_DIALOG))
#define IS_PIN_DIALOG_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_PIN_DIALOG))
#define PIN_DIALOG_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_PIN_DIALOG, PinDialogClass))

typedef struct _PinDialog PinDialog;
typedef struct _PinDialogClass PinDialogClass;

struct _PinDialog
{
  GtkDialog dialog;
  GtkWidget *display;
  guint pin_size;
};

struct _PinDialogClass
{
  GtkDialogClass parent_class;
};

GtkWidget* pin_dialog_new ( GtkWindow *parent, const gchar *name, guint pin_size );
GType pin_dialog_get_type ( void );
const gchar* pin_dialog_get_pin ( PinDialog *self );
void pin_dialog_clear_pin ( PinDialog *self );

#endif
