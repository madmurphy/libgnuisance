/*  -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */

/*\
|*|
|*| xyz-foo-bar.h
|*|
|*| https://savannah.nongnu.org/projects/xyz-foo-bar
|*|
|*| Copyright (C) 2023 <author@example.org>
|*|
|*| **XyzFooBar** is free software: you can redistribute it and/or modify it
|*| under the terms of the GNU General Public License as published by the Free
|*| Software Foundation, either version 3 of the License, or (at your option)
|*| any later version.
|*|
|*| **XyzFooBar** is distributed in the hope that it will be useful, but
|*| WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
|*| or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
|*| more details.
|*|
|*| You should have received a copy of the GNU General Public License along
|*| with this program. If not, see <http://www.gnu.org/licenses/>.
|*|
\*/



#ifndef _XYZ_FOO_BAR_H_
#define _XYZ_FOO_BAR_H_


#include <gtk/gtk.h>
#include <gnuisance/gnui-flow.h>


G_BEGIN_DECLS


#define XYZ_TYPE_FOO_BAR (xyz_foo_bar_get_type())


G_DECLARE_FINAL_TYPE(XyzFooBar, xyz_foo_bar, XYZ, FOO_BAR, GtkWidget)


GtkWidget * xyz_foo_bar_new (void) G_GNUC_WARN_UNUSED_RESULT;


G_END_DECLS


#endif


/*  EOF  */
