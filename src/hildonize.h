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


#ifndef HILDONIZE_H
#define HILDONIZE_H

/* This is code lifted from the maemo modifications of gtkdialog.c */

/* Buttons on a HildonDialog have fixed size */
#define HILDON_DIALOG_BUTTON_WIDTH 174

#define HILDONIZE_DIALOG_BUTTON(button)					\
  hildon_gtk_widget_set_theme_size ( button, HILDON_SIZE_AUTO_WIDTH	\
				     | HILDON_SIZE_FINGER_HEIGHT);	\
  g_object_set (button, "width-request", HILDON_DIALOG_BUTTON_WIDTH, NULL)

#endif
