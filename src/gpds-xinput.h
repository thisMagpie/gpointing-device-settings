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

#ifndef __GPDS_XINPUT_H__
#define __GPDS_XINPUT_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define GPDS_XINPUT_ERROR           (gpds_xinput_error_quark())

#define GPDS_TYPE_XINPUT            (gpds_xinput_get_type ())
#define GPDS_XINPUT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GPDS_TYPE_XINPUT, GpdsXInput))
#define GPDS_XINPUT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GPDS_TYPE_XINPUT, GpdsXInputClass))
#define GPDS_IS_XINPUT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GPDS_TYPE_XINPUT))
#define GPDS_IS_XINPUT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GPDS_TYPE_XINPUT))
#define GPDS_XINPUT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GPDS_TYPE_XINPUT, GpdsXInputClass))

typedef enum
{
    GPDS_XINPUT_ERROR_NO_DEVICE,
    GPDS_XINPUT_ERROR_UNABLE_TO_OPEN_DEVICE
} GpdsXInputError;

typedef struct _GpdsXInput GpdsXInput;
typedef struct _GpdsXInputClass GpdsXInputClass;

struct _GpdsXInput
{
    GObject parent;
};

struct _GpdsXInputClass
{
    GObjectClass parent_class;
};

GQuark       gpds_xinput_error_quark     (void);

GType        gpds_xinput_get_type        (void) G_GNUC_CONST;

gboolean     gpds_xinput_exist_device    (const gchar *device_name);

GpdsXInput  *gpds_xinput_new             (const gchar *device_name);

gboolean     gpds_xinput_set_property    (GpdsXInput *xinput,
                                          const gchar *property_name,
                                          GError **error,
                                          gint first_property_value,
                                          ...) G_GNUC_NULL_TERMINATED;
gboolean     gpds_xinput_get_property    (GpdsXInput *xinput,
                                          const gchar *property_name,
                                          GError **error,
                                          gint **values,
                                          gulong *n_values);

G_END_DECLS

#endif /* __GPDS_XINPUT_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
