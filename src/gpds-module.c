/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2006 Kouhei Sutou <kou@cozmixng.org>
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
 *  $Id: cut-module.c 1206 2007-04-20 10:29:28Z ktou $
 */

#include <stdlib.h>
#include <string.h>

#include <gmodule.h>

#include "gpds-module.h"
#include "gpds-module-impl.h"

#define GPDS_MODULE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj), GPDS_TYPE_MODULE, GpdsModulePrivate))

typedef struct _GpdsModulePrivate  GpdsModulePrivate;
struct _GpdsModulePrivate
{
    GModule      *library;
    gchar        *mod_path;

    GpdsModuleInitFunc         init;
    GpdsModuleExitFunc         exit;
    GpdsModuleInstantiateFunc  instantiate;
};

G_DEFINE_TYPE(GpdsModule, gpds_module, G_TYPE_TYPE_MODULE)

static void     finalize        (GObject     *object);
static gboolean load            (GTypeModule *module);
static void     unload          (GTypeModule *module);

static void     _gpds_module_show_error   (GModule     *module);
static GModule *_gpds_module_open         (const gchar *mod_path);
static void     _gpds_module_close        (GModule     *module);
static gboolean _gpds_module_load_func    (GModule     *module,
                                          const gchar *func_name,
                                          gpointer    *symbol);
static gboolean _gpds_module_match_name   (const gchar *mod_path,
                                          const gchar *name);

static void
gpds_module_class_init (GpdsModuleClass *klass)
{
    GObjectClass *gobject_class;
    GTypeModuleClass *type_module_class;

    gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize     = finalize;

    type_module_class = G_TYPE_MODULE_CLASS(klass);
    type_module_class->load     = load;
    type_module_class->unload   = unload;

    g_type_class_add_private(gobject_class, sizeof(GpdsModulePrivate));
}

static void
gpds_module_init (GpdsModule *module)
{
    GpdsModulePrivate *priv = GPDS_MODULE_GET_PRIVATE(module);

    priv->library          = NULL;
    priv->mod_path         = NULL;
}

static void
finalize (GObject *object)
{
    GpdsModulePrivate *priv = GPDS_MODULE_GET_PRIVATE(object);

    g_free(priv->mod_path);
    priv->mod_path = NULL;

    G_OBJECT_CLASS(gpds_module_parent_class)->finalize(object);
}

static gboolean
load (GTypeModule *module)
{
    GpdsModulePrivate *priv = GPDS_MODULE_GET_PRIVATE(module);

    priv->library = _gpds_module_open(priv->mod_path);
    if (!priv->library)
        return FALSE;

    if (!_gpds_module_load_func(priv->library,
                                G_STRINGIFY(GPDS_MODULE_IMPL_INIT),
                                (gpointer)&priv->init) ||
        !_gpds_module_load_func(priv->library,
                                G_STRINGIFY(GPDS_MODULE_IMPL_EXIT),
                                (gpointer)&priv->exit) ||
        !_gpds_module_load_func(priv->library,
                                G_STRINGIFY(GPDS_MODULE_IMPL_INSTANTIATE),
                                (gpointer)&priv->instantiate)) {
        _gpds_module_close(priv->library);
        priv->library = NULL;
        return FALSE;
    }

    priv->init(module);

    return TRUE;
}

static void
unload (GTypeModule *module)
{
    GpdsModulePrivate *priv = GPDS_MODULE_GET_PRIVATE(module);

    priv->exit();

    _gpds_module_close(priv->library);
    priv->library  = NULL;

    priv->init = NULL;
    priv->exit = NULL;
    priv->instantiate = NULL;
}

GList *
gpds_module_collect_names (GList *modules)
{
    GList *results = NULL;
    GList *node;

    for (node = modules; node; node = g_list_next(node)) {
        GpdsModule *module;

        module = node->data;
        results = g_list_prepend(results, G_TYPE_MODULE(module)->name);
    }

    return results;
}

static void
_gpds_module_show_error (GModule *module)
{
    const gchar *module_error_message;

    module_error_message = g_module_error();
    if (!module_error_message)
        return;

    if (module) {
        g_warning("%s: %s", g_module_name(module), module_error_message);
    } else {
        g_warning("%s", module_error_message);
    }
}

GpdsModule *
gpds_module_find (GList *modules, const gchar *name)
{
    GList *node;

    for (node = modules; node; node = g_list_next(node)) {
        GpdsModule *module = node->data;
        GpdsModulePrivate *priv;

        priv = GPDS_MODULE_GET_PRIVATE(module);
        if (_gpds_module_match_name(priv->mod_path, name))
            return module;
    }

    return NULL;
}

GObject *
gpds_module_instantiate (GpdsModule *module,
                         const gchar *first_property, va_list var_args)
{
    GObject *object = NULL;
    GpdsModulePrivate *priv;

    priv = GPDS_MODULE_GET_PRIVATE(module);
    if (g_type_module_use(G_TYPE_MODULE(module))) {
        object = priv->instantiate(first_property, var_args);
        g_type_module_unuse(G_TYPE_MODULE(module));
    }

    return object;
}


static GModule *
_gpds_module_open (const gchar *mod_path)
{
    GModule *module;

    module = g_module_open(mod_path, G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
    if (!module) {
        _gpds_module_show_error(NULL);
    }

    return module;
}

static void
_gpds_module_close (GModule *module)
{
    if (module /*&& !g_module_close(module)*/) {
        _gpds_module_show_error(NULL);
    }
}

static gchar *
_gpds_module_module_file_name (const gchar *name)
{
    return g_strconcat(name, "." G_MODULE_SUFFIX, NULL);
}

static gboolean
_gpds_module_load_func (GModule *module, const gchar *func_name,
                       gpointer *symbol)
{
    g_return_val_if_fail(module, FALSE);

    if (g_module_symbol(module, func_name, symbol)) {
        return TRUE;
    } else {
        _gpds_module_show_error(module);
        return FALSE;
    }
}

GpdsModule *
gpds_module_load_module (const gchar *base_dir, const gchar *name)
{
    gchar *mod_base_name, *mod_path;
    GpdsModule *module = NULL;

    mod_base_name = g_build_filename(base_dir, name, NULL);
    if (g_str_has_suffix(mod_base_name, G_MODULE_SUFFIX)) {
        mod_path = mod_base_name;
    } else {
        mod_path = _gpds_module_module_file_name(mod_base_name);
        g_free(mod_base_name);
    }

    if (g_file_test(mod_path, G_FILE_TEST_EXISTS)) {
        GpdsModulePrivate *priv;
        gchar *mod_name;

        module = g_object_new(GPDS_TYPE_MODULE, NULL);
        priv = GPDS_MODULE_GET_PRIVATE(module);
        priv->mod_path = g_strdup(mod_path);

        mod_name = g_strdup(name);
        if (g_str_has_suffix(mod_name, "."G_MODULE_SUFFIX)) {
            guint last_index;
            last_index = strlen(mod_name) - strlen("."G_MODULE_SUFFIX);
            mod_name[last_index] = '\0';
        }
        g_type_module_set_name(G_TYPE_MODULE(module), mod_name);
        g_free(mod_name);
    }
    g_free(mod_path);

    return module;
}

const gchar *
gpds_module_directory (void)
{
    const gchar *dir;

    dir = g_getenv("GPDS_MODULE_DIR");
    return dir ? dir : GPDS_MODULEDIR;
}

GList *
gpds_module_load_modules (void)
{
    return gpds_module_load_modules_unique(gpds_module_directory(), NULL);
}

GList *
gpds_module_load_modules_unique (const gchar *base_dir, GList *exist_modules)
{
    GDir *dir;
    GSList *sorted_entries = NULL;
    GSList *node = NULL;
    GList *modules = NULL;
    const gchar *entry;

    dir = g_dir_open(base_dir, 0, NULL);
    if (!dir)
        return exist_modules;

    while ((entry = g_dir_read_name(dir))) {
        sorted_entries = g_slist_prepend(sorted_entries, g_strdup(entry));
    }
    sorted_entries = g_slist_sort(sorted_entries, (GCompareFunc)strcmp);
    for (node = sorted_entries; node; node = g_slist_next(node)) {
        GpdsModule *module;
        GTypeModule *g_module;

        entry = node->data;
        module = gpds_module_load_module(base_dir, entry);
        if (!module)
            continue;

        g_module = G_TYPE_MODULE(module);
        if (gpds_module_find(exist_modules, g_module->name))
            gpds_module_unload(module);
        else
            modules = g_list_prepend(modules, module);
    }
    g_slist_foreach(sorted_entries, (GFunc)g_free, NULL);
    g_slist_free(sorted_entries);
    g_dir_close(dir);

    return g_list_concat(modules, exist_modules);
}

static gboolean
_gpds_module_match_name (const gchar *mod_path, const gchar *name)
{
    gboolean matched;
    gchar *module_base_name, *normalized_matched_name;

    module_base_name = g_path_get_basename(mod_path);
    normalized_matched_name = g_strconcat(name, "." G_MODULE_SUFFIX, NULL);

    matched = (0 == strcmp(module_base_name, normalized_matched_name));

    g_free(module_base_name);
    g_free(normalized_matched_name);

    return matched;
}

void
gpds_module_unload (GpdsModule *module)
{
    GTypeModule *type_module;

    g_return_if_fail(GPDS_IS_MODULE(module));

    type_module = G_TYPE_MODULE(module);

    if (type_module->type_infos || type_module->interface_infos)
        return;

    g_object_unref(module);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
