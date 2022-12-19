/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*  Please make sure that the TAB width in your editor is set to 4 spaces  */

/*\
|*|
|*| xyz-foo-bar.c
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



#ifndef I_
#define I_(STRING) g_intern_static_string(STRING)
#endif


#include <gtk/gtk.h>
#include <gnuisance/gnui-flow.h>
#include "xyz-foo-bar.h"



/*\
|*|
|*| GLOBAL TYPES AND VARIABLES
|*|
\*/


typedef struct {
	gboolean dummy;
} XyzFooBarPrivate;


struct _XyzFooBar {
	GtkWidget parent_instance;
};


G_DEFINE_FINAL_TYPE_WITH_PRIVATE(XyzFooBar, xyz_foo_bar, GTK_TYPE_WIDGET)


enum {
	/*  Reserved for GObject  */
	_PROPERTY_RESERVED_ = 0,
	/*  Properties  */
	PROPERTY_DUMMY,
	/*  Number of properties  */
	N_PROPERTIES
};


static GParamSpec * props[N_PROPERTIES];



/*\
|*|
|*| PRIVATE FUNCTIONS
|*|
\*/


static void xyz_foo_bar_dispose (
	GObject * const object
) {

	GtkWidget * child;

	while ((child = gtk_widget_get_first_child(GTK_WIDGET(object)))) {

		gtk_widget_unparent(child);

	}

	G_OBJECT_CLASS(xyz_foo_bar_parent_class)->dispose(object);

}


static void xyz_foo_bar_get_property (
	GObject * const object,
	const guint prop_id,
	GValue * const value,
	GParamSpec * const pspec
) {

	XyzFooBar * const self = XYZ_FOO_BAR(object);
	XyzFooBarPrivate *const priv = xyz_foo_bar_get_instance_private(self);

	switch (prop_id) {

		case PROPERTY_DUMMY:

			g_value_set_boolean(value, priv->dummy);
			break;

		default:

			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);

	}

}


static void xyz_foo_bar_set_property (
	GObject * const object,
	const guint prop_id,
	const GValue * const value,
	GParamSpec * const pspec
) {

	XyzFooBar * const self = XYZ_FOO_BAR(object);
	XyzFooBarPrivate * const priv = xyz_foo_bar_get_instance_private(self);

	switch (prop_id) {

		case PROPERTY_DUMMY:

			priv->dummy = g_value_get_boolean(value);
			break;

		default:

			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);

	}

}


static void xyz_foo_bar_class_init (
	XyzFooBarClass * const klass
) {

	GObjectClass * const object_class = G_OBJECT_CLASS(klass);
	GtkWidgetClass * const widget_class = GTK_WIDGET_CLASS(klass);

	object_class->dispose = xyz_foo_bar_dispose;
	object_class->get_property = xyz_foo_bar_get_property;
	object_class->set_property = xyz_foo_bar_set_property;

	props[PROPERTY_DUMMY] = g_param_spec_boolean(
		"dummy",
		"gboolean",
		"This is a dummy property",
		false,
		G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
	);

	g_object_class_install_properties(object_class, N_PROPERTIES, props);

	gtk_widget_class_set_layout_manager_type(
		widget_class,
		GNUI_TYPE_FLOW_LAYOUT
	);

	gtk_widget_class_set_css_name(widget_class, I_("foobar"));

}


static void xyz_foo_bar_init (
	XyzFooBar * const self
) {

	GnuiFlowLayout * layout_manager = 
		GNUI_FLOW_LAYOUT(gtk_widget_get_layout_manager(GTK_WIDGET(self)));

	g_object_set(
		layout_manager,
		"spacing", 6,
		"leading", 6,
		NULL
	);

	gtk_widget_insert_before(gtk_label_new("One"), GTK_WIDGET(self), NULL);
	gtk_widget_insert_before(gtk_label_new("Two"), GTK_WIDGET(self), NULL);
	gtk_widget_insert_before(gtk_label_new("Three"), GTK_WIDGET(self), NULL);
	gtk_widget_insert_before(gtk_label_new("Four"), GTK_WIDGET(self), NULL);
	gtk_widget_insert_before(gtk_label_new("Five"), GTK_WIDGET(self), NULL);
	gtk_widget_insert_before(gtk_label_new("Six"), GTK_WIDGET(self), NULL);
	gtk_widget_insert_before(gtk_label_new("Seven"), GTK_WIDGET(self), NULL);
	gtk_widget_insert_before(gtk_label_new("Eight"), GTK_WIDGET(self), NULL);
	gtk_widget_insert_before(gtk_label_new("Nine"), GTK_WIDGET(self), NULL);
	gtk_widget_insert_before(gtk_label_new("Ten"), GTK_WIDGET(self), NULL);

}



/*\
|*|
|*| PUBLIC FUNCTIONS
|*|
\*/


G_GNUC_WARN_UNUSED_RESULT GtkWidget * xyz_foo_bar_new (void) {

	return g_object_new(XYZ_TYPE_FOO_BAR, NULL);

}


/*  EOF  */
