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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "gsd-pointing-device-manager.h"
#include <glib/gi18n.h>
#include <gpds-xinput.h>
#include <gpds-xinput-utils.h>

#include "gsd-mouse-extension-manager.h"
#include "gsd-touchpad-manager.h"
#include "gsd-pointingstick-manager.h"
#include "gpds-gconf.h"

enum
{
    PROP_0,
    PROP_DEVICE_NAME
};

#define GSD_POINTING_DEVICE_MANAGER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GSD_TYPE_POINTING_DEVICE_MANAGER, GsdPointingDeviceManagerPrivate))

typedef struct _GsdPointingDeviceManagerPrivate  GsdPointingDeviceManagerPrivate;
struct _GsdPointingDeviceManagerPrivate
{
    gchar *device_name;
    GConfClient *gconf;
    guint notify_id;
};

G_DEFINE_ABSTRACT_TYPE (GsdPointingDeviceManager, gsd_pointing_device_manager, G_TYPE_OBJECT)

static void dispose      (GObject      *object);
static void set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec);
static void get_property (GObject      *object,
                          guint         prop_id,
                          GValue       *value,
                          GParamSpec   *pspec);
static void
gsd_pointing_device_manager_init (GsdPointingDeviceManager *manager)
{
    GsdPointingDeviceManagerPrivate *priv;

    priv = GSD_POINTING_DEVICE_MANAGER_GET_PRIVATE(manager);

    priv->device_name = NULL;
    priv->gconf = NULL;
    priv->notify_id = 0;
}

static void
gsd_pointing_device_manager_class_init (GsdPointingDeviceManagerClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    g_object_class_install_property
        (gobject_class,
         PROP_DEVICE_NAME,
         g_param_spec_string("device-name",
                             "Device Name",
                             "The device name",
                             NULL,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

    g_type_class_add_private(gobject_class, sizeof(GsdPointingDeviceManagerPrivate));
}

static gchar *
build_gconf_dir (const gchar *device_name)
{
    gchar *escaped_device_name;
    gchar *gconf_dir;

    escaped_device_name = gconf_escape_key(device_name, -1);
    gconf_dir = g_strdup_printf("%s/%s",
                                GPDS_GCONF_DIR,
                                escaped_device_name);
    g_free(escaped_device_name);
    return gconf_dir;
}

static void
dispose_gconf (GsdPointingDeviceManagerPrivate *priv)
{
    if (priv->notify_id) {
        gchar *gconf_dir;
        gconf_dir = build_gconf_dir(priv->device_name);
        gconf_client_remove_dir(priv->gconf, gconf_dir, NULL);
        gconf_client_notify_remove(priv->gconf, priv->notify_id);
        g_free(gconf_dir);
        priv->notify_id = 0;
    }

    if (priv->gconf) {
        g_object_unref(priv->gconf);
        priv->gconf = NULL;
    }
}

static void
dispose (GObject *object)
{
    GsdPointingDeviceManagerPrivate *priv = GSD_POINTING_DEVICE_MANAGER_GET_PRIVATE(object);

    g_free(priv->device_name);
    dispose_gconf(priv);

    if (G_OBJECT_CLASS(gsd_pointing_device_manager_parent_class)->dispose)
        G_OBJECT_CLASS(gsd_pointing_device_manager_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    GsdPointingDeviceManagerPrivate *priv = GSD_POINTING_DEVICE_MANAGER_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_DEVICE_NAME:
        g_free(priv->device_name);
        priv->device_name = g_value_dup_string(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    GsdPointingDeviceManagerPrivate *priv = GSD_POINTING_DEVICE_MANAGER_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_DEVICE_NAME:
        g_value_set_string(value, priv->device_name);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

GsdPointingDeviceManager *
gsd_pointing_device_manager_new (const gchar *device_type, const gchar *device_name)
{
    GType type = 0;

    if (!g_ascii_strcasecmp(device_type, "mouse"))
        type = GSD_TYPE_MOUSE_EXTENSION_MANAGER;
    else if (!g_ascii_strcasecmp(device_type, "touchpad"))
        type = GSD_TYPE_TOUCHPAD_MANAGER;
    else if (!g_ascii_strcasecmp(device_type, "pointingstick"))
        type = GSD_TYPE_POINTINGSTICK_MANAGER;

    if (type)
        return g_object_new(type, "device-name", device_name, NULL);

    return NULL;
}

static void
cb_gconf_client_notify (GConfClient *client,
                        guint cnxn_id,
                        GConfEntry *entry,
                        gpointer user_data)
{
    GsdPointingDeviceManager *manager = user_data;
    GsdPointingDeviceManagerClass *klass;

    klass = GSD_POINTING_DEVICE_MANAGER_GET_CLASS(manager);

    if (klass->gconf_client_notify)
        klass->gconf_client_notify(manager, client, cnxn_id, entry);
}

gboolean
gsd_pointing_device_manager_start (GsdPointingDeviceManager *manager,
                                   GError                  **error)
{
    GsdPointingDeviceManagerClass *klass;
    GsdPointingDeviceManagerPrivate *priv;
    gchar *gconf_dir;

    priv = GSD_POINTING_DEVICE_MANAGER_GET_PRIVATE(manager);
    priv->gconf = gconf_client_get_default();

    gconf_dir = build_gconf_dir(priv->device_name);
    gconf_client_add_dir(priv->gconf, gconf_dir, GCONF_CLIENT_PRELOAD_ONELEVEL, NULL);
    priv->notify_id = gconf_client_notify_add(priv->gconf,
                                              gconf_dir,
                                              cb_gconf_client_notify,
                                              manager,
                                              NULL,
                                              NULL);
    g_free(gconf_dir);

    klass = GSD_POINTING_DEVICE_MANAGER_GET_CLASS(manager);

    return (klass->start) ? klass->start(manager, error) : TRUE;
}

void
gsd_pointing_device_manager_stop (GsdPointingDeviceManager *manager)
{
    GsdPointingDeviceManagerClass *klass;

    klass = GSD_POINTING_DEVICE_MANAGER_GET_CLASS(manager);

    if (klass->stop)
        klass->stop(manager);

    dispose_gconf(GSD_POINTING_DEVICE_MANAGER_GET_PRIVATE(manager));
}

const gchar *
gsd_pointing_device_manager_get_device_name (GsdPointingDeviceManager *manager)
{
    return GSD_POINTING_DEVICE_MANAGER_GET_PRIVATE(manager)->device_name;
}

GpdsXInput *
gsd_pointing_device_manager_get_xinput (GsdPointingDeviceManager *manager)
{
    const gchar *device_name;

    device_name = gsd_pointing_device_manager_get_device_name(manager);
    if (!device_name)
        return NULL;

    if (!gpds_xinput_utils_exist_device(device_name))
        return NULL;

    return gpds_xinput_new(device_name);
}

gchar *
gsd_pointing_device_manager_build_gconf_key (GsdPointingDeviceManager *manager, const gchar *key)
{
    const gchar *device_name;
    gchar *escaped_device_name, *gconf_key;

    device_name = gsd_pointing_device_manager_get_device_name(manager);
    if (!device_name)
        return NULL;

    escaped_device_name = gconf_escape_key(device_name, -1);
    gconf_key = g_strdup_printf("%s/%s/%s",
                                GPDS_GCONF_DIR,
                                escaped_device_name,
                                key);
    g_free(escaped_device_name);

    return gconf_key;
}

gboolean
gsd_pointing_device_manager_get_gconf_boolean (GsdPointingDeviceManager *manager,
                                               GConfClient *gconf,
                                               const gchar *key,
                                               gboolean *value)
{
    gboolean value_exist;
    gchar *gconf_key;

    gconf_key = gsd_pointing_device_manager_build_gconf_key(manager, key);
    value_exist = gpds_gconf_get_boolean(gconf, gconf_key, value);
    g_free(gconf_key);

    return value_exist;
}

gboolean
gsd_pointing_device_manager_get_gconf_int (GsdPointingDeviceManager *manager,
                                               GConfClient *gconf,
                                               const gchar *key,
                                               gint *value)
{
    gboolean value_exist;
    gchar *gconf_key;

    gconf_key = gsd_pointing_device_manager_build_gconf_key(manager, key);
    value_exist = gpds_gconf_get_int(gconf, gconf_key, value);
    g_free(gconf_key);

    return value_exist;
}

gboolean
gsd_pointing_device_manager_get_gconf_float (GsdPointingDeviceManager *manager,
                                             GConfClient *gconf,
                                             const gchar *key,
                                             gdouble *value)
{
    gboolean value_exist;
    gchar *gconf_key;

    gconf_key = gsd_pointing_device_manager_build_gconf_key(manager, key);
    value_exist = gpds_gconf_get_float(gconf, gconf_key, value);
    g_free(gconf_key);

    return value_exist;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
