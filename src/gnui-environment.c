/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*  Please make sure that the TAB width in your editor is set to 4 spaces  */

/*\
|*|
|*| gnui-environment.h
|*|
|*| https://github.com/madmurphy/libgnuisance
|*|
|*| Copyright (C) 2022 <madmurphy333@gmail.com>
|*|
|*| **libgnuisance** is free software: you can redistribute it and/or modify it
|*| under the terms of the GNU General Public License as published by the Free
|*| Software Foundation, either version 3 of the License, or (at your option)
|*| any later version.
|*|
|*| **libgnuisance** is distributed in the hope that it will be useful, but
|*| WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
|*| or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
|*| more details.
|*|
|*| You should have received a copy of the GNU General Public License along
|*| with this program. If not, see <http://www.gnu.org/licenses/>.
|*|
\*/


#ifndef GNUISANCE_CONST_BUILD_FLAG_NO_ENVIRONMENT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdbool.h>
#include <gtk/gtk.h>


#ifndef GNUI_ENVIRONMENT_CONST_BUILD_FLAG_NO_INIT
#include "gnui-environment.h"
#endif


#include "gnui-definitions.h"
#include "gnui-internals.h"



/*\
|*|
|*| LOCAL DEFINITIONS
|*|
\*/


#define GNUI_ENVIRONMENT_AUTOINIT \
	GNUI_PP_PASTE2(GNUISANCE_CONST_BUILD_FLAG_C_UNIT, _environment_autoinit)


#ifndef GNUI_ENVIRONMENT_RESOURCE_PATH
#define GNUI_ENVIRONMENT_RESOURCE_PATH "/org/gnuisance"
#endif



/*\
|*|
|*| GLOBAL TYPES AND VARIABLES
|*|
\*/


#ifndef GNUI_ENVIRONMENT_CONST_BUILD_FLAG_NO_INIT
static int GNUI_ENVIRONMENT_IS_INITIALIZED = false;
#endif


/*\
|*|
|*| PRIVATE FUNCTIONS
|*|
\*/


#ifndef GNUI_ENVIRONMENT_CONST_BUILD_FLAG_NO_AUTOINIT

/**

	GNUI_ENVIRONMENT_AUTOINIT:

    Library's autoinit function

    This function has a variable name. Its default name is
    `gnui_environment_autoinit()`, but this can change if the function is
    compiled as part of a stand-alone module.

**/
GNUI_GNUC_CONSTRUCTOR static void GNUI_ENVIRONMENT_AUTOINIT (void) {

	I18N_INIT();

}

#endif



/*\
|*|
|*| PUBLIC FUNCTIONS
|*|
\*/


#ifndef GNUI_ENVIRONMENT_CONST_BUILD_FLAG_NO_INIT

/*

	GNUI_ENVIRONMENT_INIT:

    Load the library's `GResource` objects

    This function has a variable name. Its default name is
    `gnui_environment_init()`, but this can change if the function is compiled
    as part of a stand-alone module.

    Each invocation after the first one will silently be no-op.

*/
void GNUI_ENVIRONMENT_INIT (void) {

	if (G_LIKELY(GNUI_ENVIRONMENT_IS_INITIALIZED)) {

		return;

	}

	GdkDisplay * const display = gdk_display_get_default();

	#ifndef GNUI_ENVIRONMENT_CONST_BUILD_FLAG_NO_RESOURCE_CSS

	GtkCssProvider * const gnuisance_css = gtk_css_provider_new();

	gtk_css_provider_load_from_resource(
		gnuisance_css,
		GNUI_ENVIRONMENT_RESOURCE_PATH "/style.css"
	);

	gtk_style_context_add_provider_for_display (
		display,
		GTK_STYLE_PROVIDER(gnuisance_css),
		GTK_STYLE_PROVIDER_PRIORITY_SETTINGS
	);

	g_object_unref(gnuisance_css);

	#endif

	#ifndef GNUI_ENVIRONMENT_CONST_BUILD_FLAG_NO_RESOURCE_ICONS

	static const char * const gnuisance_resource_icon_paths[] = {
		GNUI_ENVIRONMENT_RESOURCE_PATH "/icons",
		NULL
	};

	gtk_icon_theme_set_resource_path(
		gtk_icon_theme_get_for_display(display),
		gnuisance_resource_icon_paths
	);

	#endif

	GNUI_ENVIRONMENT_IS_INITIALIZED = true;

}


/*

	GNUI_ENVIRONMENT_GET_IS_INITIALIZED:

    Get if the library's `GResource` objects are loaded or not

    This function has a variable name. Its default name is
    `gnui_environment_get_is_initialized()`, but this can change if the
    function is compiled as part of a stand-alone module.

    Returns:    `true` if the library's `GResource` objects are loaded, `false`
                otherwise

*/
gboolean GNUI_ENVIRONMENT_GET_IS_INITIALIZED (void) {

	return (gboolean) GNUI_ENVIRONMENT_IS_INITIALIZED;

}

#endif


#endif


/*  EOF  */

