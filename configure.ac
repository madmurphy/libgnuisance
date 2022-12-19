dnl  Process this file with autoconf to produce a configure script.

m4_include([autostuff/m4/not-autotools.m4])


dnl  **************************************************************************
dnl     A U T O C O N F   P R E A M B L E
dnl  **************************************************************************


AC_PREREQ([2.69])

AC_INIT([GNUIsance Library],
	[0.1.0],
	[madmurphy333@gmail.com],
	[libgnuisance],
	[https://gitlab.gnome.org/madmurphy/libgnuisance])

###  Temporarily unset `CFLAGS` and let `AC_PROG_CC` guess freely
NS_MOVEVAR([na_tmpusrcflags], [CFLAGS])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_SRCDIR([src/gnui-environment.c])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([subdir-objects])

AM_SILENT_RULES([yes])

AC_PROG_CC

###  Save the guessed `CFLAGS` for `AM_CFLAGS`
NC_SUBST_NOTMAKE([GNUISANCE_BUILD_GUESSED_AM_CFLAGS], ["${CFLAGS}"])

###  Restore `CFLAGS` according to the user's will
NS_MOVEVAR([CFLAGS], [na_tmpusrcflags])

###  Keep the `pkgconfigdir` environment variable when non-empty
AS_IF([test "x${pkgconfigdir}" = x],
	[AS_VAR_SET([pkgconfigdir], ['${libdir}/pkgconfig'])])

###  Add `--with-pkgconfigdir=DIR` option
AC_ARG_WITH([pkgconfigdir],
	[AS_HELP_STRING([--with-pkgconfigdir=DIR],
		[pkg-config installation directory
		@<:@default=<libdir>/pkgconfig@:>@])],
	[AS_IF([test "x${withval}" = x -o "x${with_pkgconfigdir}" = xyes],
		[AC_MSG_ERROR([a DIR value must be specified for option --with-pkgconfigdir=DIR])],
		[AS_IF([test "x${with_pkgconfigdir}" = xno],
			[AC_MSG_WARN([unrecognized option: --without-pkgconfigdir])],
			[AS_VAR_COPY([pkgconfigdir], [withval])])])],
	[:])

AC_SUBST([pkgconfigdir])

AC_SUBST([gnuiincludedir], ['${includedir}/gnuisance'])

AC_CHECK_PROG([HAVE_PKGCONFIG], [pkg-config], [yes], [no])

AS_IF([test "x${HAVE_PKGCONFIG}" = xno],
	[AC_MSG_ERROR([you need to have pkgconfig installed!])])

NA_MODULE_CHECK_PKGS([GNUISANCE],
	[glib-2.0, libadwaita-1],
	[gtk4])

NA_MODULE_CHECK_PKGS([GNUI_ENVIRONMENT],
	[glib-2.0],
	[gtk4])

NA_MODULE_CHECK_PKGS([GNUI_EMBLEM_PICKER],
	[glib-2.0, libadwaita-1],
	[gtk4])

NA_MODULE_CHECK_PKGS([GNUI_FLOW],
	[glib-2.0],
	[gtk4])

NA_MODULE_CHECK_PKGS([GNUI_TAGGED_ENTRY],
	[glib-2.0],
	[gtk4],
	[gnuiflow])

NC_AUTO_REQ_PROGS([find], [sort])


dnl  **************************************************************************
dnl     I N T E R N A T I O N A L I Z A T I O N
dnl  **************************************************************************

AM_GNU_GETTEXT([external])

AM_GNU_GETTEXT_VERSION([0.19])

AM_CONDITIONAL([NLS_ENABLED], [test "x${USE_NLS}" != xno])

AM_COND_IF([NLS_ENABLED], [
	AC_SUBST([GETTEXT_PACKAGE], AC_PACKAGE_TARNAME)
	AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],
		["${GETTEXT_PACKAGE}"], [GETTEXT package name])
])


dnl  **************************************************************************
dnl     F I N A L I Z A T I O N
dnl  **************************************************************************

LT_INIT

GTK_DOC_CHECK([1.18])

###  Default `AM_CFLAGS` for this project
NC_SUBST_NOTMAKE([GNUISANCE_BUILD_COMMON_AM_CFLAGS],
	['-Wall -Wextra -Winline -g'])

###  Default `AM_CPPFLAGS` for this project
NC_SUBST_NOTMAKE([GNUISANCE_BUILD_COMMON_AM_CPPFLAGS],
	m4_normalize([\''-DPACKAGE_LOCALE_DIR="$(localedir)"'\''
	'\''-DPACKAGE_SRC_DIR="$(srcdir)"'\''
	'\''-DPACKAGE_DATA_DIR="$(pkgdatadir)"'\']))

AC_CONFIG_FILES([
	Makefile
	docgen/Makefile
	docgen/reference/Makefile
	docgen/reference/version.xml
	po/Makefile.in
	src/Makefile
	src/gnuienvironment.pc
	src/gnuisance.pc
	src/widgets/Makefile
	src/widgets/emblem-picker/Makefile
	src/widgets/emblem-picker/gnuiemblempicker.pc
	src/widgets/flow/Makefile
	src/widgets/flow/gnuiflow.pc
	src/widgets/tagged-entry/Makefile
	src/widgets/tagged-entry/gnuitaggedentry.pc
])

AC_OUTPUT

