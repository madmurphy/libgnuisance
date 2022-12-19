# Process this file with automake to produce Makefile.in


AM_CPPFLAGS = \
	@GNUISANCE_BUILD_COMMON_AM_CPPFLAGS@


AM_CFLAGS = \
	@GNUISANCE_BUILD_GUESSED_AM_CFLAGS@ \
	@GNUISANCE_BUILD_COMMON_AM_CFLAGS@ \
	$(WARN_CFLAGS)


BUILT_SOURCES = \
	gnui-emblem-picker.gresources.c \
	gnui-emblem-picker.gresources.h \
	gnui-emblem-picker.gresources.xml


CLEANFILES = \
	$(BUILT_SOURCES)


EMBLEM_PICKER_RESOURCE_PATH = /org/gnuisance/emblem-picker


lib_LTLIBRARIES = \
	libgnuiemblempicker.la


libgnuiemblempicker_la_SOURCES = \
	$(top_srcdir)/src/gnui-environment.c \
	$(top_srcdir)/src/gnui-environment.h \
	gnui-emblem-picker.c \
	gnui-emblem-picker.h


nodist_libgnuiemblempicker_la_SOURCES = \
	gnui-emblem-picker.gresources.c \
	gnui-emblem-picker.gresources.h


libgnuiemblempicker_la_LDFLAGS =


libgnuiemblempicker_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-I'$(top_srcdir)/src/' \
	'-DG_LOG_DOMAIN="GnuiEmblemPicker"' \
	'-DGNUISANCE_CONST_BUILD_FLAG_CPP_UNIT=GNUI_EMBLEM_PICKER' \
	'-DGNUISANCE_CONST_BUILD_FLAG_C_UNIT=gnui_emblem_picker' \
	'-DGNUI_ENVIRONMENT_RESOURCE_PATH="$(EMBLEM_PICKER_RESOURCE_PATH)"'


libgnuiemblempicker_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GNUI_EMBLEM_PICKER_CFLAGS)


libgnuiemblempicker_la_LIBADD = \
	$(GNUI_EMBLEM_PICKER_LIBS)


gnuiinclude_HEADERS = \
	gnui-emblem-picker.h


pkgconfig_DATA = \
	gnuiemblempicker.pc


EXTRA_DIST = \
	gnuiemblempicker.pc.in \
	icons \
	style.css

gnui-emblem-picker.gresources.xml:
	@{ \
		echo '<?xml version="1.0" encoding="UTF-8"?>'; \
		echo '<gresources>'; \
		echo '	<gresource prefix="$(EMBLEM_PICKER_RESOURCE_PATH)">'; \
		! test -f 'style.css' || \
		echo '		<file>style.css</file>'; \
		! test -d 'icons' || \
		$(FIND) 'icons' -type f -printf '\t\t<file>%p</file>\n' | $(SORT); \
		echo '	</gresource>'; \
		echo '</gresources>'; \
	} > '$@'


gnui-emblem-picker.gresources.c: gnui-emblem-picker.gresources.xml style.css
	glib-compile-resources \
	--sourcedir='$(top_srcdir)/src/widgets/emblem-picker' --target='$@' \
	--c-name gnui_emblem_picker --generate-source '$<';


gnui-emblem-picker.gresources.h: gnui-emblem-picker.gresources.xml style.css
	glib-compile-resources \
	--sourcedir='$(top_srcdir)/src/widgets/emblem-picker' --target='$@' \
	--c-name gnui_emblem_picker --generate-header '$<';


# EOF
