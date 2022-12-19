# Process this file with automake to produce Makefile.in


AM_CPPFLAGS = \
	@GNUISANCE_BUILD_COMMON_AM_CPPFLAGS@


AM_CFLAGS = \
	@GNUISANCE_BUILD_GUESSED_AM_CFLAGS@ \
	@GNUISANCE_BUILD_COMMON_AM_CFLAGS@ \
	$(WARN_CFLAGS)


BUILT_SOURCES = \
	gnui-tagged-entry.gresources.c \
	gnui-tagged-entry.gresources.h \
	gnui-tagged-entry.gresources.xml


CLEANFILES = \
	$(BUILT_SOURCES)


TAGGED_ENTRY_RESOURCE_PATH = /org/gnuisance/tagged-entry


lib_LTLIBRARIES = \
	libgnuitaggedentry.la


libgnuitaggedentry_la_SOURCES = \
	$(top_srcdir)/src/gnui-environment.c \
	$(top_srcdir)/src/gnui-environment.h \
	gnui-tagged-entry.c \
	gnui-tagged-entry.h


nodist_libgnuitaggedentry_la_SOURCES = \
	gnui-tagged-entry.gresources.c \
	gnui-tagged-entry.gresources.h


libgnuitaggedentry_la_LDFLAGS =


libgnuitaggedentry_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-I'$(top_srcdir)/src/' \
	-I'$(top_srcdir)/src/widgets/flow/' \
	'-DG_LOG_DOMAIN="GnuiTaggedEntry"' \
	'-DGNUISANCE_CONST_BUILD_FLAG_CPP_UNIT=GNUI_TAGGED_ENTRY' \
	'-DGNUISANCE_CONST_BUILD_FLAG_C_UNIT=gnui_tagged_entry' \
	'-DGNUI_ENVIRONMENT_RESOURCE_PATH="$(TAGGED_ENTRY_RESOURCE_PATH)"' \
	'-DGNUI_ENVIRONMENT_CONST_BUILD_FLAG_NO_RESOURCE_ICONS'


libgnuitaggedentry_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GNUI_TAGGED_ENTRY_CFLAGS)


libgnuitaggedentry_la_LIBADD = \
	$(GNUI_TAGGED_ENTRY_LIBS) \
	$(top_builddir)/src/widgets/flow/libgnuiflow.la


gnuiinclude_HEADERS = \
	gnui-tagged-entry.h


pkgconfig_DATA = \
	gnuitaggedentry.pc


EXTRA_DIST = \
	gnuitaggedentry.pc.in \
	style.css


gnui-tagged-entry.gresources.xml:
	@{ \
		echo '<?xml version="1.0" encoding="UTF-8"?>'; \
		echo '<gresources>'; \
		echo '	<gresource prefix="$(TAGGED_ENTRY_RESOURCE_PATH)">'; \
		! test -f 'style.css' || \
		echo '		<file>style.css</file>'; \
		! test -d 'icons' || \
		$(FIND) 'icons' -type f -printf '\t\t<file>%p</file>\n' | $(SORT); \
		echo '	</gresource>'; \
		echo '</gresources>'; \
	} > '$@'


gnui-tagged-entry.gresources.c: gnui-tagged-entry.gresources.xml style.css
	glib-compile-resources \
	--sourcedir='$(top_srcdir)/src/widgets/tagged-entry' --target='$@' \
	--c-name gnui_tagged_entry --generate-source '$<';


gnui-tagged-entry.gresources.h: gnui-tagged-entry.gresources.xml style.css
	glib-compile-resources \
	 --sourcedir='$(top_srcdir)/src/widgets/tagged-entry' --target='$@' \
	--c-name gnui_tagged_entry --generate-header '$<';


# EOF
