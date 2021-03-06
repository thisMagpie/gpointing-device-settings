#include "gpds-utils.h"

#include <gcutter.h>

void test_get_ui_file_directory (void);
void test_get_icon_file_directory (void);

static gchar *ui_dir;
static gchar *icon_dir;

void
setup (void)
{
    ui_dir = NULL;
    if (g_getenv("GPDS_UI_DIR")) {
        ui_dir = g_strdup(g_getenv("GPDS_UI_DIR"));
        g_unsetenv("GPDS_UI_DIR");
    }
    if (g_getenv("GPDS_ICON_DIR")) {
        icon_dir = g_strdup(g_getenv("GPDS_ICON_DIR"));
        g_unsetenv("GPDS_ICON_DIR");
    }
}

void
teardown (void)
{
    if (ui_dir)
        g_setenv("GPDS_UI_DIR", ui_dir, FALSE);
    g_free(ui_dir);
    if (icon_dir)
        g_setenv("GPDS_ICON_DIR", icon_dir, FALSE);
    g_free(icon_dir);
}

void
test_get_ui_file_directory (void)
{
    cut_assert_equal_string(GPDS_UIDIR,
                            gpds_get_ui_file_directory());
}

void
test_get_icon_file_directory (void)
{
    cut_assert_equal_string(GPDS_ICONDIR,
                            gpds_get_icon_file_directory());
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
