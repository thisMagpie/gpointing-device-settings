@GSD_INTLTOOL_PLUGIN_RULE@

INCLUDES = 			        \
	 -I$(top_builddir)	        \
	 -I$(top_srcdir)	        \
	 -I$(top_srcdir)/src            \
	 -I$(top_srcdir)/modules/common

AM_CFLAGS =			\
	$(GPDS_CFLAGS)		\
	$(GCONF2_CFLAGS)	\
	$(COVERAGE_CFLAGS)

AM_LDFLAGS =			\
  	-avoid-version -module	\
  	-no-undefined 		\
  	-export-dynamic $(LIBTOOL_EXPORT_OPTIONS)

LIBS =					\
	$(top_builddir)/src/libgpds.la	\
	$(GTK_LIBS)			\
	$(GCONF2_LIBS)			\
	$(GNOME_SETTINGS_DAEMON_LIBS)

gsd_plugin_LTLIBRARIES =	\
	libtrackpoint.la	\
	libtouchpad.la

gsd_plugin_CFLAGS =					\
	$(AM_CFLAGS)					\
	$(GNOME_SETTINGS_DAEMON_CFLAGS)			\
	-DGNOME_SETTINGS_LOCALEDIR=\""$(localedir)"\"

libtrackpoint_la_CFLAGS = $(gsd_plugin_CFLAGS)
libtrackpoint_la_SOURCES = 		\
	gsd-trackpoint-plugin.c		\
	gsd-trackpoint-manager.h	\
	gsd-trackpoint-manager.c

libtrackpoint_la_LIBADD = $(top_builddir)/modules/common/libtrackpoint_common.la

libtouchpad_la_CFLAGS = $(gsd_plugin_CFLAGS)
libtouchpad_la_SOURCES = 		\
	gsd-touchpad-plugin.c		\
	gsd-touchpad-manager.h		\
	gsd-touchpad-manager.c

libtouchpad_la_LIBADD = $(top_builddir)/modules/common/libtouchpad_common.la

gsd_plugin_in_files =	 			\
	trackpoint.gnome-settings-plugin.in	\
	touchpad.gnome-settings-plugin.in

gsd_plugin_DATA = $(gsd_plugin_in_files:.gnome-settings-plugin.in=.gnome-settings-plugin)

EXTRA_DIST = 			\
	$(gsd_plugin_in_files)	\
	gnome-settings-plugin.h

CLEANFILES = 			\
	$(gsd_plugin_DATA)

DISTCLEANFILES =		\
	$(gsd_plugin_DATA)
