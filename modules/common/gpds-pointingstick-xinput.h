/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009 Hiroyuki Ikezoe  <poincare@ikezoe.net>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __GPDS_POINTINGSTICK_XINPUT_H__
#define __GPDS_POINTINGSTICK_XINPUT_H__

#include <glib.h>
#include <gpds-xinput.h>

typedef enum {
    GPDS_POINTINGSTICK_SENSITIVITY,
    GPDS_POINTINGSTICK_SPEED,
    GPDS_POINTINGSTICK_SCROLLING,
    GPDS_POINTINGSTICK_MIDDLE_BUTTON_TIMEOUT,
    GPDS_POINTINGSTICK_PRESS_TO_SELECT,
    GPDS_POINTINGSTICK_PRESS_TO_SELECT_THRESHOLD,
} GpdsPointingStickProperty;

GpdsXInput *gpds_pointingstick_xinput_new                    (const gchar *device_name);
void        gpds_pointingstick_xinput_setup_property_entries (GpdsXInput *xinput);

#endif /* __GPDS_POINTINGSTICK_XINPUT_H__ */
/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
