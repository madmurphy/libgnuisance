/*  -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */

/*\
|*|
|*| gnui-tagged-entry.h
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



#ifndef _GNUI_TAGGED_ENTRY_H_
#define _GNUI_TAGGED_ENTRY_H_


#include <gtk/gtk.h>


G_BEGIN_DECLS


/**

    SECTION:gnui-tagged-entry
    @title: GnuiTaggedEntry
    @short_description: A tagged entry
    @section_id: gnui-tagged-entry
    @see_also: #GtkEntry, #GnuiFlowLayout
    @stability: Unstable
    @include: gnuisance/gnui-tagged-entry.h
    @image: gnui-tagged-entry-screenshot.png

    `GnuiTaggedEntry` is a text entry widget that lets the user provide an
    array of strings (namely tags).

**/


/**

    GNUI_TYPE_TAGGED_ENTRY:

    The `GType` of `GnuiTaggedEntry`

**/
#define GNUI_TYPE_TAGGED_ENTRY (gnui_tagged_entry_get_type())


/**

    GnuiTaggedEntry:

    The `GnuiTaggedEntry` widget

**/
G_DECLARE_FINAL_TYPE(
    GnuiTaggedEntry,
    gnui_tagged_entry,
    GNUI,
    TAGGED_ENTRY,
    GtkWidget
)



/*  Callback types  */


/**

    GnuiTaggedEntryFilterFunc:
    @self:      (auto) (not nullable): The tagged entry
    @tag:       (auto) (not nullable) (non-zero length) (transfer none): The
                tag to check
    @user_data: (auto) (nullable) (closure): The #GnuiTaggedEntry:filter-data
                property

    A function type for determining whether a tag can be added

    It is granted that the strings passed to the filter function are always
    non-`NULL` and with a length greater than zero, therefore the filter
    function must not check for `NULL` or `""`.

    Returns:    `true` if the tag must be added, `false` if it must be
                filtered out

**/
typedef gboolean (* GnuiTaggedEntryFilterFunc) (
    GnuiTaggedEntry * self,
    const gchar * tag,
    gpointer user_data
);


/**

    GnuiTaggedEntryMatchFunc:
    @entry_tag: (auto) (not nullable) (non-zero length) (transfer none): A tag
                from the tagged entry
    @other_tag: (auto) (not nullable) (non-zero length) (transfer none): A
                string to compare with @entry_tag
    @user_data: (auto) (nullable) (closure): The #GnuiTaggedEntry:match-data
                property

    A function type for determining whether two tags match

    The first argument passed to the match function is always a tag already in
    the entry, while the second argument is what is being checked for equality.

    It is granted that the strings passed to the match function are always
    non-`NULL` and with a length greater than zero, therefore the match
    function must not check for `NULL` or `""`.

    Returns:    `true` if the two strings match, `false` otherwise

**/
typedef gboolean (* GnuiTaggedEntryMatchFunc) (
    const gchar * entry_tag,
    const gchar * other_tag,
    gpointer user_data
);


/**

    GnuiTaggedEntrySanitizeFunc:
    @self:      (auto) (not nullable): The tagged entry
    @tag:       (auto) (not nullable) (non-zero length) (transfer none): The
                tag to sanitize
    @user_data: (auto) (nullable) (closure): The #GnuiTaggedEntry:sanitize-data
                property

    A function type for allocating the strings that are added to the tagged
    entry as tags based on the given input

    It is granted that the strings passed to the sanitize function are always
    non-`NULL` and with a length greater than zero, therefore the sanitize
    function must not check for `NULL` or `""`.

    If @tag must not be inserted this function must return `NULL`.

    Returns:    (transfer full) (nullable): A newly allocated string

**/
typedef gchar * (* GnuiTaggedEntrySanitizeFunc) (
    GnuiTaggedEntry * self,
    const gchar * tag,
    gpointer user_data
);


/**

    GnuiTaggedEntrySortFunc:
    @tag_1:     (auto) (not nullable) (non-zero length) (transfer none): The
                first tag
    @tag_2:     (auto) (not nullable) (non-zero length) (transfer none): The
                second tag
    @user_data: (auto) (nullable) (closure): The #GnuiTaggedEntry:sort-data
                property

    A function type for sorting the tags added to the tagged entry

    It is granted that the strings passed to the sort function are always
    non-`NULL` and with a length greater than zero, therefore the sort function
    must not check for `NULL` or `""`.

    Returns:    Zero or a negative number if the order is @tag_1 followed by
                @tag_2, a positive number otherwise

**/
typedef gint (* GnuiTaggedEntrySortFunc) (
    const gchar * tag_1,
    const gchar * tag_2,
    gpointer user_data
);



/*  Functions  */


/**

    gnui_tagged_entry_new:

    Create a new tagged entry widget

    Returns:    The newly created tagged entry widget

**/
extern GtkWidget * gnui_tagged_entry_new (void) G_GNUC_WARN_UNUSED_RESULT;


/**

    gnui_tagged_entry_has_tag:
    @self:      (not nullable): The tagged entry to query
    @tag:       (nullable): A string matching a tag to find

    Get whether a tagged entry has a particular tag

    A tagged entry must always have a match function; if the
    #GnuiTaggedEntry:match-function property is set to `NULL` literal equality
    checks between strings will be performed in order to find the given tag.

    Returns:    `true` if the tagged entry has the given tag, `false` otherwise

**/
extern gboolean gnui_tagged_entry_has_tag (
    GnuiTaggedEntry * const self,
    const gchar * const tag
);


/**

    gnui_tagged_entry_remove_tag:
    @self:      (not nullable): The tagged entry to modify
    @tag:       (not nullable): A string matching a tag to remove from the
                tagged entry

    Remove a particular tag from a tagged entry

    A tagged entry must always have a match function; if the
    #GnuiTaggedEntry:match-function property is set to `NULL` literal equality
    checks between strings will be performed in order to find the tag to
    remove.

    Returns:    `true` if the tagged entry had the tag, `false` otherwise

**/
extern gboolean gnui_tagged_entry_remove_tag (
    GnuiTaggedEntry * const self,
    const gchar * const tag
);


/**

    gnui_tagged_entry_remove_all_tags:
    @self:      (not nullable): The tagged entry to clear

    Remove all the tags from a tagged entry

**/
extern void gnui_tagged_entry_remove_all_tags (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_populate:
    @self:      (not nullable): The tagged entry to populate
    @pinned:    Whether the tags added must be pinned
    @...:       Zero or more strings to insert into the tagged entry as tags,
                followed by `NULL` (if no strings are provided the tagged entry
                will only be cleared)

    Clear and populate a tagged entry

    Returns:    `true` if all the tags were successfully added, `false`
                otherwise

**/
extern gboolean gnui_tagged_entry_populate (
    GnuiTaggedEntry * const self,
    const gboolean pinned,
    ...
) G_GNUC_NULL_TERMINATED;


/**

    gnui_tagged_entry_populate_strv:
    @self:      (not nullable): The tagged entry to populate
    @tags:      (transfer none) (nullable): An array containing zero or more
                strings to insert into the tagged entry as tags, followed by
                `NULL` (if no strings are provided, or if the array is `NULL`,
                the tagged entry will only be cleared)
    @pinned:    Whether the tags added must be pinned

    Clear and populate a tagged entry, using an array of strings as input

    Returns:    `true` if all the tags were successfully added, `false`
                otherwise

**/
extern gboolean gnui_tagged_entry_populate_strv (
    GnuiTaggedEntry * const self,
    const gchar * const * const tags,
    const gboolean pinned
);


/**

    gnui_tagged_entry_add_tag:
    @self:      (not nullable): The tagged entry to populate
    @tag:       (not nullable): A string to insert into the tagged entry as tag
    @pinned:    Whether the tag added must be pinned

    Add a a tag to a tagged entry

    Returns:    `true` if the tag was successfully added, `false` otherwise

**/
extern gboolean gnui_tagged_entry_add_tag (
    GnuiTaggedEntry * const self,
    const gchar * const tag,
    const gboolean pinned
);


/**

    gnui_tagged_entry_add_tags
    @self:      (not nullable): The tagged entry to populate
    @pinned:    Whether the tags inserted must be pinned
    @...:       Zero or more strings to insert into the tagged entry as tags,
                followed by `NULL` (if no strings are provided the function is
                no-op)

    Add a group of tags to a tagged entry

    Returns:    `true` if all the tags were successfully added, `false`
                otherwise

**/
extern gboolean gnui_tagged_entry_add_tags (
    GnuiTaggedEntry * const self,
    const gboolean pinned,
    ...
) G_GNUC_NULL_TERMINATED;


/**

    gnui_tagged_entry_parse_expression:
    @self:          (not nullable): The tagged entry to populate
    @expression:    (not nullable): A delimited string containing one or more
                    tags to add to the tagged entry                                                                 []
    @pinned:        Whether the tags inserted must be pinned

    Parse a delimited string and add its tags to a tagged entry

    Returns:    `true` if all the tags were successfully added, `false`
                otherwise

**/
extern gsize gnui_tagged_entry_parse_expression (
    GnuiTaggedEntry * const self,
    const gchar * const expression,
    const gboolean pinned
);


/**

    gnui_tagged_entry_rename_tag:
    @self:      (not nullable): The tagged entry to modify
    @old_name:  (not nullable): A string matching a tag to rename in the tagged
                entry
    @new_name:  (not nullable): The new value for the tag

    Rename a particular tag in a tagged entry

    A tagged entry must always have a match function; if the
    #GnuiTaggedEntry:match-function property is set to `NULL`, in order to find
    the tag to rename literal equality checks between strings will be
    performed.

    Returns:    `true` if the tagged entry had the tag, `false` otherwise

**/
extern gboolean gnui_tagged_entry_rename_tag (
    GnuiTaggedEntry * const self,
    const gchar * const old_name,
    const gchar * const new_name
);


/**

    gnui_tagged_entry_set_pin:
    @self:      (not nullable): The tagged entry
    @tag:       (not nullable): A string matching a tag to pin or unpin in the
                tagged entry
    @pinned:    Whether the state of the tag must be marked as pinned

    Pin/unpin a given tag

    A tagged entry must always have a match function; if the
    #GnuiTaggedEntry:match-function property is set to `NULL`, in order to find
    the tag to pin/unpin literal equality checks between strings will be
    performed.

    Returns:    `true` if the tag is found, `false` otherwise

**/
extern gboolean gnui_tagged_entry_set_pin (
    GnuiTaggedEntry * const self,
    const gchar * const tag,
    const gboolean pinned
);


/**

    gnui_tagged_entry_set_pins:
    @self:      (not nullable): The tagged entry
    @pinned:    Whether the state of the tag must be marked as pinned
    @...:       Zero or more strings matching tags in the tagged entry to
                pin/unpin, followed by `NULL` (if no strings are provided the
                function is no-op)

    Pin/unpin a group of tags

    A tagged entry must always have a match function; if the
    #GnuiTaggedEntry:match-function property is set to `NULL`, in order to find
    the tag to pin/unpin literal equality checks between strings will be
    performed.

    Returns:    `true` if all the tags were found, `false` otherwise

**/
extern gboolean gnui_tagged_entry_set_pins (
    GnuiTaggedEntry * const self,
    const gboolean pinned,
    ...
) G_GNUC_NULL_TERMINATED;


/**

    gnui_tagged_entry_activate:
    @self:      (not nullable): The tagged entry to activate

    Activate a tagged entry (similar to pressing `ENTER`)

**/
extern void gnui_tagged_entry_activate (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_get_delimiter_chars:  (get-property delimiter-chars)
    @self:      (not nullable): The tagged entry

    Get the delimiter characters of a tagged entry

    Returns:    (transfer none): A `NIL`-terminated array of delimiter
                characters (i.e. a “C string”), or `NULL` if the tagged entry
                has no delimiters. The returned array must not be freed or
                modified.

**/
extern const gchar * gnui_tagged_entry_get_delimiter_chars (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_delimiter_chars: (set-property delimiter-chars)
    @self:              (not nullable): The tagged entry
    @delimiter_chars:   (transfer none) (nullable): A `NIL`-terminated array of
                        characters (i.e. a "C string") to use as input
                        delimiters between different tags, or `NULL` for
                        setting no delimiters

    Set the delimiter characters of a tagged entry

    If no delimiters are set, new tags can be added only by activating the
    tagged entry (usually by pressing `ENTER`).

**/
extern void gnui_tagged_entry_set_delimiter_chars (
    GnuiTaggedEntry * const self,
    const gchar * delimiter_chars
);


/**

    gnui_tagged_entry_get_filter_data: (get-property filter-data)
    @self:      (not nullable): The tagged entry

    Get the closure data passed to the #GnuiTaggedEntry:filter-function

    Returns:    The closure data passed to the #GnuiTaggedEntry:filter-function

**/
extern gpointer gnui_tagged_entry_get_filter_data (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_filter_data: (set-property filter-data)
    @self:          (not nullable): The tagged entry
    @filter_data    (nullable): The closure data to pass to the
                    #GnuiTaggedEntry:filter-function

    Set the closure data passed to the #GnuiTaggedEntry:filter-function

    If later you need to destroy @filter_data, connect the address to the
    tagged entry's #Object::destroy signal.

    |[<!-- language="C" -->
    g_signal_connect_swapped(
        my_tagged_entry,
        "destroy",
        G_CALLBACK(my_destroy_listener),
        filter_data
    );
    ]|

**/
extern void gnui_tagged_entry_set_filter_data (
    GnuiTaggedEntry * const self,
    const gpointer filter_data
);


/**

    gnui_tagged_entry_get_filter_function: (get-property filter-function)
    @self:      (not nullable): The tagged entry

    Get the function that determines whether a tag can be added

    Returns:    The filter function

**/
extern GnuiTaggedEntryFilterFunc gnui_tagged_entry_get_filter_function (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_filter_function: (set-property filter-function)
    @self:          (not nullable): The tagged entry
    @filter_func:   (nullable): The filter function

    Set the function that determines whether a tag can be added

    It is granted that the strings passed to the filter function are always
    non-`NULL` and with a length greater than zero, therefore the filter
    function must not check for `NULL` or `""`.

**/
extern void gnui_tagged_entry_set_filter_function (
    GnuiTaggedEntry * const self,
    const GnuiTaggedEntryFilterFunc filter_func
);


/**

    gnui_tagged_entry_get_invalid: (get-property invalid)
    @self:      (not nullable): The tagged entry

    Get whether the tagged entry is in invalid state

    Returns:    `true` if the tagged entry is in invalid state, `false`
                otherwise

**/
extern gboolean gnui_tagged_entry_get_invalid (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_invalid: (set-property invalid)
    @self:      (not nullable): The tagged entry
    @invalid:   `true` if the tagged entry must be set in invalid state,
                `false` otherwise

    Set whether the tagged entry is in invalid state

**/
extern void gnui_tagged_entry_set_invalid (
    GnuiTaggedEntry * const self,
    const gboolean invalid
);


/**

    gnui_tagged_entry_get_match_data: (get-property match-data)
    @self:      (not nullable): The tagged entry

    Get the closure data passed to the #GnuiTaggedEntry:match-function

    Returns:    The closure data passed to the #GnuiTaggedEntry:match-function

**/
extern gpointer gnui_tagged_entry_get_match_data (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_match_data: (set-property match-data)
    @self:          (not nullable): The tagged entry
    @match_data:    (nullable): The closure data to pass to the
                    #GnuiTaggedEntry:match-function

    Set the closure data passed to the #GnuiTaggedEntry:match-function

    If later you need to destroy @match_data, connect the address to the tagged
    entry's #Object::destroy signal.

    |[<!-- language="C" -->
    g_signal_connect_swapped(
        my_tagged_entry,
        "destroy",
        G_CALLBACK(my_destroy_listener),
        match_data
    );
    ]|

**/
extern void gnui_tagged_entry_set_match_data (
    GnuiTaggedEntry * const self,
    const gpointer match_data
);


/**

    gnui_tagged_entry_get_match_function: (get-property match-function)
    @self:      (not nullable): The tagged entry

    Get the function that determines whether two tags match

    Returns:    The match function

**/
extern GnuiTaggedEntryMatchFunc gnui_tagged_entry_get_match_function (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_match_function: (set-property match-function)
    @self:          (not nullable): The tagged entry
    @match_func:    (nullable): The match function

    Set the function that determines whether two tags match

    The first argument passed to the match function is always a tag already in
    the entry, whereas the second argument is what is being checked for
    equality.

    It is granted that the strings passed to the match function are always
    non-`NULL` and with a length greater than zero, therefore the match
    function must not check for `NULL` or `""`.

    A tagged entry must always have a match function; if @match_func is set to
    `NULL` a literal equality check between string will be used as the match
    function.

**/
extern void gnui_tagged_entry_set_match_function (
    GnuiTaggedEntry * const self,
    const GnuiTaggedEntryMatchFunc match_func
);


/**

    gnui_tagged_entry_get_modified: (get-property modified)
    @self:      (not nullable): The tagged entry

    Get whether the tagged entry has been modified by the user

    Returns:    Whether the tagged entry has been modified by the user

**/
extern gboolean gnui_tagged_entry_get_modified (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_modified: (set-property modified)
    @self:      (not nullable): The tagged entry
    @modified:  `true` if the tagged entry's state must be set to "modified",
                `false` otherwise

    Set whether the tagged entry has been modified by the user

**/
extern void gnui_tagged_entry_set_modified (
    GnuiTaggedEntry * const self,
    const gboolean modified
);


/**

    gnui_tagged_entry_get_placeholder_text: (get-property placeholder-text)
    @self:      (not nullable): The tagged entry

    Get the text that must be displayed when the tagged entry is empty

    Returns:    (nullable): The text that must be displayed when the tagged
                entry is empty

**/
extern const char * gnui_tagged_entry_get_placeholder_text (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_placeholder_text: (set-property placeholder-text)
    @self:              (not nullable): The tagged entry
    @placeholder_text:  (transfer none) (nullable): The text that must be
                        displayed when the tagged entry is empty

    Set the text that must be displayed when the tagged entry is empty

**/
extern void gnui_tagged_entry_set_placeholder_text (
    GnuiTaggedEntry * const self,
    const char * const placeholder_text
);


/**

    gnui_tagged_entry_get_sanitize_data: (get-property sanitize-data)
    @self:      (not nullable): The tagged entry

    Get the closure data passed to the #GnuiTaggedEntry:sanitize-function

    Returns:    The closure data passed to the
                #GnuiTaggedEntry:sanitize-function

**/
extern gpointer gnui_tagged_entry_get_sanitize_data (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_sanitize_data: (set-property sanitize-data)
    @self:          (not nullable): The tagged entry
    @sanitize_data  (nullable): The closure data to pass to the
                    #GnuiTaggedEntry:sanitize-function

    Set the closure data passed to the #GnuiTaggedEntry:sanitize-function

    If later you need to destroy @sanitize_data, connect the address to the
    tagged entry's #Object::destroy signal.

    |[<!-- language="C" -->
    g_signal_connect_swapped(
        my_tagged_entry,
        "destroy",
        G_CALLBACK(my_destroy_listener),
        sanitize_data
    );
    ]|

**/
extern void gnui_tagged_entry_set_sanitize_data (
    GnuiTaggedEntry * const self,
    const gpointer sanitize_data
);


/**

    gnui_tagged_entry_get_sanitize_function: (get-property sanitize-function)
    @self:      (not nullable): The tagged entry

    Get the function that allocates the strings that must be added to the
    tagged entry as tags, based on the given input

    It is granted that the strings passed to the sanitize function are always
    non-`NULL` and with a length greater than zero, therefore the sanitize
    function must not check for `NULL` or `""`.

    Returns:    The sanitize function

**/
extern GnuiTaggedEntrySanitizeFunc gnui_tagged_entry_get_sanitize_function (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_sanitize_function: (set-property sanitize-function)
    @self:          (not nullable): The tagged entry
    @sanitize_func: (nullable): The sanitize function

    Set the function that allocates the strings that must be added to the
    tagged entry as tags, based on the given input

    It is granted that the strings passed to the sanitize function are always
    non-`NULL` and with a length greater than zero, therefore the sanitize
    function must not check for `NULL` or `""`.

    If this function set to `NULL`, `g_strdup()` will be used.

**/
extern void gnui_tagged_entry_set_sanitize_function (
    GnuiTaggedEntry * const self,
    const GnuiTaggedEntrySanitizeFunc sanitize_func
);


/**

    gnui_tagged_entry_get_sort_data: (get-property sort-data)
    @self:      (not nullable): The tagged entry

    Get the closure data passed to the #GnuiTaggedEntry:sort-function

    Returns:    The closure data passed to the #GnuiTaggedEntry:sort-function

**/
extern gpointer gnui_tagged_entry_get_sort_data (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_sort_data: (set-property sort-data)
    @self:      (not nullable): The tagged entry
    @sort_data  (nullable): The closure data to pass to the
                #GnuiTaggedEntry:sort-function

    Get the closure data passed to the #GnuiTaggedEntry:sort-function

    If later you need to destroy @sort_data, connect the address to the tagged
    entry's #Object::destroy signal.

    |[<!-- language="C" -->
    g_signal_connect_swapped(
        my_tagged_entry,
        "destroy",
        G_CALLBACK(my_destroy_listener),
        sort_data
    );
    ]|

**/
extern void gnui_tagged_entry_set_sort_data (
    GnuiTaggedEntry * const self,
    const gpointer sort_data
);


/**

    gnui_tagged_entry_get_sort_function: (get-property sort-function)
    @self:      (not nullable): The tagged entry

    Get the function that sorts the tags added to the tagged entry

    Returns:    The #GnuiTaggedEntry:sort-function

**/
extern GnuiTaggedEntrySortFunc gnui_tagged_entry_get_sort_function (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_sort_function: (set-property sort-function)
    @self:      (not nullable): The tagged entry
    @sort_func: (nullable): The function that will sort the tags added to the
                tagged entry

    Set the function that sorts the tags added to the tagged entry

    It is granted that the strings passed to the sort function are always
    non-`NULL` and with a length greater than zero, therefore the sort function
    must not check for `NULL` or `""`.

    Even if `strcmp()` takes less parameters than a `GnuiTaggedEntrySortFunc`,
    you can use `strcmp()` too as sort function.

**/
extern void gnui_tagged_entry_set_sort_function (
    GnuiTaggedEntry * const self,
    const GnuiTaggedEntrySortFunc sort_func
);


/**

    gnui_tagged_entry_get_tags: (get-property tags)
    @self:      (not nullable): The tagged entry

    Get the tagged entry's tags

    Returns:    (transfer none) (nullable): An array of trings containing the
                tagged entry's tags. The returned array must not be freed or
                modified.

**/
extern const gchar * const * gnui_tagged_entry_get_tags (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_set_tags: (set-property tags)
    @self:      (not nullable): The tagged entry
    @tags:      (transfer none) (nullable): Zero or more strings to insert into
                the (tagged entry as tags, followed by `NULL` if no strings are
                provided, or if this argument is `NULL`, the tagged entry will
                only be cleared)

    Set the tagged entry's tags

    The tags added using this function (or, equivalently, using
    `g_object_set_property()`) are always unpinned. For populating a tagged
    entry with pinned tags please use `gnui_tagged_entry_populate_strv()`.

**/
extern void gnui_tagged_entry_set_tags (
    GnuiTaggedEntry * const self,
    const gchar * const * const tags
);


/**

    gnui_tagged_entry_invalidate_filter:
    @self:      (not nullable): The tagged entry

    For each tag in the tagged entry call the #GnuiTaggedEntry:filter-function

**/
extern void gnui_tagged_entry_invalidate_filter (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_invalidate_sort:
    @self:      (not nullable): The tagged entry

    For each tag in the tagged entry call the #GnuiTaggedEntry:sort-function

**/
extern void gnui_tagged_entry_invalidate_sort (
    GnuiTaggedEntry * const self
);


/**

    gnui_tagged_entry_get_text_delegate:
    @self:      (not nullable): The tagged entry

    Get the tagged entry's `GtkText` delegate

    Returns:    The tagged entry's `GtkText` delegate

**/
extern GtkText * gnui_tagged_entry_get_text_delegate (
    GnuiTaggedEntry * const self
);



/*  Signal handler types  */


/**

    GnuiTaggedEntrySignalHandlerTagAdded:
    @self:      (auto) (not nullable): The tagged entry
    @tag:       (auto) (not nullable) (non-zero length) (transfer none): The
                tag added
    @user_data: (auto) (nullable) (closure): The custom data passed to the
                signal

    A handler function type for the #GnuiTaggedEntry::tag-added signal

**/
typedef void (* GnuiTaggedEntrySignalHandlerTagAdded) (
    GnuiTaggedEntry * self,
    const gchar * tag,
    gpointer user_data
);


/**

    GnuiTaggedEntrySignalHandlerTagRemoved:
    @self:      (auto) (not nullable): The tagged entry
    @tag:       (auto) (not nullable) (non-zero length) (transfer none): The
                tag removed
    @user_data: (auto) (nullable) (closure): The custom data passed to the
                signal

    A handler function type for the #GnuiTaggedEntry::tag-removed signal

**/
typedef void (* GnuiTaggedEntrySignalHandlerTagRemoved) (
    GnuiTaggedEntry * self,
    const gchar * tag,
    gpointer user_data
);


/**

    GnuiTaggedEntrySignalHandlerActivate:
    @self:      (auto) (not nullable): The tagged entry
    @user_data: (auto) (nullable) (closure): The custom data passed to the
                signal

    A handler function type for the #GnuiTaggedEntry::activate signal

**/
typedef void (* GnuiTaggedEntrySignalHandlerActivate) (
    GnuiTaggedEntry * self,
    gpointer user_data
);


/**

    GnuiTaggedEntrySignalHandlerInvalidChanged:
    @self:      (auto) (not nullable): The tagged entry
    @invalid:   (auto): The current state of the #GnuiTaggedEntry:invalid
                property
    @user_data: (auto) (nullable) (closure): The custom data passed to the
                signal

    A handler function type for the #GnuiTaggedEntry::invalid-changed signal

**/
typedef void (* GnuiTaggedEntrySignalHandlerInvalidChanged) (
    GnuiTaggedEntry * self,
    gboolean invalid,
    gpointer user_data
);


/**

    GnuiTaggedEntrySignalHandlerModifiedChanged:
    @self:      (auto) (not nullable): The tagged entry
    @modified:  (auto): The current state of the #GnuiTaggedEntry:modified
                property
    @user_data: (auto) (nullable) (closure): The custom data passed to the
                signal

    A handler function type for the #GnuiTaggedEntry::modified-changed signal

**/
typedef void (* GnuiTaggedEntrySignalHandlerModifiedChanged) (
    GnuiTaggedEntry * self,
    gboolean modified,
    gpointer user_data
);


G_END_DECLS


#endif


/*  EOF  */
