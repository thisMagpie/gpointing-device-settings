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

#ifndef __GPDS_GCONF_H__
#define __GPDS_GCONF_H__

#include <glib.h>
#include <gconf/gconf-client.h>

G_BEGIN_DECLS

#define GPDS_GCONF_DIR             "/desktop/gnome/peripherals"
#define GPDS_GCONF_DEVICE_TYPE_KEY "device_type"

const gchar *gpds_gconf_get_key_from_path (const gchar *path);

gboolean     gpds_gconf_get_boolean       (GConfClient *gconf,
                                           const gchar *key,
                                           gboolean *value);
gboolean     gpds_gconf_get_int           (GConfClient *gconf,
                                           const gchar *key,
                                           gboolean *value);
gboolean     gpds_gconf_get_float         (GConfClient *gconf,
                                           const gchar *key,
                                           gdouble *value);
gboolean     gpds_gconf_get_string        (GConfClient *gconf,
                                           const gchar *key,
                                           gchar **value);

G_END_DECLS

#endif /* __GPDS_GCONF_H__ */
/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
