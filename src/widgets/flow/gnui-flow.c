/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*  Please make sure that the TAB width in your editor is set to 4 spaces  */

/*\
|*|
|*| gnui-flow.c
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



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdbool.h>
#include <stdarg.h>
#include <gtk/gtk.h>
#include "gnui-internals.h"
#include "gnui-definitions.h"
#include "gnui-flow.h"



/*\
|*|
|*| GLOBAL TYPES AND VARIABLES
|*|
\*/


/**

    GnuiOrientableAllocation:

    An orientation-agnostic version of `GtkAllocation`

**/
typedef struct _GnuiOrientableAllocation {
	guint
		fix_size,
		var_size;
	gint
		fix_axis,
		var_axis;
} GnuiOrientableAllocation;


/**

    GnuiOrientableRequisition:

    An orientation-agnostic version of `GtkRequisition`

**/
typedef struct _GnuiOrientableRequisition {
	guint
		fix_size,
		var_size;
} GnuiOrientableRequisition;


/**

    GnuiOrientableCoordinates:

    Orientation-agnostic coordinates

**/
typedef struct _GnuiOrientableCoordinates {
	gint
		fix_axis,
		var_axis;
} GnuiOrientableCoordinates;


/**

    GnuiOrientablePositionFlags:

    Child orientation-agnostic position flags

**/
typedef guint8 GnuiOrientablePositionFlags;


enum {
	GNUI_ORIENTABLE_POSITION_FLAG_NONE = 0,
	GNUI_ORIENTABLE_POSITION_FLAG_IS_L_START = 1,
	GNUI_ORIENTABLE_POSITION_FLAG_IS_P_START = 2,
	GNUI_ORIENTABLE_POSITION_FLAG_IS_L_END = 4,
	GNUI_ORIENTABLE_POSITION_FLAG_IS_P_END = 8,
	GNUI_ORIENTABLE_POSITION_FLAG_P_RTL = 16,
	GNUI_ORIENTABLE_POSITION_FLAG_L_RTL = 32,
	GNUI_ORIENTABLE_POSITION_FLAG_IS_VERTICAL = 64,
	GNUI_ORIENTABLE_POSITION_FLAG_ALL_FLAGS_SET = 127
};


/**

    GnuiPositionFlags:

    Child position flags

**/
typedef guint8 GnuiPositionFlags;


enum {
	GNUI_POSITION_FLAG_NONE = 0,
	GNUI_POSITION_FLAG_IS_LEFT = 1,
	GNUI_POSITION_FLAG_IS_TOP = 2,
	GNUI_POSITION_FLAG_IS_RIGHT = 4,
	GNUI_POSITION_FLAG_IS_BOTTOM = 8,
	GNUI_POSITION_FLAG_ALL_FLAGS_SET = 15
};


/**

    GnuiFlowRowInfo:

    Row private data

**/
typedef struct _GnuiFlowRowInfo {
	guint
		n_children,
		n_ch_to_expand,
		l_space,
		row_size;
	bool
		vexpand : 1,
		starts_here : 1,
		first_row : 1,
		last_row : 1;
} GnuiFlowRowInfo;


/**

    GnuiFlowChildLayoutPrivate:

    Flow child layout's private data

**/
typedef struct _GnuiFlowChildLayoutPrivate GnuiFlowChildLayoutPrivate;


struct _GnuiFlowChildLayoutPrivate {
	GnuiOrientableAllocation allocation;
	GnuiOrientableCoordinates baselines;
	GtkWidget * child;
	GnuiFlowChildLayoutPrivate
		* prev,
		* next;
	GnuiPositionFlags pos_flags;
	GnuiFlowRowInfo row;
	bool fexpand;
};


/**

    GnuiFlowLayoutPrivate:

    Flow layout manager's private data

**/
typedef struct _GnuiFlowLayoutPrivate {
	GtkOrientation orientation;
	GtkJustification
		line_justify,
		page_justify;
	gint
		spacing,
		leading;
	GtkTextDirection
		page_direction,
		line_direction;
} GnuiFlowLayoutPrivate;


static void gnui_flow_buildable_init (
	GtkBuildableIface * iface
);


G_DEFINE_TYPE_WITH_PRIVATE(
	GnuiFlowChildLayout,
	gnui_flow_child_layout,
	GTK_TYPE_LAYOUT_CHILD
)


G_DEFINE_TYPE_WITH_CODE(
	GnuiFlowLayout,
	gnui_flow_layout,
	GTK_TYPE_LAYOUT_MANAGER,
	G_ADD_PRIVATE(
		GnuiFlowLayout
	) G_IMPLEMENT_INTERFACE(
		GTK_TYPE_ORIENTABLE,
		NULL
	)
)


G_DEFINE_TYPE_WITH_CODE(
	GnuiFlow,
	gnui_flow,
	GTK_TYPE_WIDGET,
	G_IMPLEMENT_INTERFACE(
		GTK_TYPE_ORIENTABLE,
		NULL
	) G_IMPLEMENT_INTERFACE(
		GTK_TYPE_BUILDABLE,
		gnui_flow_buildable_init
	)
)


/**

    GnuiFlowLayoutProperty:

    The flow layout manager's property identifier numbers

**/
typedef enum _GnuiFlowLayoutProperty {

    /*  Reserved for GObject  */
    FLOW_LAYOUT_RESERVED_PROPERTY = 0,

    /*  New properties  */
    FLOW_LAYOUT_PROPERTY_SPACING,
    FLOW_LAYOUT_PROPERTY_LEADING,
    FLOW_LAYOUT_PROPERTY_PAGE_DIRECTION,
    FLOW_LAYOUT_PROPERTY_PAGE_JUSTIFY,
    FLOW_LAYOUT_PROPERTY_LINE_DIRECTION,
    FLOW_LAYOUT_PROPERTY_LINE_JUSTIFY,

    /*  Number of properties  */
    N_FLOW_LAYOUT_PROPERTIES,

    /*  Overridden properties (uncounted)  */
    FLOW_LAYOUT_PROPERTY_ORIENTATION = N_FLOW_LAYOUT_PROPERTIES

} GnuiFlowLayoutProperty;


/**

    GnuiFlowProperty:

    The flow widget's property identifier numbers

**/
typedef enum _GnuiFlowProperty {

    /*  Reserved for GObject  */
    FLOW_RESERVED_PROPERTY = 0,

    /*  New properties  */
    FLOW_PROPERTY_SPACING,
    FLOW_PROPERTY_LEADING,
    FLOW_PROPERTY_PAGE_DIRECTION,
    FLOW_PROPERTY_PAGE_JUSTIFY,
    FLOW_PROPERTY_LINE_DIRECTION,
    FLOW_PROPERTY_LINE_JUSTIFY,

    /*  Number of properties  */
    N_FLOW_PROPERTIES,

    /*  Overridden properties (uncounted)  */
    FLOW_PROPERTY_ORIENTATION = N_FLOW_PROPERTIES

} GnuiFlowProperty;


static GParamSpec * flow_layout_props[N_FLOW_LAYOUT_PROPERTIES];


static GParamSpec * flow_props[N_FLOW_PROPERTIES];


static GtkBuildableIface * parent_buildable_iface;



/*\
|*|
|*| PRIVATE FUNCTIONS
|*|
\*/


/*  Utilities  */


/**

    gnui_flow_chinfo_update_style_classes:
    @chinfo:    (not nullable): Private `struct` of a `GnuiFlowChildLayout`
    @flags:     The orientable position flags for the child

    Update the flow children's positional CSS classes

**/
static void gnui_flow_chinfo_update_style_classes (
	GnuiFlowChildLayoutPrivate * const chinfo,
	const GnuiOrientablePositionFlags flags
) {

	GnuiPositionFlags pos_new_flags = GNUI_ORIENTABLE_POSITION_FLAG_NONE;

	switch (
		flags & (
			GNUI_ORIENTABLE_POSITION_FLAG_P_RTL |
			GNUI_ORIENTABLE_POSITION_FLAG_L_RTL |
			GNUI_ORIENTABLE_POSITION_FLAG_IS_VERTICAL
		)
	) {

		case GNUI_ORIENTABLE_POSITION_FLAG_NONE:

			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_LEFT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_TOP;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_RIGHT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_BOTTOM;

			break;

		case GNUI_ORIENTABLE_POSITION_FLAG_IS_VERTICAL:

			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_LEFT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_TOP;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_RIGHT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_BOTTOM;

			break;

		case GNUI_ORIENTABLE_POSITION_FLAG_L_RTL:

			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_LEFT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_TOP;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_RIGHT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_BOTTOM;

			break;

		case GNUI_ORIENTABLE_POSITION_FLAG_P_RTL:

			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_LEFT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_TOP;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_RIGHT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_BOTTOM;

			break;

		case
			GNUI_ORIENTABLE_POSITION_FLAG_P_RTL |
			GNUI_ORIENTABLE_POSITION_FLAG_IS_VERTICAL:

			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_LEFT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_TOP;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_RIGHT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_BOTTOM;

			break;

		case
			GNUI_ORIENTABLE_POSITION_FLAG_L_RTL |
			GNUI_ORIENTABLE_POSITION_FLAG_IS_VERTICAL:

			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_LEFT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_TOP;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_RIGHT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_BOTTOM;

			break;

		case
			GNUI_ORIENTABLE_POSITION_FLAG_P_RTL |
			GNUI_ORIENTABLE_POSITION_FLAG_L_RTL:

			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_LEFT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_TOP;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_RIGHT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_BOTTOM;

			break;

		case
			GNUI_ORIENTABLE_POSITION_FLAG_P_RTL |
			GNUI_ORIENTABLE_POSITION_FLAG_L_RTL |
			GNUI_ORIENTABLE_POSITION_FLAG_IS_VERTICAL:

			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_LEFT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_END)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_TOP;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_P_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_RIGHT;
			if (flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_L_START)
				pos_new_flags |= GNUI_POSITION_FLAG_IS_BOTTOM;

	}

	GtkWidget * const child = chinfo->child;
	GnuiPositionFlags _tmp_posflags_1_;

	#define css_classes_to_remove _tmp_posflags_1_

	css_classes_to_remove = chinfo->pos_flags & ~pos_new_flags;

	if (css_classes_to_remove & GNUI_POSITION_FLAG_IS_LEFT)
		gtk_widget_remove_css_class(child, "left");
	if (css_classes_to_remove & GNUI_POSITION_FLAG_IS_TOP)
		gtk_widget_remove_css_class(child, "top");
	if (css_classes_to_remove & GNUI_POSITION_FLAG_IS_RIGHT)
		gtk_widget_remove_css_class(child, "right");
	if (css_classes_to_remove & GNUI_POSITION_FLAG_IS_BOTTOM)
		gtk_widget_remove_css_class(child, "bottom");

	#undef css_classes_to_remove
	#define css_classes_to_add _tmp_posflags_1_

	css_classes_to_add = pos_new_flags & ~chinfo->pos_flags;

	if (css_classes_to_add & GNUI_POSITION_FLAG_IS_LEFT)
		gtk_widget_add_css_class(child, "left");
	if (css_classes_to_add & GNUI_POSITION_FLAG_IS_TOP)
		gtk_widget_add_css_class(child, "top");
	if (css_classes_to_add & GNUI_POSITION_FLAG_IS_RIGHT)
		gtk_widget_add_css_class(child, "right");
	if (css_classes_to_add & GNUI_POSITION_FLAG_IS_BOTTOM)
		gtk_widget_add_css_class(child, "bottom");

	#undef css_classes_to_add

	chinfo->pos_flags = pos_new_flags;

}


/*  GnuiFlowLayout and GnuiFlowChildLayout  */


/**

    gnui_flow_layout_update_orientation:
    @manager:       (not nullable): The flow layout manager
    @widget:        (not nullable): The widget currently adopting the flow
                    layout manager
    @orientation:   The flow layout manager's orientation

    Update the flow layout manager's orientation

**/
static void gnui_flow_layout_update_orientation (
	GnuiFlowLayout * const manager,
	GtkWidget * const widget,
	const GtkOrientation orientation
) {

	GnuiFlowLayoutPrivate * const priv =
		gnui_flow_layout_get_instance_private(manager);

	if (priv->orientation == orientation) {

		return;

	}

	priv->orientation = orientation;
	gtk_orientable_set_orientation(GTK_ORIENTABLE(manager), orientation);

	if (orientation == GTK_ORIENTATION_VERTICAL) {

		gtk_widget_add_css_class(widget, "vertical");
		gtk_widget_remove_css_class(widget, "horizontal");

	} else {

		gtk_widget_add_css_class(widget, "horizontal");
		gtk_widget_remove_css_class(widget, "vertical");
	}

	gtk_accessible_update_property(
		GTK_ACCESSIBLE(widget),
		GTK_ACCESSIBLE_PROPERTY_ORIENTATION,
		orientation,
		-1
	);

	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(manager));
	g_object_notify(G_OBJECT(manager), "orientation");

}


/**

    gnui_flow_layout_measure:
    @manager:           (auto) (not nullable): The flow layout manager
    @widget:            (auto) (not nullable): The widget currently adopting
                        the flow layout manager
    @dimension:         (auto): The dimension that is being queried
    @for_size:          (auto): The size of the other dimension, or `-1` if
                        this is unknown
    @minimum_size:      (auto) (optional) (out): A pointer for saving the
						calculated minimum size
    @natural_size:      (auto) (optional) (out): A pointer for saving the
						calculated natural size
    @minimum_baseline:  (auto) (optional) (out): A pointer for saving the
						calculated minimum baseline
    @natural_baseline:  (auto) (optional) (out): A pointer for saving the
						calculated natural baseline

    Class handler for the #GtkLayoutManager.measure() method on the flow layout
    manager instance

**/
static void gnui_flow_layout_measure (
	GtkLayoutManager * const manager,
	GtkWidget * const widget,
	const GtkOrientation dimension,
	const gint for_size,
	gint * const minimum_size,
	gint * const natural_size,
	gint * const minimum_baseline,
	gint * const natural_baseline
) {

	GnuiFlowLayoutPrivate * const priv =
		gnui_flow_layout_get_instance_private(GNUI_FLOW_LAYOUT(manager));

	GtkWidget * child;
	gint ret_min = 0, ret_nat = 0;

	if (for_size == -1 || dimension == priv->orientation) {

		#define tmpint ret_min

		for (
			child = gtk_widget_get_first_child(widget);
				child;
			child = gtk_widget_get_next_sibling(child)
		) {

			gtk_widget_measure(
				child,
				dimension,
				-1,
				NULL,
				&tmpint,
				NULL,
				NULL
			);

			GNUI_SET_IF_GREATER__2_2(ret_nat, tmpint);

		}

		#undef tmpint

		ret_min = ret_nat;
		goto save_and_exit;

	}

	GnuiOrientableAllocation
		nexta, preva = { 0 };
	GnuiOrientableRequisition occupied;
	guint row_size = 0;
	guint space = for_size > -1 ? *((guint *) &for_size) : 0;

	occupied.fix_size = space;
	occupied.var_size = 0;

	for (
		child = gtk_widget_get_first_child(widget);
			child;
		child = gtk_widget_get_next_sibling(child)
	) {

		if (!gtk_widget_should_layout(child)) {

			continue;

		}

		gtk_widget_measure(
			child,
			priv->orientation,
			-1,
			NULL,
			(gint *) &nexta.fix_size,
			NULL,
			NULL
		);

		gtk_widget_measure(
			child,
			GNUI_PERPENDICULAR_ORIENTATION(priv->orientation),
			-1,
			NULL,
			(gint *) &nexta.var_size,
			NULL,
			NULL
		);

		GNUI_NORMALIZE_INT_UINT_CAST__2(nexta.fix_size);
		GNUI_NORMALIZE_INT_UINT_CAST__2(nexta.var_size);

		if (!row_size) {

			nexta.fix_axis = 0;
			nexta.var_axis = 0;
			row_size = nexta.var_size;

		} else if (
			preva.fix_axis + preva.fix_size + nexta.fix_size +
				priv->spacing > space
		) {

			nexta.fix_axis = 0;
			nexta.var_axis = preva.var_axis + row_size + priv->leading;
			row_size = nexta.var_size;

		} else {

			nexta.fix_axis = preva.fix_axis + preva.fix_size + priv->spacing;
			nexta.var_axis = preva.var_axis;
			GNUI_SET_IF_GREATER__2_2(row_size, nexta.var_size);

		}

		GNUI_SET_IF_GREATER__2_2(occupied.fix_size, nexta.fix_size);
		GNUI_SET_IF_GREATER__2_2(occupied.var_size, nexta.var_axis + row_size);
		preva = nexta;

	}

	ret_min = ret_nat =
		dimension == priv->orientation ?
			(gint) occupied.fix_size
		:
			(gint) occupied.var_size;


	/* \                                  /\
	\ */     save_and_exit:              /* \
	 \/     ________________________     \ */


	GNUI_SET_POINTED_VALUE_IF_GIVEN__2_1(minimum_size, ret_min);
	GNUI_SET_POINTED_VALUE_IF_GIVEN__2_1(natural_size, ret_nat);
	GNUI_SET_POINTED_VALUE_IF_GIVEN__2_1(minimum_baseline, -1);
	GNUI_SET_POINTED_VALUE_IF_GIVEN__2_1(natural_baseline, -1);

}


/**

    gnui_flow_layout_allocate:
    @manager:           (auto) (not nullable): The flow layout manager
    @widget:            (auto) (not nullable): The widget currently adopting
                        the flow layout manager
    @available_width:   (auto): The available width
    @available_height:  (auto): The available height
    @baseline:          (auto) (unused): The baseline

    Class handler for the #GtkLayoutManager.allocate() method on the flow
    layout manager instance

**/
static void gnui_flow_layout_allocate (
	GtkLayoutManager * const manager,
	GtkWidget * const widget,
	const int available_width,
	const int available_height,
	const int baseline G_GNUC_UNUSED
) {

	/*  Part I. Measuring and connecting  */

	GtkWidget * child = gtk_widget_get_first_child(widget);


	/* \                                  /\
	\ */     get_should_layout:          /* \
	 \/     ________________________     \ */


	if (!child) {

		return;

	}

	if (!gtk_widget_should_layout(child)) {

		child = gtk_widget_get_next_sibling(child);
		goto get_should_layout;

	}

	/*  ==> End of loop `get_should_layout`  */

	GtkAllocation child_allocation;
	GnuiOrientableAllocation nexta, _alloc_placeholder_1_ = { 0 };
	GnuiOrientableRequisition occupied, space;
	GnuiFlowChildLayoutPrivate * chinfo;
	GnuiFlowRowInfo * roinfo;

	GnuiFlowChildLayoutPrivate * const first_chinfo =
		gnui_flow_child_layout_get_instance_private(
			GNUI_FLOW_CHILD_LAYOUT(
				gtk_layout_manager_get_layout_child(
					manager,
					child
				)
			)
		);

	GnuiFlowLayoutPrivate * const priv =
		gnui_flow_layout_get_instance_private(GNUI_FLOW_LAYOUT(manager));

	guint _uint_placeholder_1_, _uint_placeholder_2_;
	GnuiOrientablePositionFlags new_flags = 0;

	#define preva _alloc_placeholder_1_
	#define n_ro_to_expand _uint_placeholder_1_
	#define n_rows _uint_placeholder_2_

	if (priv->page_direction == GTK_TEXT_DIR_RTL) new_flags |=
		GNUI_ORIENTABLE_POSITION_FLAG_P_RTL;
	if (priv->line_direction == GTK_TEXT_DIR_RTL) new_flags |=
		GNUI_ORIENTABLE_POSITION_FLAG_L_RTL;
	if (priv->orientation == GTK_ORIENTATION_VERTICAL) new_flags |=
		GNUI_ORIENTABLE_POSITION_FLAG_IS_VERTICAL;

	n_ro_to_expand = 0;
	n_rows = 1;

	if (new_flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_VERTICAL) {

		space.fix_size = available_height;
		space.var_size = available_width;

	} else {

		space.fix_size = available_width;
		space.var_size = available_height;

	}

	occupied.fix_size = space.fix_size;
	occupied.var_size = 0;
	chinfo = first_chinfo;
	chinfo->prev = NULL;
	chinfo->row.first_row = true;
	roinfo = &chinfo->row;


	/* \                                  /\
	\ */     measure_children:           /* \
	 \/     ________________________     \ */


	gtk_widget_measure(
		child,
		priv->orientation,
		-1,
		NULL,
		(gint *) &nexta.fix_size,
		NULL,
		&chinfo->baselines.fix_axis
	);

	gtk_widget_measure(
		child,
		GNUI_PERPENDICULAR_ORIENTATION(priv->orientation),
		-1,
		NULL,
		(gint *) &nexta.var_size,
		NULL,
		&chinfo->baselines.var_axis
	);

	GNUI_NORMALIZE_INT_UINT_CAST__2(nexta.fix_size);
	GNUI_NORMALIZE_INT_UINT_CAST__2(nexta.var_size);

	if (!chinfo->prev) {

		/*  This is the first widget  */

		nexta.fix_axis = nexta.var_axis = 0;
		roinfo->row_size = nexta.var_size;
		roinfo->n_children = 1;

	} else if (
		preva.fix_axis + preva.fix_size +
			nexta.fix_size + priv->spacing > space.fix_size
	) {

		/*  This widget starts a new line  */

		roinfo->starts_here = true;

		roinfo->l_space =
			preva.fix_axis + preva.fix_size < space.fix_size ?
				space.fix_size - preva.fix_axis - preva.fix_size
			:
				0;

		nexta.fix_axis = 0;
		nexta.var_axis = preva.var_axis + roinfo->row_size + priv->leading;
		roinfo = &chinfo->row;
		roinfo->n_ch_to_expand = 0;
		roinfo->n_children = 1;
		roinfo->row_size = nexta.var_size;
		n_rows++;

	} else {

		/*  This widget continues on the same line  */

		nexta.fix_axis = preva.fix_axis + preva.fix_size + priv->spacing;
		nexta.var_axis = preva.var_axis;
		GNUI_SET_IF_GREATER__2_2(roinfo->row_size, nexta.var_size);
		roinfo->n_children++;

	}

	chinfo->fexpand = gtk_widget_compute_expand(child, priv->orientation);

	if (chinfo->fexpand) {

		roinfo->n_ch_to_expand++;

	}

	if (
		!roinfo->vexpand && gtk_widget_compute_expand(
			child,
			new_flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_VERTICAL ?
				GTK_ORIENTATION_HORIZONTAL
			:
				GTK_ORIENTATION_VERTICAL
		)
	) {

		roinfo->vexpand = true;
		n_ro_to_expand++;

	}

	GNUI_SET_IF_GREATER__2_2(occupied.fix_size, nexta.fix_size);

	GNUI_SET_IF_GREATER__2_2(
		occupied.var_size,
		nexta.var_axis + roinfo->row_size
	);

	chinfo->allocation = preva = nexta;
	chinfo->child = child;


	/* \                                  /\
	\ */     get_next_measurable:        /* \
	 \/     ________________________     \ */


	child = gtk_widget_get_next_sibling(child);

	if (child) {

		if (!gtk_widget_should_layout(child)) {

			goto get_next_measurable;

		}

		/*  ==> End of loop `get_next_measurable`  */

		chinfo->next =
			gnui_flow_child_layout_get_instance_private(
				GNUI_FLOW_CHILD_LAYOUT(
					gtk_layout_manager_get_layout_child(
						manager,
						child
					)
				)
			);

		chinfo->next->prev = chinfo;
		chinfo = chinfo->next;
		goto measure_children;

	}

	/*  ==> End of loop `measure_children`  */

	roinfo->l_space =
		nexta.fix_axis + nexta.fix_size < space.fix_size ?
			space.fix_size - nexta.fix_axis - nexta.fix_size
		:
			0;

	roinfo->starts_here = true;
	roinfo->last_row = true;
	chinfo->next = NULL;

	/*  Part II. Allocating  */

	const bool
		p_no_big_guys = n_ro_to_expand < 1,
		p_fill_evenly =
			p_no_big_guys && priv->page_justify == GTK_JUSTIFY_FILL;

	bool l_fill_evenly, l_no_big_guys;

	guint
		_uint_placeholder_3_,
		p_n_exp = p_fill_evenly ? n_rows : n_ro_to_expand;

	const guint
		p_space =
			space.var_size > occupied.var_size ?
				space.var_size - occupied.var_size
			:
				0,
		p_n_larger_exp_quota =
			p_n_exp > 1 ?
				p_n_exp - (p_space % p_n_exp)
			:
				p_n_exp - p_space,
		p_exp_quota = p_n_exp > 0 ? p_space / p_n_exp : 0;

	#undef n_rows
	#undef n_ro_to_expand
	#undef preva
	#define next_offset _alloc_placeholder_1_
	#define l_n_exp _uint_placeholder_1_
	#define l_exp_quota _uint_placeholder_2_
	#define l_n_larger_exp_quota _uint_placeholder_3_

	chinfo = first_chinfo;
	memset(&_alloc_placeholder_1_, 0, sizeof(next_offset));

	do {

		roinfo = &chinfo->row;
		l_no_big_guys = roinfo->n_ch_to_expand < 1;

		l_fill_evenly =
			l_no_big_guys && priv->line_justify == GTK_JUSTIFY_FILL;

		l_n_exp =
			l_fill_evenly ?
				roinfo->n_children
			:
				roinfo->n_ch_to_expand;

		l_n_larger_exp_quota =
			l_n_exp > 0 ?
				l_n_exp - (roinfo->l_space % l_n_exp)
			:
				l_n_exp;

		l_exp_quota = l_n_exp > 0 ? roinfo->l_space / l_n_exp : 0;

		do {

			nexta = chinfo->allocation;
			nexta.fix_axis = next_offset.fix_axis;
			nexta.var_size = roinfo->row_size;
			child = chinfo->child;

			if (l_fill_evenly || (l_n_exp > 0 && chinfo->fexpand)) {

				nexta.fix_size +=
					l_n_exp-- > l_n_larger_exp_quota ?
						l_exp_quota + 1
					:
						l_exp_quota;

			} else if (
				l_no_big_guys &&
				priv->line_justify == GTK_JUSTIFY_RIGHT
			) {

				nexta.fix_axis += roinfo->l_space;

			} else if (
				l_no_big_guys &&
				priv->line_justify == GTK_JUSTIFY_CENTER
			) {

				nexta.fix_axis += roinfo->l_space / 2;

			}

			if (new_flags & GNUI_ORIENTABLE_POSITION_FLAG_L_RTL) {

				nexta.fix_axis =
					space.fix_size - nexta.fix_axis - nexta.fix_size;

			}

			next_offset.fix_axis += nexta.fix_size + priv->spacing;
			nexta.var_axis = next_offset.var_axis;

			if (p_fill_evenly || roinfo->vexpand) {

				nexta.var_size +=
					p_n_exp > p_n_larger_exp_quota ?
						p_exp_quota + 1
					:
						p_exp_quota;

			} else if (
				p_no_big_guys &&
				priv->page_justify == GTK_JUSTIFY_RIGHT
			) {

				nexta.var_axis += p_space;

			} else if (
				p_no_big_guys &&
				priv->page_justify == GTK_JUSTIFY_CENTER
			) {

				nexta.var_axis += p_space / 2;

			}

			if (new_flags & GNUI_ORIENTABLE_POSITION_FLAG_P_RTL) {

				nexta.var_axis =
					space.var_size - nexta.var_axis - nexta.var_size;

			}

			if (new_flags & GNUI_ORIENTABLE_POSITION_FLAG_IS_VERTICAL) {

				child_allocation.width = nexta.var_size;
				child_allocation.height = nexta.fix_size;
				child_allocation.x = nexta.var_axis;
				child_allocation.y = nexta.fix_axis;

			} else {

				child_allocation.width = nexta.fix_size;
				child_allocation.height = nexta.var_size;
				child_allocation.x = nexta.fix_axis;
				child_allocation.y = nexta.var_axis;

			}

			if (roinfo == &chinfo->row) new_flags |=
				GNUI_ORIENTABLE_POSITION_FLAG_IS_L_START;
			if (roinfo->first_row) new_flags |=
				GNUI_ORIENTABLE_POSITION_FLAG_IS_P_START;
			if (!chinfo->next || chinfo->next->row.starts_here) new_flags |=
				GNUI_ORIENTABLE_POSITION_FLAG_IS_L_END;
			if (roinfo->last_row) new_flags |=
				GNUI_ORIENTABLE_POSITION_FLAG_IS_P_END;

			gnui_flow_chinfo_update_style_classes(chinfo, new_flags);

			gtk_widget_size_allocate(
				child,
				&child_allocation,
				-1 /*chinfo->baselines.var_axis*/
			);

			new_flags &=
				GNUI_ORIENTABLE_POSITION_FLAG_IS_VERTICAL |
				GNUI_ORIENTABLE_POSITION_FLAG_L_RTL |
				GNUI_ORIENTABLE_POSITION_FLAG_P_RTL;

		} while ((chinfo = chinfo->next) && !chinfo->row.starts_here);

		next_offset.fix_axis = 0;
		next_offset.var_axis += nexta.var_size + priv->leading;

		if (p_fill_evenly || roinfo->vexpand) {

			p_n_exp--;

		}

		memset(roinfo, 0, sizeof(GnuiFlowRowInfo));

	} while (chinfo);

	#undef l_n_larger_exp_quota
	#undef l_exp_quota
	#undef l_n_exp
	#undef next_offset

}


/**

    gnui_flow_layout_get_request_mode:
    @manager:   (auto) (not nullable): The flow layout manager
    @widget:    (auto) (unused): The widget currently adopting the flow layout
                manager

    Class handler for the #GtkWidget.get_request_mode() method on the flow
    layout manager instance

    Returns:    The current `GtkSizeRequestMode`

**/
static GtkSizeRequestMode gnui_flow_layout_get_request_mode (
	GtkLayoutManager * const manager,
	GtkWidget * const widget G_GNUC_UNUSED
) {

	return (
		(GnuiFlowLayoutPrivate *) gnui_flow_layout_get_instance_private(
			GNUI_FLOW_LAYOUT(manager)
		)
	)->orientation == GTK_ORIENTATION_VERTICAL ?
			GTK_SIZE_REQUEST_WIDTH_FOR_HEIGHT
		:
			GTK_SIZE_REQUEST_HEIGHT_FOR_WIDTH;

}


/**

    gnui_flow_layout_get_property:
    @object:    (auto) (not nullable): The flow layout manager passed as
                `GObject`
    @prop_id:   (auto): The id of the property to retrieve
    @value:     (auto) (out): The `GValue` that must be returned
    @pspec:     (auto) (not nullable): The `GParamSpec` of the property

    Class handler for `g_object_get()` on the flow layout manager instance

**/
static void gnui_flow_layout_get_property (
	GObject * const object,
	const guint prop_id,
	GValue * const value,
	GParamSpec * const pspec
) {

	GnuiFlowLayout * const self = GNUI_FLOW_LAYOUT(object);

	GnuiFlowLayoutPrivate * const priv =
		gnui_flow_layout_get_instance_private(self);

	switch (prop_id) {

		case FLOW_LAYOUT_PROPERTY_LEADING:

			g_value_set_int(value, priv->leading);
			return;

		case FLOW_LAYOUT_PROPERTY_PAGE_DIRECTION:

			g_value_set_enum(value, priv->page_direction);
			return;

		case FLOW_LAYOUT_PROPERTY_PAGE_JUSTIFY:

			g_value_set_enum(value, priv->page_justify);
			return;

		case FLOW_LAYOUT_PROPERTY_LINE_DIRECTION:

			g_value_set_enum(value, priv->line_direction);
			return;

		case FLOW_LAYOUT_PROPERTY_ORIENTATION:

			g_value_set_enum(value, priv->orientation);
			return;

		case FLOW_LAYOUT_PROPERTY_LINE_JUSTIFY:

			g_value_set_enum(value, priv->line_justify);
			return;

		case FLOW_LAYOUT_PROPERTY_SPACING:

			g_value_set_int(value, priv->spacing);
			return;

		default:

			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			return;

	}

}


/**

    gnui_flow_layout_set_property:
    @object:    (auto) (not nullable): The flow layout manager passed as
                `GObject`
    @prop_id:   (auto): The id of the property to set
    @value:     (auto): The `GValue` containing the new value assigned to the
                property
    @pspec:     (auto) (not nullable): The `GParamSpec` of the property

    Class handler for `g_object_set()` on the flow layout manager instance

**/
static void gnui_flow_layout_set_property (
	GObject * const object,
	const guint prop_id,
	const GValue * const value,
	GParamSpec * const pspec
) {

	GnuiFlowLayoutPrivate * const priv =
		gnui_flow_layout_get_instance_private(GNUI_FLOW_LAYOUT(object));

	GtkWidget * const widget =
		gtk_layout_manager_get_widget(GTK_LAYOUT_MANAGER(object));

	GnuiFlowLayoutProperty notify_id = FLOW_LAYOUT_RESERVED_PROPERTY;

	union {
		gint d;
		guint u;
	} val;

	switch (prop_id) {

		case FLOW_LAYOUT_PROPERTY_LEADING:

			if (priv->leading == (val.d = g_value_get_int(value))) {

				return;

			}

			priv->leading = val.d;
			notify_id = FLOW_LAYOUT_PROPERTY_LEADING;
			break;

		case FLOW_LAYOUT_PROPERTY_PAGE_DIRECTION:

			if (priv->page_direction == (val.u = g_value_get_enum(value))) {

				return;

			}

			priv->page_direction = val.u;
			notify_id = FLOW_LAYOUT_PROPERTY_PAGE_DIRECTION;
			break;

		case FLOW_LAYOUT_PROPERTY_PAGE_JUSTIFY:

			if (priv->page_justify == (val.u = g_value_get_enum(value))) {

				return;

			}

			priv->page_justify = val.u;
			notify_id = FLOW_LAYOUT_PROPERTY_PAGE_JUSTIFY;
			break;

		case FLOW_LAYOUT_PROPERTY_ORIENTATION:

			gnui_flow_layout_update_orientation(
				GNUI_FLOW_LAYOUT(object),
				widget,
				g_value_get_enum(value)
			);

			/*  Keep `return` here!  */
			return;

		case FLOW_LAYOUT_PROPERTY_LINE_DIRECTION:

			if (priv->line_direction == (val.u = g_value_get_enum(value))) {

				return;

			}

			priv->line_direction = val.u;
			notify_id = FLOW_LAYOUT_PROPERTY_LINE_DIRECTION;
			break;

		case FLOW_LAYOUT_PROPERTY_LINE_JUSTIFY:

			if (priv->line_justify == (val.u = g_value_get_enum(value))) {

				return;

			}

			priv->line_justify = val.u;
			notify_id = FLOW_LAYOUT_PROPERTY_LINE_JUSTIFY;
			break;

		case FLOW_LAYOUT_PROPERTY_SPACING:

			if (priv->spacing == (val.d = g_value_get_int(value))) {

				return;

			}

			priv->spacing = val.d;
			notify_id = FLOW_LAYOUT_PROPERTY_SPACING;
			break;

		default:

			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);

			/*  Keep `return` here!  */
			return;

	}

	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(object));
	g_object_notify_by_pspec(object, flow_layout_props[notify_id]);

}


/**

    gnui_flow_layout_class_init:
    @klass:     (auto) (not nullable): The `GObject` klass

    The init function of the flow layout manager class

**/
static void gnui_flow_layout_class_init (
	GnuiFlowLayoutClass * const klass
) {

	GObjectClass * const object_class = G_OBJECT_CLASS(klass);

	GtkLayoutManagerClass * const layout_class =
		GTK_LAYOUT_MANAGER_CLASS(klass);

	object_class->set_property = gnui_flow_layout_set_property;
	object_class->get_property = gnui_flow_layout_get_property;

	layout_class->measure = gnui_flow_layout_measure;
	layout_class->allocate = gnui_flow_layout_allocate;
	layout_class->get_request_mode = gnui_flow_layout_get_request_mode;
	layout_class->layout_child_type = GNUI_TYPE_FLOW_CHILD_LAYOUT;

	g_object_class_override_property(
		object_class,
		FLOW_LAYOUT_PROPERTY_ORIENTATION,
		"orientation"
	);

	flow_layout_props[FLOW_LAYOUT_PROPERTY_PAGE_DIRECTION] = g_param_spec_enum(
		"page-direction",
		"GtkTextDirection",
		"The direction of child placement perpendicularly to the "
			"\"orientation\" axis",
		GTK_TYPE_TEXT_DIRECTION,
		GTK_TEXT_DIR_LTR,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	flow_layout_props[FLOW_LAYOUT_PROPERTY_PAGE_JUSTIFY] = g_param_spec_enum(
		"page-justify",
		"GtkJustification",
		"The alignment of the lines of children widgets relative to each "
			"other; this does not affect the alignment of the flow widget "
			"within its allocation \342\200\223 see \"halign\" and \"valign\" "
			"for that",
		GTK_TYPE_JUSTIFICATION,
		GTK_JUSTIFY_LEFT,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	flow_layout_props[FLOW_LAYOUT_PROPERTY_LINE_DIRECTION] = g_param_spec_enum(
		"line-direction",
		"GtkTextDirection",
		"The direction of child placement within the given \"orientation\" "
			"axis",
		GTK_TYPE_TEXT_DIRECTION,
		GTK_TEXT_DIR_LTR,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	flow_layout_props[FLOW_LAYOUT_PROPERTY_LINE_JUSTIFY] = g_param_spec_enum(
		"line-justify",
		"GtkJustification",
		"The alignment of the children relative to each other within each "
			"line; this does not affect the alignment of the flow widget "
			"within its allocation \342\200\223 see \"halign\" and \"valign\" "
			"for that",
		GTK_TYPE_JUSTIFICATION,
		GTK_JUSTIFY_LEFT,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	flow_layout_props[FLOW_LAYOUT_PROPERTY_LEADING] = g_param_spec_int(
		"leading",
		"gint",
		"Space between rows of children perpendicularly to the "
			"\"orientation\" axis",
		G_MININT,
		G_MAXINT,
		0,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	flow_layout_props[FLOW_LAYOUT_PROPERTY_SPACING] = g_param_spec_int(
		"spacing",
		"gint",
		"Space between children along the \"orientation\" axis",
		G_MININT,
		G_MAXINT,
		0,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	g_object_class_install_properties(
		object_class,
		N_FLOW_LAYOUT_PROPERTIES,
		flow_layout_props
	);

}


/**

    gnui_flow_child_layout_class_init:
    @klass:     (auto) (unused): The `GObject` klass

    The init function of the flow child layout manager class

**/
static void gnui_flow_child_layout_class_init (
	GnuiFlowChildLayoutClass * const klass G_GNUC_UNUSED
) {

}


/**

    gnui_flow_layout_init:
    @self:      (auto) (unused): The newly allocated flow layout manager

    The init function of the flow layout manager instance

**/
static void gnui_flow_layout_init (
	GnuiFlowLayout * const self G_GNUC_UNUSED
) {

}


/**

    gnui_flow_child_layout_init:
    @self:      (auto) (unused): The newly allocated flow child layout manager

    The init function of the flow child layout manager instance

**/
static void gnui_flow_child_layout_init (
	GnuiFlowChildLayout * const self G_GNUC_UNUSED
) {

}



/*  GnuiFlow  */


/**

    gnui_flow_dispose:
    @object:    (auto) (not nullable): The flow widget passed as `GObject`

    Class handler for the #Object.dispose() method on the flow widget instance

**/
static void gnui_flow_dispose (
	GObject * const object
) {

	GtkWidget * child;

	while ((child = gtk_widget_get_first_child(GTK_WIDGET(object)))) {

		gtk_widget_unparent(child);

	}

	G_OBJECT_CLASS(gnui_flow_parent_class)->dispose(object);

}


/**

    gnui_flow_compute_expand:
    @flow:      (auto) (not nullable): The flow widget
    @hexpand    (auto) (not nullable) (out): A pointer for saving the
				horizontal expansion boolean
    @vexpand    (auto) (not nullable) (out): A pointer for saving the vertical
                expansion boolean

    Class handler for the #GtkWidget.compute_expand() method on the flow widget
    instance

**/
static void gnui_flow_compute_expand (
	GtkWidget * const flow,
	gboolean * const hexpand,
	gboolean * const vexpand
) {

	bool is_vertical = (
		(GnuiFlowLayoutPrivate *) gnui_flow_layout_get_instance_private(
			GNUI_FLOW_LAYOUT(
				gtk_widget_get_layout_manager(GTK_WIDGET(flow))
			)
		)
	)->orientation == GTK_ORIENTATION_VERTICAL;

	*hexpand = !is_vertical || gtk_widget_get_hexpand(flow);
	*vexpand = is_vertical || gtk_widget_get_vexpand(flow);

}


/**

    gnui_flow_get_property:
    @object:    (auto) (not nullable): The flow widget passed as `GObject`
    @prop_id:   (auto): The id of the property to retrieve
    @value:     (auto) (out): The `GValue` that must be returned
    @pspec:     (auto) (not nullable): The `GParamSpec` of the property

    Class handler for `g_object_get()` on the flow widget instance

**/
static void gnui_flow_get_property (
	GObject * const object,
	const guint prop_id,
	GValue * const value,
	GParamSpec * const pspec
) {

	GnuiFlowLayoutPrivate * const layout =
		gnui_flow_layout_get_instance_private(
			GNUI_FLOW_LAYOUT(gtk_widget_get_layout_manager(GTK_WIDGET(object)))
		);

	switch (prop_id) {

		case FLOW_PROPERTY_LEADING:

			g_value_set_int(value, layout->leading);
			return;

		case FLOW_PROPERTY_PAGE_DIRECTION:

			g_value_set_enum(value, layout->page_direction);
			return;

		case FLOW_PROPERTY_PAGE_JUSTIFY:

			g_value_set_enum(value, layout->page_justify);
			return;

		case FLOW_PROPERTY_LINE_DIRECTION:

			g_value_set_enum(value, layout->line_direction);
			return;

		case FLOW_PROPERTY_ORIENTATION:

			g_value_set_enum(value, layout->orientation);
			return;

		case FLOW_PROPERTY_LINE_JUSTIFY:

			g_value_set_enum(value, layout->line_justify);
			return;

		case FLOW_PROPERTY_SPACING:

			g_value_set_int(value, layout->spacing);
			return;

		default:

			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			return;

	}

}


/**

    gnui_flow_set_property:
    @object:    (auto) (not nullable): The flow widget passed as `GObject`
    @prop_id:   (auto): The id of the property to set
    @value:     (auto): The `GValue` containing the new value assigned to the
                property
    @pspec:     (auto) (not nullable): The `GParamSpec` of the property

    Class handler for `g_object_set()` on the flow widget instance

**/
static void gnui_flow_set_property (
	GObject * const object,
	const guint prop_id,
	const GValue * const value,
	GParamSpec * const pspec
) {

	GtkLayoutManager * const manager =
		gtk_widget_get_layout_manager(GTK_WIDGET(object));

	GnuiFlowLayoutPrivate * const layout =
		gnui_flow_layout_get_instance_private(GNUI_FLOW_LAYOUT(manager));

	GnuiFlowLayoutProperty m_prop_id = FLOW_LAYOUT_RESERVED_PROPERTY;

	union {
		gint d;
		guint u;
	} val;

	switch (prop_id) {

		case FLOW_PROPERTY_LEADING:

			if (layout->leading == (val.d = g_value_get_int(value))) {

				return;

			}

			layout->leading = val.d;
			m_prop_id = FLOW_LAYOUT_PROPERTY_LEADING;
			break;

		case FLOW_PROPERTY_PAGE_DIRECTION:

			if (layout->page_direction == (val.u = g_value_get_enum(value))) {

				return;

			};

			layout->page_direction = val.u;
			m_prop_id = FLOW_LAYOUT_PROPERTY_PAGE_DIRECTION;
			break;

		case FLOW_PROPERTY_PAGE_JUSTIFY:

			if (layout->page_justify == (val.u = g_value_get_enum(value))) {

				return;

			};

			layout->page_justify = val.u;
			m_prop_id = FLOW_LAYOUT_PROPERTY_PAGE_JUSTIFY;
			break;

		case FLOW_PROPERTY_ORIENTATION:

			gnui_flow_layout_update_orientation(
				GNUI_FLOW_LAYOUT(manager),
				GTK_WIDGET(object),
				g_value_get_enum(value)
			);

			/*  Keep `return` here!  */
			return;

		case FLOW_PROPERTY_LINE_DIRECTION:

			if (layout->line_direction == (val.u = g_value_get_enum(value))) {

				return;

			}

			layout->line_direction = val.u;
			m_prop_id = FLOW_LAYOUT_PROPERTY_LINE_DIRECTION;
			break;

		case FLOW_PROPERTY_LINE_JUSTIFY:

			if (layout->line_justify == (val.u = g_value_get_enum(value))) {

				return;

			}

			layout->line_justify = val.u;
			m_prop_id = FLOW_LAYOUT_PROPERTY_LINE_JUSTIFY;
			break;

		case FLOW_PROPERTY_SPACING:

			if (layout->spacing == (val.d = g_value_get_int(value))) {

				return;

			}

			layout->spacing = val.d;
			m_prop_id = FLOW_LAYOUT_PROPERTY_SPACING;
			break;

		default:

			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);

			/*  Keep `return` here!  */
			return;

	}

	gtk_layout_manager_layout_changed(manager);

	/*  Note that we notify only the layout manager here! The widget will be
		notified afterwards by `gnui_flow__on_manager_property_changed()`  */
	g_object_notify_by_pspec(G_OBJECT(manager), flow_layout_props[m_prop_id]);

}


/**

    gnui_flow__on_manager_property_changed:
    @manager_object:    (auto) (unused): The flow widget's layout manager
                        passed as `GObject`
    @pspec:             (auto) (non-nullanle): The `GParamSpec` of the property
    @v_flow:            (auto) (non-nullanle): The flow widget passed as
                        `gpointer`

    Event handler for the flow widget's layout manager `"notify"` event

**/
void gnui_flow__on_manager_property_changed (
	GObject * const manager_object G_GNUC_UNUSED,
	GParamSpec * const pspec,
	const gpointer v_flow
) {

	GnuiFlowProperty w_prop_id;
	guint m_prop_id = FLOW_LAYOUT_RESERVED_PROPERTY;

	while (
		++m_prop_id < N_FLOW_LAYOUT_PROPERTIES &&
		flow_layout_props[m_prop_id] != pspec
	);

	if (m_prop_id == N_FLOW_LAYOUT_PROPERTIES) {

		/*  There is only one overridden property, it can only be that...  */

		g_object_notify(G_OBJECT(v_flow), "orientation");
		return;

	}

	switch (m_prop_id) {

		case FLOW_LAYOUT_PROPERTY_LEADING:

			w_prop_id = FLOW_PROPERTY_LEADING;
			break;

		case FLOW_LAYOUT_PROPERTY_PAGE_DIRECTION:

			w_prop_id = FLOW_PROPERTY_PAGE_DIRECTION;
			break;

		case FLOW_LAYOUT_PROPERTY_PAGE_JUSTIFY:

			w_prop_id = FLOW_PROPERTY_PAGE_JUSTIFY;
			break;

		case FLOW_LAYOUT_PROPERTY_LINE_DIRECTION:

			w_prop_id = FLOW_PROPERTY_LINE_DIRECTION;
			break;

		case FLOW_LAYOUT_PROPERTY_LINE_JUSTIFY:

			w_prop_id = FLOW_PROPERTY_LINE_JUSTIFY;
			break;

		case FLOW_LAYOUT_PROPERTY_SPACING:

			w_prop_id = FLOW_PROPERTY_SPACING;
			break;

	}

	g_object_notify_by_pspec(G_OBJECT(v_flow), flow_props[w_prop_id]);

}


/**

    gnui_flow_class_init:
    @klass:     (auto) (not nullable): The `GObject` klass

    The init function of the flow widget class

**/
static void gnui_flow_class_init (
	GnuiFlowClass * const klass
) {

	GObjectClass * const object_class = G_OBJECT_CLASS(klass);
	GtkWidgetClass * const widget_class = GTK_WIDGET_CLASS(klass);

	object_class->dispose = gnui_flow_dispose;
	object_class->get_property = gnui_flow_get_property;
	object_class->set_property = gnui_flow_set_property;

	widget_class->compute_expand = gnui_flow_compute_expand;

	g_object_class_override_property(
		object_class,
		FLOW_PROPERTY_ORIENTATION,
		"orientation"
	);

	flow_props[FLOW_PROPERTY_PAGE_DIRECTION] = g_param_spec_enum(
		"page-direction",
		"GtkTextDirection",
		"The direction of child placement perpendicularly to the "
			"\"orientation\" axis",
		GTK_TYPE_TEXT_DIRECTION,
		GTK_TEXT_DIR_LTR,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	flow_props[FLOW_PROPERTY_PAGE_JUSTIFY] = g_param_spec_enum(
		"page-justify",
		"GtkJustification",
		"The alignment of the lines of children widgets relative to each "
			"other; this does not affect the alignment of the flow widget "
			"within its allocation \342\200\223 see \"halign\" and \"valign\" "
			"for that",
		GTK_TYPE_JUSTIFICATION,
		GTK_JUSTIFY_LEFT,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	flow_props[FLOW_PROPERTY_LINE_DIRECTION] = g_param_spec_enum(
		"line-direction",
		"GtkTextDirection",
		"The direction of child placement within the given \"orientation\" "
			"axis",
		GTK_TYPE_TEXT_DIRECTION,
		GTK_TEXT_DIR_LTR,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	flow_props[FLOW_PROPERTY_LINE_JUSTIFY] = g_param_spec_enum(
		"line-justify",
		"GtkJustification",
		"The alignment of the children relative to each other within each "
			"line; this does not affect the alignment of the flow widget "
			"within its allocation \342\200\223 see \"halign\" and \"valign\" "
			"for that",
		GTK_TYPE_JUSTIFICATION,
		GTK_JUSTIFY_LEFT,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	flow_props[FLOW_PROPERTY_LEADING] = g_param_spec_int(
		"leading",
		"gint",
		"Space between rows of children perpendicularly to the "
			"\"orientation\" axis",
		G_MININT,
		G_MAXINT,
		0,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	flow_props[FLOW_PROPERTY_SPACING] = g_param_spec_int(
		"spacing",
		"gint",
		"Space between children along the \"orientation\" axis",
		G_MININT,
		G_MAXINT,
		0,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	g_object_class_install_properties(
		object_class,
		N_FLOW_PROPERTIES,
		flow_props
	);

	gtk_widget_class_set_css_name(widget_class, I_("flow"));

	gtk_widget_class_set_layout_manager_type(
		widget_class,
		GNUI_TYPE_FLOW_LAYOUT
	);

}


/**

    gnui_flow_layout_init:
    @self:      (auto) (not nullable): The newly allocated flow widget

    The init function of the flow widget instance

**/
static void gnui_flow_init (
	GnuiFlow * const self
) {

	g_signal_connect(
		gtk_widget_get_layout_manager(GTK_WIDGET(self)),
		"notify",
		G_CALLBACK(gnui_flow__on_manager_property_changed),
		self
	);

	gtk_widget_add_css_class(GTK_WIDGET(self), "horizontal");

}



/*   GtkBuildable interface  */


/**

    gnui_flow_buildable_add_child:
    @buildable  (auto) (not nullable): The flow widget passed as `GtkBuildable`
    @builder    (auto) (not nullable): The `GtkBuilder` object
    @child      (auto) (not nullable): The child to add
    @type       (auto) (nullable): A string identifying the child type

    Class handler for the #GtkBuildable.add_child() method on the flow's
    buildable interface

**/
static void gnui_flow_buildable_add_child (
	GtkBuildable * const buildable,
	GtkBuilder * const builder,
	GObject * const child,
	const char * const type
) {

	if (GTK_IS_WIDGET(child)) {

		gnui_flow_append(GNUI_FLOW(buildable), GTK_WIDGET(child));

	} else {

		parent_buildable_iface->add_child(buildable, builder, child, type);

	}

}


/**

    gnui_flow_buildable_init:
    @iface      (auto) (not nullable): The flow's buildable interface

    The init function of the flow's buildable interface

**/
static void gnui_flow_buildable_init (
	GtkBuildableIface * const iface
) {

	parent_buildable_iface = g_type_interface_peek_parent(iface);
	iface->add_child = gnui_flow_buildable_add_child;

}



/*\
|*|
|*| PUBLIC FUNCTIONS
|*|
|*| (See the public header for the documentation)
|*|
\*/


/*  GnuiFlowLayout  */


gint gnui_flow_layout_get_spacing (
	GnuiFlowLayout * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW_LAYOUT(self), 0);

	return (
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->spacing;

}


void gnui_flow_layout_set_spacing (
	GnuiFlowLayout * const self,
	const gint spacing
) {

	g_return_if_fail(GNUI_IS_FLOW_LAYOUT(self));

	gint * const current = &(
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->spacing;

	if (*current == spacing) {

		return;

	}

	*current = spacing;
	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(self));

	g_object_notify_by_pspec(
		G_OBJECT(self),
		flow_layout_props[FLOW_LAYOUT_PROPERTY_SPACING]
	);

}


gint gnui_flow_layout_get_leading (
	GnuiFlowLayout * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW_LAYOUT(self), 0);

	return (
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->leading;

}


void gnui_flow_layout_set_leading (
	GnuiFlowLayout * const self,
	const gint leading
) {

	g_return_if_fail(GNUI_IS_FLOW_LAYOUT(self));

	gint * const current = &(
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->leading;

	if (*current == leading) {

		return;

	}

	*current = leading;
	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(self));

	g_object_notify_by_pspec(
		G_OBJECT(self),
		flow_layout_props[FLOW_LAYOUT_PROPERTY_LEADING]
	);

}


GtkTextDirection gnui_flow_layout_get_page_direction (
	GnuiFlowLayout * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW_LAYOUT(self), GTK_TEXT_DIR_NONE);

	return (
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->page_direction;

}


void gnui_flow_layout_set_page_direction (
	GnuiFlowLayout * const self,
	const GtkTextDirection direction
) {

	g_return_if_fail(GNUI_IS_FLOW_LAYOUT(self));

	GtkTextDirection * const current = &(
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->page_direction;

	if (*current == direction) {

		return;

	}

	*current = direction;
	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(self));

	g_object_notify_by_pspec(
		G_OBJECT(self),
		flow_layout_props[FLOW_LAYOUT_PROPERTY_PAGE_DIRECTION]
	);

}


GtkJustification gnui_flow_layout_get_page_justify (
	GnuiFlowLayout * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW_LAYOUT(self), GTK_JUSTIFY_LEFT);

	return (
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->page_justify;

}


void gnui_flow_layout_set_page_justify (
	GnuiFlowLayout * const self,
	const GtkJustification justification
) {

	g_return_if_fail(GNUI_IS_FLOW_LAYOUT(self));

	GtkJustification * const current = &(
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->page_justify;

	if (*current == justification) {

		return;

	}

	*current = justification;
	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(self));

	g_object_notify_by_pspec(
		G_OBJECT(self),
		flow_layout_props[FLOW_LAYOUT_PROPERTY_PAGE_JUSTIFY]
	);

}


GtkOrientation gnui_flow_layout_get_orientation (
	GnuiFlowLayout * const self
) {

	g_return_val_if_fail(
		GNUI_IS_FLOW_LAYOUT(self),
		GTK_ORIENTATION_HORIZONTAL
	);

	return (
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->orientation;

}


void gnui_flow_layout_set_orientation (
	GnuiFlowLayout * const self,
	const GtkOrientation orientation
) {

	g_return_if_fail(GNUI_IS_FLOW_LAYOUT(self));

	gnui_flow_layout_update_orientation(
		GNUI_FLOW_LAYOUT(gtk_widget_get_layout_manager(GTK_WIDGET(self))),
		GTK_WIDGET(self),
		orientation
	);

}


GtkTextDirection gnui_flow_layout_get_line_direction (
	GnuiFlowLayout * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW_LAYOUT(self), GTK_TEXT_DIR_NONE);

	return (
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->line_direction;

}


void gnui_flow_layout_set_line_direction (
	GnuiFlowLayout * const self,
	const GtkTextDirection direction
) {

	g_return_if_fail(GNUI_IS_FLOW_LAYOUT(self));

	GtkTextDirection * const current = &(
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->line_direction;

	if (*current == direction) {

		return;

	}

	*current = direction;
	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(self));

	g_object_notify_by_pspec(
		G_OBJECT(self),
		flow_layout_props[FLOW_LAYOUT_PROPERTY_LINE_DIRECTION]
	);

}


GtkJustification gnui_flow_layout_get_line_justify (
	GnuiFlowLayout * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW_LAYOUT(self), GTK_JUSTIFY_LEFT);

	return (
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->line_justify;

}


void gnui_flow_layout_set_line_justify (
	GnuiFlowLayout * const self,
	const GtkJustification justification
) {

	g_return_if_fail(GNUI_IS_FLOW_LAYOUT(self));

	GtkJustification * const current = &(
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(self)
	)->line_justify;

	if (*current == justification) {

		return;

	}

	*current = justification;
	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(self));

	g_object_notify_by_pspec(
		G_OBJECT(self),
		flow_layout_props[FLOW_LAYOUT_PROPERTY_LINE_JUSTIFY]
	);

}


G_GNUC_WARN_UNUSED_RESULT GtkLayoutManager * gnui_flow_layout_new (void) {

	return g_object_new(
		GNUI_TYPE_FLOW_LAYOUT,
		NULL
	);

}



/*  GnuiFlow  */


void gnui_flow_clear (
	GnuiFlow * const self
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	GtkWidget * child;

	while ((child = gtk_widget_get_first_child(GTK_WIDGET(self)))) {

		gtk_widget_unparent(child);

	}

}


void gnui_flow_remove (
	GnuiFlow * const self,
	GtkWidget * const child
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	gtk_widget_unparent(child);

}


void gnui_flow_append (
	GnuiFlow * const self,
	GtkWidget * const widget
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	gtk_widget_insert_before(widget, GTK_WIDGET(self), NULL);

}


void gnui_flow_prepend (
	GnuiFlow * const self,
	GtkWidget * const widget
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	gtk_widget_insert_after(widget, GTK_WIDGET(self), NULL);

}


void gnui_flow_insert (
	GnuiFlow * const self,
	GtkWidget * const widget,
	const gint at_index
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	gint idx = 0;
	GtkWidget * sibling;

	if (at_index < 0) {

		for (
			sibling = gtk_widget_get_last_child(GTK_WIDGET(self));
				sibling && --idx > at_index;
			sibling = gtk_widget_get_prev_sibling(sibling)
		);

		gtk_widget_insert_after(widget, GTK_WIDGET(self), sibling);

	} else {

		for (
			sibling = gtk_widget_get_first_child(GTK_WIDGET(self));
				sibling && idx++ < at_index;
			sibling = gtk_widget_get_next_sibling(sibling)
		);

		gtk_widget_insert_before(widget, GTK_WIDGET(self), sibling);

	}

}


void gnui_flow_insert_child_before (
	GnuiFlow * const self,
	GtkWidget * const widget,
	GtkWidget * const next_sibling
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	g_return_if_fail(
		!next_sibling ||
		gtk_widget_get_parent(next_sibling) == GTK_WIDGET(self)
	);

	gtk_widget_insert_before(widget, GTK_WIDGET(self), next_sibling);

}


void gnui_flow_insert_child_after (
	GnuiFlow * const self,
	GtkWidget * const widget,
	GtkWidget * const previous_sibling
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	g_return_if_fail(
		!previous_sibling ||
		gtk_widget_get_parent(previous_sibling) == GTK_WIDGET(self)
	);

	gtk_widget_insert_after(widget, GTK_WIDGET(self), previous_sibling);

}


G_GNUC_NULL_TERMINATED void gnui_flow_populate_before (
	GnuiFlow * const self,
	GtkWidget * const next_sibling,
	GtkWidget * const widget_1,
	...
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	va_list args;

	gtk_widget_insert_before(widget_1, GTK_WIDGET(self), next_sibling);
	va_start(args, widget_1);

	for (
		GtkWidget * child, * prev = widget_1;
			(child = va_arg(args, GtkWidget *));
		prev = child
	) {

		gtk_widget_insert_after(child, GTK_WIDGET(self), prev);

	}

	va_end(args);

}


G_GNUC_NULL_TERMINATED void gnui_flow_populate_after (
	GnuiFlow * const self,
	GtkWidget * const previous_sibling,
	GtkWidget * const widget_1,
	...
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	va_list args;

	gtk_widget_insert_after(widget_1, GTK_WIDGET(self), previous_sibling);
	va_start(args, widget_1);

	for (
		GtkWidget * child, * prev = widget_1;
			(child = va_arg(args, GtkWidget *));
		prev = child
	) {

		gtk_widget_insert_after(child, GTK_WIDGET(self), prev);

	}

	va_end(args);

}


gint gnui_flow_get_spacing (
	GnuiFlow * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW(self), 0);

	return (
		(GnuiFlowLayoutPrivate *) gnui_flow_layout_get_instance_private(
			GNUI_FLOW_LAYOUT(
				gtk_widget_get_layout_manager(GTK_WIDGET(self))
			)
		)
	)->spacing;

}


void gnui_flow_set_spacing (
	GnuiFlow * const self,
	const gint spacing
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	GnuiFlowLayout * const manager = GNUI_FLOW_LAYOUT(
		gtk_widget_get_layout_manager(GTK_WIDGET(self))
	);

	gint * const current = &(
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(manager)
	)->spacing;

	if (*current == spacing) {

		return;

	}

	*current = spacing;
	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(manager));

	g_object_notify_by_pspec(
		G_OBJECT(manager),
		flow_layout_props[FLOW_LAYOUT_PROPERTY_SPACING]
	);

}


gint gnui_flow_get_leading (
	GnuiFlow * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW(self), 0);

	return (
		(GnuiFlowLayoutPrivate *) gnui_flow_layout_get_instance_private(
			GNUI_FLOW_LAYOUT(
				gtk_widget_get_layout_manager(GTK_WIDGET(self))
			)
		)
	)->leading;

}


void gnui_flow_set_leading (
	GnuiFlow * const self,
	const gint leading
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	GnuiFlowLayout * const manager = GNUI_FLOW_LAYOUT(
		gtk_widget_get_layout_manager(GTK_WIDGET(self))
	);

	gint * const current = &(
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(manager)
	)->leading;

	if (*current == leading) {

		return;

	}

	*current = leading;
	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(manager));

	g_object_notify_by_pspec(
		G_OBJECT(manager),
		flow_layout_props[FLOW_LAYOUT_PROPERTY_LEADING]
	);

}


GtkTextDirection gnui_flow_get_page_direction (
	GnuiFlow * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW(self), GTK_TEXT_DIR_NONE);

	return (
		(GnuiFlowLayoutPrivate *) gnui_flow_layout_get_instance_private(
			GNUI_FLOW_LAYOUT(
				gtk_widget_get_layout_manager(GTK_WIDGET(self))
			)
		)
	)->page_direction;

}


void gnui_flow_set_page_direction (
	GnuiFlow * const self,
	const GtkTextDirection direction
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	GnuiFlowLayout * const manager = GNUI_FLOW_LAYOUT(
		gtk_widget_get_layout_manager(GTK_WIDGET(self))
	);

	GtkTextDirection * const current = &(
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(manager)
	)->page_direction;

	if (*current == direction) {

		return;

	}

	*current = direction;
	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(manager));

	g_object_notify_by_pspec(
		G_OBJECT(manager),
		flow_layout_props[FLOW_LAYOUT_PROPERTY_PAGE_DIRECTION]
	);

}


GtkJustification gnui_flow_get_page_justify (
	GnuiFlow * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW(self), GTK_JUSTIFY_LEFT);

	return (
		(GnuiFlowLayoutPrivate *) gnui_flow_layout_get_instance_private(
			GNUI_FLOW_LAYOUT(
				gtk_widget_get_layout_manager(GTK_WIDGET(self))
			)
		)
	)->page_justify;

}


void gnui_flow_set_page_justify (
	GnuiFlow * const self,
	const GtkJustification justification
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	GnuiFlowLayout * const manager = GNUI_FLOW_LAYOUT(
		gtk_widget_get_layout_manager(GTK_WIDGET(self))
	);

	GtkJustification * const current = &(
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(manager)
	)->page_justify;

	if (*current == justification) {

		return;

	}

	*current = justification;
	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(manager));

	g_object_notify_by_pspec(
		G_OBJECT(manager),
		flow_layout_props[FLOW_LAYOUT_PROPERTY_PAGE_JUSTIFY]
	);

}


GtkOrientation gnui_flow_get_orientation (
	GnuiFlow * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW(self), GTK_ORIENTATION_HORIZONTAL);

	return (
		(GnuiFlowLayoutPrivate *) gnui_flow_layout_get_instance_private(
			GNUI_FLOW_LAYOUT(
				gtk_widget_get_layout_manager(GTK_WIDGET(self))
			)
		)
	)->orientation;

}


void gnui_flow_set_orientation (
	GnuiFlow * const self,
	const GtkOrientation orientation
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	gnui_flow_layout_update_orientation(
		GNUI_FLOW_LAYOUT(gtk_widget_get_layout_manager(GTK_WIDGET(self))),
		GTK_WIDGET(self),
		orientation
	);

}


GtkTextDirection gnui_flow_get_line_direction (
	GnuiFlow * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW(self), GTK_TEXT_DIR_NONE);

	return (
		(GnuiFlowLayoutPrivate *) gnui_flow_layout_get_instance_private(
			GNUI_FLOW_LAYOUT(
				gtk_widget_get_layout_manager(GTK_WIDGET(self))
			)
		)
	)->line_direction;

}


void gnui_flow_set_line_direction (
	GnuiFlow * const self,
	const GtkTextDirection direction
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	GnuiFlowLayout * const manager = GNUI_FLOW_LAYOUT(
		gtk_widget_get_layout_manager(GTK_WIDGET(self))
	);

	GtkTextDirection * const current = &(
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(manager)
	)->line_direction;

	if (*current == direction) {

		return;

	}

	*current = direction;
	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(manager));

	g_object_notify_by_pspec(
		G_OBJECT(manager),
		flow_layout_props[FLOW_LAYOUT_PROPERTY_LINE_DIRECTION]
	);

}


GtkJustification gnui_flow_get_line_justify (
	GnuiFlow * const self
) {

	g_return_val_if_fail(GNUI_IS_FLOW(self), GTK_JUSTIFY_LEFT);

	return (
		(GnuiFlowLayoutPrivate *) gnui_flow_layout_get_instance_private(
			GNUI_FLOW_LAYOUT(
				gtk_widget_get_layout_manager(GTK_WIDGET(self))
			)
		)
	)->line_justify;

}


void gnui_flow_set_line_justify (
	GnuiFlow * const self,
	const GtkJustification justification
) {

	g_return_if_fail(GNUI_IS_FLOW(self));

	GnuiFlowLayout * const manager = GNUI_FLOW_LAYOUT(
		gtk_widget_get_layout_manager(GTK_WIDGET(self))
	);

	GtkJustification * const current = &(
		(GnuiFlowLayoutPrivate *)
			gnui_flow_layout_get_instance_private(manager)
	)->line_justify;

	if (*current == justification) {

		return;

	}

	*current = justification;
	gtk_layout_manager_layout_changed(GTK_LAYOUT_MANAGER(manager));

	g_object_notify_by_pspec(
		G_OBJECT(manager),
		flow_layout_props[FLOW_LAYOUT_PROPERTY_LINE_JUSTIFY]
	);

}


G_GNUC_WARN_UNUSED_RESULT GtkWidget * gnui_flow_new (void) {

	return g_object_new(GNUI_TYPE_FLOW, NULL);

}


G_GNUC_WARN_UNUSED_RESULT GtkWidget * gnui_flow_new_with_params (
	const GtkOrientation orientation,
	const gint spacing,
	const gint leading
) {

	return g_object_new(
		GNUI_TYPE_FLOW,
		"orientation", orientation,
		"spacing", spacing,
		"leading", leading,
		NULL
	);

}


G_GNUC_NULL_TERMINATED G_GNUC_WARN_UNUSED_RESULT
GtkWidget * gnui_flow_new_with_params_and_children (
	const GtkOrientation orientation,
	const gint spacing,
	const gint leading,
	...
) {

	GtkWidget * child, * const self = g_object_new(
		GNUI_TYPE_FLOW,
		"orientation", orientation,
		"spacing", spacing,
		"leading", leading,
		NULL
	);

	va_list args;

	va_start(args, leading);

	while ((child = va_arg(args, GtkWidget *))) {

		gtk_widget_insert_before(child, GTK_WIDGET(self), NULL);

	}

	va_end(args);
	return self;

}


/*  EOF  */

