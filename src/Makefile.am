# Process this file with automake to produce Makefile.in


SUBDIRS = \
	widgets


AM_CPPFLAGS = \
	@GNUISANCE_BUILD_COMMON_AM_CPPFLAGS@


AM_CFLAGS = \
	@GNUISANCE_BUILD_GUESSED_AM_CFLAGS@ \
	@GNUISANCE_BUILD_COMMON_AM_CFLAGS@ \
	$(WARN_CFLAGS)


BUILT_SOURCES = \
	gnuisance.gresources.c \
	gnuisance.gresources.h \
	gnuisance.gresources.xml \
	style.css


CLEANFILES = \
	$(BUILT_SOURCES)


lib_LTLIBRARIES = \
	libgnuienvironment.la \
	libgnuisance.la


libgnuienvironment_la_SOURCES = \
	gnuisance.gresources.c \
	gnuisance.gresources.h \
	gnui-environment.c \
	gnui-environment.h


libgnuienvironment_la_LDFLAGS =


libgnuienvironment_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	'-DG_LOG_DOMAIN="GnuiTaggedEnvironment"'


libgnuienvironment_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GNUI_ENVIRONMENT_CFLAGS)


libgnuienvironment_la_LIBADD = \
	$(GNUI_ENVIRONMENT_LIBS)


libgnuisance_la_SOURCES = \
	gnui-environment.c \
	gnui-environment.h \
	widgets/emblem-picker/gnui-emblem-picker.c \
	widgets/emblem-picker/gnui-emblem-picker.h \
	widgets/flow/gnui-flow.c \
	widgets/flow/gnui-flow.h \
	widgets/tagged-entry/gnui-tagged-entry.c \
	widgets/tagged-entry/gnui-tagged-entry.h


nodist_libgnuisance_la_SOURCES = \
	gnuisance.gresources.c \
	gnuisance.gresources.h


libgnuisance_la_LDFLAGS =


libgnuisance_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-I'$(top_srcdir)/src/widgets/flow/' \
	'-DG_LOG_DOMAIN="GNUIsance"'


libgnuisance_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GNUISANCE_CFLAGS)


libgnuisance_la_LIBADD = \
	$(GNUISANCE_LIBS)


include_HEADERS = \
	gnuisance.h


gnuiinclude_HEADERS = \
	gnui-definitions.h \
	gnui-environment.h


pkgconfig_DATA = \
	gnuienvironment.pc \
	gnuisance.pc


EXTRA_DIST = \
	gnui-internals.h \
	gnuienvironment.pc.in \
	gnuisance.pc.in

CSS_FACTORIES = \
	'$(top_srcdir)/src/widgets/emblem-picker' \
	'$(top_srcdir)/src/widgets/tagged-entry'

ICON_FACTORIES = \
	'$(top_srcdir)/src/widgets/emblem-picker'

clean-local:
	-rm -rf 'icons';

style.css:
	@{ for _COIN_ in $(CSS_FACTORIES); do cat "$${_COIN_}/style.css"; \
	echo; done; } > '$@'

icons:
	@_ICONDIRS_="$$(for _COIN_ in $(ICON_FACTORIES); do (cd "$${_COIN_}" && $(FIND) 'icons' -type d) done)" \
	_ICONFILES_="$$(for _COIN_ in $(ICON_FACTORIES); do (cd "$${_COIN_}" && $(FIND) 'icons' -type f -printf "$${_COIN_}"':%p\n') done)" && \
	for _COIN_ in $${_ICONDIRS_}; do $(MKDIR_P) "$${_COIN_}"; done && \
	for _COIN_ in $${_ICONFILES_}; do \
	_SRCPATH_="$$(echo "$${_COIN_}" | cut -d ':' -f 2)" && cp \
	"$$(echo "$${_COIN_}" | cut -d ':' -f 1)/$${_SRCPATH_}" \
	"$${_SRCPATH_}"; done;

gnuisance.gresources.xml: style.css icons
	@{ \
		echo '<?xml version="1.0" encoding="UTF-8"?>'; \
		echo '<gresources>'; \
		echo '	<gresource prefix="/org/gnuisance">'; \
		echo '		<file>style.css</file>'; \
		$(FIND) 'icons' -type f -printf '\t\t<file>%p</file>\n' | $(SORT); \
		echo '	</gresource>'; \
		echo '</gresources>'; \
	} > '$@'

gnuisance.gresources.c: gnuisance.gresources.xml style.css icons
	glib-compile-resources --target='$@' \
	--c-name gnuisance --generate-source '$<';


gnuisance.gresources.h: gnuisance.gresources.xml style.css icons
	glib-compile-resources --target='$@' \
	--c-name gnuisance --generate-header '$<';


# EOF
