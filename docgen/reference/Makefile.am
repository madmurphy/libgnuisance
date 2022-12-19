# Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir)

AM_CFLAGS =

# CFLAGS and LDFLAGS for compiling gtkdoc-scangobj with your library.
# Only needed if you are using gtkdoc-scangobj to dynamically query widget
# signals and properties.
# e.g. GTKDOC_CFLAGS=-I$(top_srcdir) -I$(top_builddir) $(GTK_DEBUG_FLAGS)
# e.g. GTKDOC_LIBS=$(top_builddir)/gtk/$(gtktargetlib)
GTKDOC_CFLAGS =

GTKDOC_LIBS = \
	$(top_builddir)/src/libgnuisance.la \
	$(top_builddir)/src/libgnuienvironment.la \
	$(top_builddir)/src/widgets/emblem-picker/libgnuiemblempicker.la \
	$(top_builddir)/src/widgets/flow/libgnuiflow.la \
	$(top_builddir)/src/widgets/tagged-entry/libgnuitaggedentry.la

DOC_MODULE = gnuisance

#DOC_MODULE_VERSION = 1

DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.xml

DOC_SOURCE_DIR = \
	$(top_srcdir)/src

# Extra options to pass to gtkdoc-scangobj. Normally not needed.
SCANGOBJ_OPTIONS =

SCAN_OPTIONS = \
	--deprecated-guards='GNUISANCE_DISABLE_DEPRECATED'

MKDB_OPTIONS = \
	--name-space=gnuisance \
	--xml-mode --output-format=xml

# Extra options to supply to gtkdoc-mkhtml
MKHTML_OPTIONS =

# Extra options to supply to gtkdoc-fixref. Normally not needed.
# e.g. FIXXREF_OPTIONS=--extra-dir=../gdk-pixbuf/html --extra-dir=../gdk/html
FIXXREF_OPTIONS =

HFILE_GLOB = \
	$(top_srcdir)/src/*.h

CFILE_GLOB = \
	$(top_srcdir)/src/*.c

# Extra header to include when scanning, which are not under DOC_SOURCE_DIR
# e.g. EXTRA_HFILES=$(top_srcdir}/contrib/extra.h
EXTRA_HFILES =

# Header files or dirs to ignore when scanning. Use base file/dir names
# e.g. IGNORE_HFILES=gtkdebug.h gtkintl.h private_code
IGNORE_HFILES = \
	$(top_srcdir)/src/gnui-internals.h

# Images to copy into HTML directory.
# e.g. HTML_IMAGES=$(top_srcdir)/gtk/stock-icons/stock_about_24.png
HTML_IMAGES = \
	$(top_srcdir)/src/widgets/emblem-picker/gnui-emblem-picker-screenshot.png \
	$(top_srcdir)/src/widgets/flow/gnui-flow-screenshot.png \
	$(top_srcdir)/src/widgets/tagged-entry/gnui-tagged-entry-screenshot.png

# Extra files that are included by $(DOC_MAIN_SGML_FILE).
# e.g. content_files=running.xml building.xml changes-2.0.xml
content_files =

# Files where gtk-doc abbreviations (#GtkWidget) are expanded
# e.g. expand_content_files=running.xml
expand_content_files =

include $(srcdir)/gtk-doc.make

CLEANFILES += \
	$(DOC_MODULE)-sections.txt \
	$(DOC_MODULE)-overrides.txt \
	$(DOC_MODULE).types

# Comment this out if you don't want 'make check' to test you doc status
# and run some sanity checks
if ENABLE_GTK_DOC

TESTS_ENVIRONMENT = \
	DOC_MODULE=$(DOC_MODULE) DOC_MAIN_SGML_FILE=$(DOC_MAIN_SGML_FILE) \
	SRCDIR=$(abs_srcdir) BUILDDIR=$(abs_builddir)

TESTS = \
	$(GTKDOC_CHECK)

endif


## EOF
