# Process this file with automake to produce Makefile.in


AM_CPPFLAGS = \
	@GNUISANCE_BUILD_COMMON_AM_CPPFLAGS@


AM_CFLAGS = \
	@GNUISANCE_BUILD_GUESSED_AM_CFLAGS@ \
	@GNUISANCE_BUILD_COMMON_AM_CFLAGS@ \
	$(WARN_CFLAGS)


lib_LTLIBRARIES = \
	libgnuiflow.la


libgnuiflow_la_SOURCES = \
	$(top_srcdir)/src/gnui-environment.c \
	$(top_srcdir)/src/gnui-environment.h \
	gnui-flow.c \
	gnui-flow.h


libgnuiflow_la_LDFLAGS =


libgnuiflow_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-I'$(top_srcdir)/src/' \
	'-DG_LOG_DOMAIN="GnuiFlow"' \
	'-DGNUISANCE_CONST_BUILD_FLAG_CPP_UNIT=GNUI_FLOW' \
	'-DGNUISANCE_CONST_BUILD_FLAG_C_UNIT=gnui_flow' \
	'-DGNUISANCE_CONST_BUILD_FLAG_NO_ENVIRONMENT'


libgnuiflow_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GNUI_FLOW_CFLAGS)


libgnuiflow_la_LIBADD = \
	$(GNUI_FLOW_LIBS)


gnuiinclude_HEADERS = \
	gnui-flow.h \
	gnui-flow-layout.h


pkgconfig_DATA = \
	gnuiflow.pc


EXTRA_DIST = \
	gnuiflow.pc.in


# EOF
