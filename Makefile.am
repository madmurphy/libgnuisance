# Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = \
	src \
	docgen


dist_doc_DATA = \
	README \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS


EXTRA_DIST = \
	ChangeLog.md \
	INSTALL \
	NEWS \
	README.md \
	autostuff \
	bootstrap \
	docs \
	package.json \
	sandbox \
	.editorconfig


if NLS_ENABLED

SUBDIRS += \
	po

else !NLS_ENABLED

EXTRA_DIST += \
	po

endif !NLS_ENABLED


CLEANFILES = \
	po/@PACKAGE_TARNAME@.pot

_clean_dirs_ = \
	no-dist

# Shell expansion is supported here
_distclean_dirs_ = \
	'po/backups'

# Shell expansion is supported here
_bootstrapclean_dirs_ = \
	'autom4te.cache' \
	'build-aux' \
	'm4' \
	`$(FIND) -L . -type d -name .deps`

# Shell expansion is supported here
_bootstrapclean_files_ = \
	*'~' \
	*'.stamp' \
	'ABOUT-NLS' \
	'aclocal.m4' \
	'compile' \
	'config'.* \
	'configure' \
	'depcomp' \
	'docgen/reference/gtk-doc.make' \
	'install-sh' \
	'libtool' \
	'ltmain.sh' \
	'missing' \
	'po/boldquot.sed' \
	'po/en@boldquot.header' \
	'po/en@quot.header' \
	'po/insert-header.sin' \
	'po'/*'~' \
	'po/Makefile.in.in' \
	'po/Makevars.template' \
	'po/quot.sed' \
	'po/remove-potcdate.sin' \
	'po/Rules-quot' \
	`$(FIND) -L . -type f -name 'Makefile.in'`


.PHONY: undist
undist:
	@! test -d '@PACKAGE_TARNAME@-@PACKAGE_VERSION@' || (chmod -R +w \
	'@PACKAGE_TARNAME@-@PACKAGE_VERSION@' && rm -r \
	'@PACKAGE_TARNAME@-@PACKAGE_VERSION@'); rm -f \
	'@PACKAGE_TARNAME@-@PACKAGE_VERSION@.tar.gz';


# Remove `$(docdir)` and `$(pkgdatadir)` on uninstall
uninstall-local:
	-rm -r '$(DESTDIR)$(docdir)' '$(DESTDIR)$(pkgdatadir)';


clean-local: undist
	-rm -rf $(_clean_dirs_);


distclean-local:
	-rm -rf $(_distclean_dirs_);


# Update translations
.PHONY: i18n-update
i18n-update:
	$(MKDIR_P) 'po/backups' && $(GREP) -o '^\s*\w\S\+' 'po/LINGUAS' | \
	while read _i_; do cp "po/$${_i_}.po" "po/backups/$${_i_}.po.bak" && \
	rm -f 'po/@PACKAGE_TARNAME@.pot' && $(MAKE) -C 'po' "$${_i_}.po" && \
	sed -i \
	's/"Project-Id-Version:.*$$/"Project-Id-Version: @PACKAGE_TARNAME@ @PACKAGE_VERSION@\\n"/' \
	"po/$${_i_}.po"; done;


# Make the source directory depend on Autotools and a `bootstrap` script
.PHONY: bootstrap-clean
bootstrap-clean: maintainer-clean
	-rm -rf $(_bootstrapclean_dirs_); rm -f $(_bootstrapclean_files_); \
	(cd '$(srcdir)' && rm -rf $(_bootstrapclean_dirs_); \
	rm -f $(_bootstrapclean_files_));


if HAVE_GTK_DOC

 # Generate the documentation in any case
 .PHONY: redocs
 redocs: all
	@$(MAKE) -C 'docgen/reference' clean && $(MAKE) -C 'docgen/reference' \
	all-gtk-doc && rm -rf 'docs' && cp -r 'docgen/reference/html' \
	'docs';

 # Generate the documentation if missing
 docs:
	$(MAKE) redocs;

endif HAVE_GTK_DOC


# EOF
