/*  -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */

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



#ifndef _GNUI_ENVIRONMENT_H_
#define _GNUI_ENVIRONMENT_H_


#include <glib.h>


G_BEGIN_DECLS


/**

    SECTION:gnui-environment
    @title: GNUIsance environment
    @short_description: Stylesheets and icons
    @section_id: gnui-environment
    @stability: Unstable
    @include: gnuisance/gnui-environment.h

    These functions provide stylesheets and icons for the interfaces. Unless
    you have passed `-DGNUISANCE_BUILD_FLAG_MANUAL_ENVIRONMENT` while compiling
    the library, you do not need to to invoke them manually.

    If you are not using libgnuisance in its entirety (`-lgnuisance`), but are
    using only some of its modules (`-lgnuiflow`, `-lgnuitaggedentry`, etc.),
    the names of these functions will have different prefixes equal to those
    of the modules that you are using. For example, if you are only using the
    tagged entry library (`-lgnuitaggedentry`), `gnuisance_environment_init()`
    will be named `gnui_tagged_entry_environment_init()`. You will also not
    have it advertised in any public header, and so you will have to add

    |[<!-- language="C" -->
    extern void gnui_tagged_entry_environment_init (void);
    ]|

    to your compilation unit. The stylesheets and icons loaded by these
    per-module functions are only those required by the specific modules. Some
    modules, like `-lgnuiflow`, do not provide any `..._environment_...()`
    function.

    When compiling the library, widgets that do provide their
    `..._environment_...()` functions can be prevented individually from
    autoloading stylesheets and icons by passing
    `-DGNUI..._BUILD_FLAG_MANUAL_ENVIRONMENT` per-module directives
    (e.g. `-DGNUI_EMBLEM_PICKER_BUILD_FLAG_MANUAL_ENVIRONMENT`,
    `-DGNUI_TAGGED_ENTRY_BUILD_FLAG_MANUAL_ENVIRONMENT`, etc.).

**/


/**

    gnuisance_environment_init:

    Load the library's `GResource` objects

    Each invocation after the first one will silently be no-op.

**/
extern void gnuisance_environment_init (void);


/**

    gnuisance_environment_get_is_initialized:

    Get if the library's `GResource` objects are loaded or not

    Returns:    `true` if the library's `GResource` objects are loaded, `false`
                otherwise

**/
extern gboolean gnuisance_environment_get_is_initialized (void);


G_END_DECLS


#endif


/*  EOF  */

