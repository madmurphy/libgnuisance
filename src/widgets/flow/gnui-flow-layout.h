/*  -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */

/*\
|*|
|*| gnui-flow-layout.h
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



#ifndef _GNUI_FLOW_LAYOUT_H_
#define _GNUI_FLOW_LAYOUT_H_


#include <gtk/gtk.h>


G_BEGIN_DECLS


/**

    SECTION:gnui-flow-layout
    @title: GnuiFlowLayout
    @short_description: A reusable flow layout manager
    @section_id: gnui-flow-layout
    @see_also: #GnuiFlow, #GnuiTaggedEntry
    @stability: Unstable
    @include: gnuisance/gnui-flow-layout.h
    @image: gnui-flow-screenshot.png

    `GnuiFlowLayout` is a reusable layout manager that allocates widgets like
    words in a text (exactly like `GnuiFlow` does).

    You would normally use it in custom widgets derived directly from
    `GtkWidget`, assigning `GnuiFlowLayout` to them as layout manager:

    |[<!-- language="C" -->
    static void xyz_foo_bar_class_init (
        XyzFooBarClass * const klass
    ) {

        // ...

        GtkWidgetClass * const widget_class = GTK_WIDGET_CLASS(klass);

        // ...

        gtk_widget_class_set_layout_manager_type(
            widget_class,
            GNUI_TYPE_FLOW_LAYOUT
        );

        // ...

    }
    ]|

**/


/**

    GNUI_TYPE_FLOW_LAYOUT:

    The `GType` of `GnuiFlowLayout`

**/
#define GNUI_TYPE_FLOW_LAYOUT (gnui_flow_layout_get_type())


/**

    GNUI_TYPE_FLOW_CHILD_LAYOUT:

    The `GType` of `GnuiFlowChildLayout`

**/
#define GNUI_TYPE_FLOW_CHILD_LAYOUT (gnui_flow_child_layout_get_type())


/**

    GnuiFlowLayout:

    The `GnuiFlowLayout` layout manager

**/
G_DECLARE_DERIVABLE_TYPE(
    GnuiFlowLayout,
    gnui_flow_layout,
    GNUI,
    FLOW_LAYOUT,
    GtkLayoutManager
)


/**

    GnuiFlowChildLayout:

    The `GnuiFlowChildLayout` layout child

**/
G_DECLARE_DERIVABLE_TYPE(
    GnuiFlowChildLayout,
    gnui_flow_child_layout,
    GNUI,
    FLOW_CHILD_LAYOUT,
    GtkLayoutChild
)


/**

    GnuiFlowChildLayoutClass:

    The flow child layout's class

**/
struct _GnuiFlowChildLayoutClass {
    GtkLayoutChildClass parent_class;
};


/**

    GnuiFlowLayoutClass:

    The flow layout manager's class

**/
struct _GnuiFlowLayoutClass {
    GtkLayoutManagerClass parent_class;
};


/**

    gnui_flow_layout_new:

    Create a new flow layout manager

    Returns:    The newly created flow layout manager

**/
GtkLayoutManager * gnui_flow_layout_new (void) G_GNUC_WARN_UNUSED_RESULT;


/**

    gnui_flow_layout_get_spacing: (get-property spacing)
    @self:      (not nullable): The flow layout manager


    Get the flow layout manager's spacing between children in pixels

    Returns:    The flow layout manager's spacing between children in pixels
                (can be negative for overlapping widgets)

**/
extern gint gnui_flow_layout_get_spacing (
    GnuiFlowLayout * const self
);


/**

    gnui_flow_layout_set_spacing: (set-property spacing)
    @self:      (not nullable): The flow layout manager
    @spacing:   The flow layout manager's spacing between children in pixels
                (can be negative for overlapping widgets)

    Set the flow layout manager's spacing between children in pixels

**/
extern void gnui_flow_layout_set_spacing (
    GnuiFlowLayout * const self,
    const gint spacing
);


/**

    gnui_flow_layout_get_leading: (get-property leading)
    @self:      (not nullable): The flow layout manager

    Get the flow layout manager's leading between children in pixels

    Returns:    The flow layout manager's leading between children in pixels
                (can be negative for overlapping widgets)

**/
extern gint gnui_flow_layout_get_leading (
    GnuiFlowLayout * const self
);


/**

    gnui_flow_layout_set_leading: (set-property leading)
    @self:      (not nullable): The flow layout manager
    @leading:   The flow layout manager's leading between children in pixels
                (can be negative for overlapping widgets)

    Set the flow layout manager's leading between children in pixels

**/
extern void gnui_flow_layout_set_leading (
    GnuiFlowLayout * const self,
    const gint leading
);


/**

    gnui_flow_layout_get_page_direction: (get-property page-direction)
    @self:      (not nullable): The flow layout manager

    Get the flow layout manager's page direction

    Returns:    The flow layout manager's page direction

**/
extern GtkTextDirection gnui_flow_layout_get_page_direction (
    GnuiFlowLayout * const self
);


/**

    gnui_flow_layout_set_page_direction: (set-property page-direction)
    @self:      (not nullable): The flow layout manager
    @direction: The flow layout manager's page direction

    Set the flow layout manager's page direction

**/
extern void gnui_flow_layout_set_page_direction (
    GnuiFlowLayout * const self,
    const GtkTextDirection direction
);


/**

    gnui_flow_layout_get_page_justify: (get-property page-justify)
    @self:      (not nullable): The flow layout manager

    Get the flow layout manager's page justification
    
    Returns:    The flow layout manager's page justification

**/
extern GtkJustification gnui_flow_layout_get_page_justify (
    GnuiFlowLayout * const self
);


/**

    gnui_flow_layout_set_page_justify: (set-property page-justify)
    @self:          (not nullable): The flow layout manager
    @justification: The flow layout manager's page justification

    Set the flow layout manager's page justification

**/
extern void gnui_flow_layout_set_page_justify (
    GnuiFlowLayout * const self,
    const GtkJustification justification
);


/**

    gnui_flow_layout_get_orientation: (get-property orientation)
    @self:      (not nullable): The flow layout manager

    Get the flow layout manager's orientation

    Returns:    The flow layout manager's orientation

**/
extern GtkOrientation gnui_flow_layout_get_orientation (
    GnuiFlowLayout * const self
);


/**

    gnui_flow_layout_set_orientation: (set-property orientation)
    @self:          (not nullable): The flow layout manager
    @orientation:   The flow layout manager's orientation

    Set the flow layout manager's orientation

**/
extern void gnui_flow_layout_set_orientation (
    GnuiFlowLayout * const self,
    const GtkOrientation orientation
);


/**

    gnui_flow_layout_get_line_direction: (get-property line-direction)
    @self:          (not nullable): The flow layout manager

    Get the flow layout manager's line direction

    Returns:    The flow layout manager's line direction

**/
extern GtkTextDirection gnui_flow_layout_get_line_direction (
    GnuiFlowLayout * const self
);


/**

    gnui_flow_layout_set_line_direction: (set-property line-direction)
    @self:      (not nullable): The flow layout manager
    @direction: The flow layout manager's line direction

    Set the flow layout manager's line direction

**/
extern void gnui_flow_layout_set_line_direction (
    GnuiFlowLayout * const self,
    const GtkTextDirection direction
);


/**

    gnui_flow_layout_get_line_justify: (get-property line-justify)
    @self:      (not nullable): The flow layout manager

    Get the flow layout manager's line justification

    Returns:    The flow layout manager's line justification

**/
extern GtkJustification gnui_flow_layout_get_line_justify (
    GnuiFlowLayout * const self
);


/**

    gnui_flow_layout_set_line_justify: (set-property line-justify)
    @self:          (not nullable): The flow layout manager
    @justification: The flow layout manager's line justification

    Set the flow layout manager's line justification                                    

**/
extern void gnui_flow_layout_set_line_justify (
    GnuiFlowLayout * const self,
    const GtkJustification justification
);


G_END_DECLS


#endif


/*  EOF  */
