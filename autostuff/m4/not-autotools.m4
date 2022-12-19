dnl  -*- Mode: M4; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-

dnl  **************************************************************************
dnl         _   _       _      ___        _        _              _     
dnl        | \ | |     | |    / _ \      | |      | |            | |    
dnl        |  \| | ___ | |_  / /_\ \_   _| |_ ___ | |_ ___   ___ | |___ 
dnl        | . ` |/ _ \| __| |  _  | | | | __/ _ \| __/ _ \ / _ \| / __|
dnl        | |\  | (_) | |_  | | | | |_| | || (_) | || (_) | (_) | \__ \
dnl        \_| \_/\___/ \__| \_| |_/\__,_|\__\___/ \__\___/ \___/|_|___/
dnl
dnl            A collection of useful m4-ish macros for GNU Autotools
dnl
dnl                                               -- Released under GNU GPL3 --
dnl
dnl                                  https://github.com/madmurphy/not-autotools
dnl  **************************************************************************


dnl  **************************************************************************
dnl  NOTE:  This is only a selection of macros from the **Not Autotools**
dnl         project without documentation. For the entire collection and the
dnl         documentation please refer to the project's website.
dnl  **************************************************************************


dnl  NA_SANITIZE_VARNAME(string)
dnl  **************************************************************************
dnl
dnl  Replaces `/\W/g,` with `'_'` and `/^\d/` with `_\0`
dnl
dnl  From: not-autotools/m4/not-autotools.m4
dnl  Version: 1.0.0
dnl
AC_DEFUN([NA_SANITIZE_VARNAME],
	[m4_if(m4_bregexp(m4_normalize([$1]), [[0-9]]), [0], [_])[]m4_translit(m4_normalize([$1]),
		[ !"#$%&\'()*+,-./:;<=>?@[\\]^`{|}~],
		[__________________________________])])


dnl  NS_MOVEVAR(destination, source)
dnl  **************************************************************************
dnl
dnl  Copies the value of `source` into the shell variable `destination`, then
dnl  unsets `source` if this is set
dnl
dnl  Requires: nothing
dnl  Version: 1.0.1
dnl
AC_DEFUN([NS_MOVEVAR],
	[AS_VAR_COPY([$1], [$2])[]m4_newline()[]AS_UNSET([$2])])


dnl  NC_SUBST_NOTMAKE(var[, value])
dnl  **************************************************************************
dnl
dnl  Calls `AC_SUBST(var[, value])` immediately followed by
dnl  `AM_SUBST_NOTMAKE(var)`
dnl
dnl  Requires: nothing
dnl  Version: 1.0.0
dnl
AC_DEFUN([NC_SUBST_NOTMAKE], [
	AC_SUBST([$1][]m4_if([$#], [0], [], [$#], [1], [], [, [$2]]))
	AM_SUBST_NOTMAKE([$1])
])


dnl  NC_AUTO_REQ_PROGS(prog1[, prog2[, prog3[, ... progN]]])
dnl  **************************************************************************
dnl
dnl  Checks whether one or more programs have been provided by the user or can
dnl  be retrieved automatically, generating an error if both conditions are
dnl  absent
dnl
dnl  Requires: `NA_SANITIZE_VARNAME()`
dnl  Version: 1.0.0
dnl
AC_DEFUN([NC_AUTO_REQ_PROGS],
	[m4_ifnblank([$1],
		[m4_pushdef([_lit_], m4_quote(m4_toupper(NA_SANITIZE_VARNAME([$1]))))
		AC_ARG_VAR(_lit_, [path to $1 utility])
		AS_IF([test "x@S|@{]_lit_[}" = x], [
			AC_PATH_PROG(_lit_, [$1])
			AS_IF([test "x@S|@{]_lit_[}" = x],
				[AC_MSG_ERROR([$1 utility not found])])[]m4_popdef([_lit_])
		])[]NC_AUTO_REQ_PROGS(m4_shift($@))])])


dnl  NA_MODULE_CHECK_PKGS(module-name, [requires-private-check],
dnl                       [requires-check], [requires-private-nocheck],
dnl                       [requires-nocheck])
dnl  **************************************************************************
dnl
dnl  Wrapper macro of `PKG_CHECK_MODULES()` that allows to specify and store
dnl  different kind of dependencies separately and declare local libraries that
dnl  must not be checked
dnl
dnl  Requires: `PKG_CHECK_MODULES()` from `pkg.m4` (from `pkgconf` package)
dnl  Version: 1.0.0
dnl
m4_define([NA_MODULE_CHECK_PKGS],
	[m4_define([GL_$1_REQUIRES_PRIVATE],
		[$2]m4_ifnblank([$4], [[, ]])[$4])[]m4_define([GL_$1_REQUIRES],
		[$3]m4_ifnblank([$5], [[, ]])[$5])[]PKG_CHECK_MODULES([$1],
		[$2]m4_ifnblank([$3], [[, ]])[$3])
		AC_SUBST([$1_REQUIRES_PRIVATE],
			[']m4_bpatsubst([[$2]]m4_ifnblank([$4],
				[[[, ]]])[[$4]], ['], ['\\''])['])
		AC_SUBST([$1_REQUIRES],
			[']m4_bpatsubst([[$3]]m4_ifnblank([$5],
				[[[, ]]])[[$5]], ['], ['\\''])['])
		AM_SUBST_NOTMAKE([$1_REQUIRES])
		AM_SUBST_NOTMAKE([$1_REQUIRES_PRIVATE])])


dnl  EOF

