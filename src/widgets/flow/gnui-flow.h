/*  -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */

/*\
|*|
|*| gnui-flow.h
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



#ifndef _GNUI_FLOW_H_
#define _GNUI_FLOW_H_


#include <gtk/gtk.h>
#include "gnui-flow-layout.h"


G_BEGIN_DECLS


/**

    SECTION:gnui-flow
    @title: GnuiFlow
    @short_description: A flow container
    @section_id: gnui-flow
    @see_also: #GnuiFlowLayout
    @stability: Unstable
    @include: gnuisance/gnui-flow.h
    @image: gnui-flow-screenshot.png

    `GnuiFlow` is a container widget that allocates its children widgets like
    words in a text.

    Most of the machinery that allows `GnuiFlow` to behave the way it behaves
    is done by its layout manager, `GnuiFlowLayout` (in fact, widgets that
    adopt `GnuiFlowLayout` as their layout manager will behave like
    `GnuiFlow`). However `GnuiFlow` offers a rich set of convenience functions
    for populating its content.

    `GnuiFlow` does not align its children widgets in columns like `GtkFlowBox`
    does. If you need that kind of table-like look, please refer to
    `GtkFlowBox`.

**/


/**

    GNUI_TYPE_FLOW:

    The `GType` of `GnuiFlow`

**/
#define GNUI_TYPE_FLOW (gnui_flow_get_type())


/**

    GnuiFlow:

    The `GnuiFlow` widget

**/
G_DECLARE_DERIVABLE_TYPE(GnuiFlow, gnui_flow, GNUI, FLOW, GtkWidget)


/**

    GnuiFlowClass:

    The flow widget's class

**/
struct _GnuiFlowClass {
    GtkWidgetClass parent_class;
};


/**

    gnui_flow_new:

    Create a new flow container widget

    Returns:    The newly created flow container

**/
extern GtkWidget * gnui_flow_new (void) G_GNUC_WARN_UNUSED_RESULT;


/**

    gnui_flow_new_with_params:
    @orientation:   The flow container's orientation
    @spacing:       The spacing between the children of the flow container in
                    pixels (can be negative for overlapping widgets)
    @leading:       The leading between the children of the flow container in
                    pixels (can be negative for overlapping widgets)

    Create a new flow container widget with parameters

    Returns:    The newly created flow container

**/
extern GtkWidget * gnui_flow_new_with_params (
    const GtkOrientation orientation,
    const gint spacing,
    const gint leading
) G_GNUC_WARN_UNUSED_RESULT;


/**

    gnui_flow_new_with_params_and_children:
    @orientation:   The flow container's orientation
    @spacing:       The spacing between the children of the flow container in
                    pixels (can be negative for overlapping widgets)
    @leading:       The leading between the children of the flow container in
                    pixels (can be negative for overlapping widgets)
    @...:           Zero or more widgets to insert as children in the newly
                    created flow container, followed by `NULL`

    Create a new flow container widget with parameters and children

    Example:

    |[<!-- language="C" -->
    gnui_flow_new_with_params_and_children(
        GTK_ORIENTATION_HORIZONTAL,
        6,
        6,
        gtk_label_new("Child one"),
        gtk_label_new("Child two"),
        gtk_label_new("Child three"),
        gtk_label_new("etc."),
        NULL
    )
    ]|

    Returns:    The newly created flow container

**/
GtkWidget * gnui_flow_new_with_params_and_children (
    const GtkOrientation orientation,
    const gint spacing,
    const gint leading,
    ...
) G_GNUC_NULL_TERMINATED G_GNUC_WARN_UNUSED_RESULT;


/**

    gnui_flow_append:
    @self:      (not nullable): The flow container
    @widget:    (not nullable): The child to append

    Append a widget as the last child of a flow container

**/
extern void gnui_flow_append (
    GnuiFlow * const self,
    GtkWidget * const widget
);


/**

    gnui_flow_prepend:
    @self:      (not nullable): The flow container
    @widget:    (not nullable): The child to prepend

    Prepend a widget as the first child of a flow container

**/
extern void gnui_flow_prepend (
    GnuiFlow * const self,
    GtkWidget * const widget
);


/**

    gnui_flow_remove:
    @self:      (not nullable): The flow container
    @widget:    (not nullable): The child to prepend

    Remove a child from a flow container

**/
extern void gnui_flow_remove (
    GnuiFlow * const self,
    GtkWidget * const child
);


/**

    gnui_flow_insert:
    @self:      (not nullable): The flow container
    @widget:    (not nullable): The child to insert
    @at_index   The index at which the widget must be inserted; if negative the
                counting will be from the end, with `-1` representing the last
                child

    Insert a widget as the nth child of a flow container

**/
extern void gnui_flow_insert (
    GnuiFlow * const self,
    GtkWidget * const widget,
    const gint at_index
);


/**

    gnui_flow_insert_child_before:
    @self:          (not nullable): The flow container
    @widget:        (not nullable): The child to insert
    @next_sibling:  (nullable): The flow child before which the widget must be
                    inserted; if `NULL` the widget will be appended as the last
                    child

    Insert a widget as the nth child of a flow container

**/
extern void gnui_flow_insert_child_before (
    GnuiFlow * const self,
    GtkWidget * const widget,
    GtkWidget * const next_sibling
);


/**

    gnui_flow_insert_child_after:
    @self:              (not nullable): The flow container
    @widget:            (not nullable): The child to insert
    @previous_sibling:  (nullable): The flow child after which the widget must
                        be inserted; if `NULL` the widget will be prepended as
                        the first child

    Insert a widget in a flow container after an existing child

**/
extern void gnui_flow_insert_child_after (
    GnuiFlow * const self,
    GtkWidget * const widget,
    GtkWidget * const previous_sibling
);


/**

    gnui_flow_populate_before:
    @self:          (not nullable): The flow container
    @widget:        (not nullable): The child to insert
    @next_sibling:  (nullable): The flow child before which the widgets must be
                    inserted; if `NULL` the widgets will be appended as the
                    last children
    @...:           Zero or more widgets to insert after @widget_1 but before
                    @next_sibling, followed by `NULL`

    Insert a group of widgets in a flow container before an existing child

**/
extern void gnui_flow_populate_before (
    GnuiFlow * const self,
    GtkWidget * const next_sibling,
    GtkWidget * const widget_1,
    ...
) G_GNUC_NULL_TERMINATED;


/**

    gnui_flow_populate_after:
    @self:              (not nullable): The flow container
    @widget:            (not nullable): The child to insert
    @previous_sibling:  (nullable): The flow child after which the widgets must
                        be inserted; if `NULL` the widgets will be prepended as
                        the first children
    @...:               Zero or more widgets to insert after @widget_1,
                        followed by `NULL`

    Insert a group of widgets in a flow container after an existing child

**/
extern void gnui_flow_populate_after (
    GnuiFlow * const self,
    GtkWidget * const previous_sibling,
    GtkWidget * const widget_1,
    ...
) G_GNUC_NULL_TERMINATED;


/**

    gnui_flow_clear:
    @self:      (not nullable): The flow container

    Remove all the children nested inside a flow container

**/
extern void gnui_flow_clear (
    GnuiFlow * const self
);


/**

    gnui_flow_get_spacing: (get-property spacing)
    @self:      (not nullable): The flow container

    Get the spacing between the children of a flow container in pixels

    Returns:    The spacing between the children of the flow container in
                pixels (can be negative for overlapping widgets)

**/
extern gint gnui_flow_get_spacing (
    GnuiFlow * const self
);


/**

    gnui_flow_set_spacing: (set-property spacing)
    @self:      (not nullable): The flow container
    @spacing:   The spacing between the children of the flow container in
                pixels (can be negative for overlapping widgets)

    Set the spacing between the children of a flow container in pixels

**/
extern void gnui_flow_set_spacing (
    GnuiFlow * const self,
    const gint spacing
);


/**

    gnui_flow_get_leading: (get-property leading)
    @self:      (not nullable): The flow container

    Get the leading between the children of a flow container in pixels

    Returns:    The leading between the children of the flow container in
                pixels (can be negative for overlapping widgets)

**/
extern gint gnui_flow_get_leading (
    GnuiFlow * const self
);

/**

    gnui_flow_set_leading: (set-property leading)
    @self:      (not nullable): The flow container
    @leading:   The leading between the children of the flow container in
                pixels (can be negative for overlapping widgets)

    Set the leading between the children of the flow container in pixels

**/
extern void gnui_flow_set_leading (
    GnuiFlow * const self,
    const gint leading
);


/**

    gnui_flow_get_page_direction: (get-property page-direction)
    @self:      (not nullable): The flow container

    Get the flow container's page direction

    Returns:    The flow container's page direction

**/
extern GtkTextDirection gnui_flow_get_page_direction (
    GnuiFlow * const self
);


/**

    gnui_flow_set_page_direction: (set-property page-direction)
    @self:      (not nullable): The flow container
    @direction: The flow container's page direction

    Set the flow container's page direction

**/
extern void gnui_flow_set_page_direction (
    GnuiFlow * const self,
    const GtkTextDirection direction
);


/**

    gnui_flow_get_page_justify: (get-property page-justify)
    @self:      (not nullable): The flow container

    Get the flow container's page justification

    Returns:    The flow container's page justification

**/
extern GtkJustification gnui_flow_get_page_justify (
    GnuiFlow * const self
);


/**

    gnui_flow_set_page_justify: (set-property page-justify)
    @self:          (not nullable): The flow container
    @justification: The flow container's page justification

    Set the flow container's page justification

**/
extern void gnui_flow_set_page_justify (
    GnuiFlow * const self,
    const GtkJustification justification
);


/**

    gnui_flow_get_orientation: (get-property orientation)
    @self:      (not nullable): The flow container

    Get the flow container's orientation

    Returns:    The flow container's orientation

**/
extern GtkOrientation gnui_flow_get_orientation (
    GnuiFlow * const self
);


/**

    gnui_flow_set_orientation: (set-property orientation)
    @self:          (not nullable): The flow container
    @orientation:   The flow container's justification

    Set the flow container's orientation

**/
extern void gnui_flow_set_orientation (
    GnuiFlow * const self,
    const GtkOrientation orientation
);


/**

    gnui_flow_get_line_direction: (get-property line-direction)
    @self:      (not nullable): The flow container

    Get the flow container's line direction

    Returns:    The flow container's line direction

**/
extern GtkTextDirection gnui_flow_get_line_direction (
    GnuiFlow * const self
);


/**

    gnui_flow_set_line_direction: (set-property line-direction)
    @self:      (not nullable): The flow container
    @direction: The flow container's line direction

    Set the flow container's line direction

**/
extern void gnui_flow_set_line_direction (
    GnuiFlow * const self,
    const GtkTextDirection direction
);


/**

    gnui_flow_get_line_justify: (get-property line-justify)
    @self:      (not nullable): The flow container

    Get the flow container's line justification

    Returns:    The flow container's line justification

**/
extern GtkJustification gnui_flow_get_line_justify (
    GnuiFlow * const self
);


/**

    gnui_flow_set_line_justify: (set-property line-justify)
    @self:          (not nullable): The flow container
    @justification: The flow container's line justification

    Set the flow container's line justification

**/
extern void gnui_flow_set_line_justify (
    GnuiFlow * const self,
    const GtkJustification justification
);


G_END_DECLS


#endif


/*  EOF  */
