/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*  Please make sure that the TAB width in your editor is set to 4 spaces  */

/*\
|*|
|*| gnui-tagged-entry.c
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
#include "gnui-definitions.h"
#include "gnui-internals.h"
#include "gnui-flow-layout.h"
#include "gnui-tagged-entry.h"



/*\
|*|
|*| GLOBAL TYPES AND VARIABLES
|*|
\*/


typedef struct _GnuiTaggedEntryPrivate {
	GList * taglist;
	GtkWidget * textbox;
	gsize tag_count;
	gsize next_id;
	bool tags_have_changed;
} GnuiTaggedEntryPrivate;


typedef struct _TagReference {
	gchar * tag;
	GnuiTaggedEntry * parent;
	GtkWidget
		* box,
		* remover;
	GtkLabel * marker;
	gsize id;
	bool pinned;
} TagReference;


struct _GnuiTaggedEntry {
	GtkWidget parent_instance;
	GnuiTaggedEntryFilterFunc filter_function;
	GnuiTaggedEntryMatchFunc match_function;
	GnuiTaggedEntrySanitizeFunc sanitize_function;
	GnuiTaggedEntrySortFunc sort_function;
	gpointer match_data;
	gpointer sanitize_data;
	gpointer sort_data;
	gpointer filter_data;
	gchar * delimiter_chars;
	gchar ** tags;
	bool
		modified : 1,
		invalid : 1;
};


static void gnui_tagged_entry_editable_iface_init (
	GtkEditableInterface * iface
);


G_DEFINE_FINAL_TYPE_WITH_CODE(
	GnuiTaggedEntry,
	gnui_tagged_entry,
	GTK_TYPE_WIDGET,
	G_ADD_PRIVATE(
		GnuiTaggedEntry
	) G_IMPLEMENT_INTERFACE(
		GTK_TYPE_EDITABLE,
		gnui_tagged_entry_editable_iface_init
	)
)


enum {

	/*  Reserved for GObject  */
	RESERVED_PROPERTY = 0,

	/*  Properties  */
	PROPERTY_DELIMITER_CHARS,
	PROPERTY_FILTER_DATA,
	PROPERTY_FILTER_FUNCTION,
	PROPERTY_INVALID,
	PROPERTY_MATCH_DATA,
	PROPERTY_MATCH_FUNCTION,
	PROPERTY_MODIFIED,
	PROPERTY_PLACEHOLDER_TEXT,
	PROPERTY_SANITIZE_DATA,
	PROPERTY_SANITIZE_FUNCTION,
	PROPERTY_SORT_DATA,
	PROPERTY_SORT_FUNCTION,
	PROPERTY_TAGS,

	/*  Number of properties  */
	N_PROPERTIES,

};


enum {

	/*  Signals  */
	SIGNAL_ACTIVATE,
	SIGNAL_INVALID_CHANGED,
	SIGNAL_MODIFIED_CHANGED,
	SIGNAL_TAG_ADDED,
	SIGNAL_TAG_REMOVED,

	/*  Number of signals  */
	N_SIGNALS

};


static GParamSpec * props[N_PROPERTIES];


static guint signals[N_SIGNALS];



/*\
|*|
|*| PRIVATE FUNCTIONS
|*|
\*/


/*  Inline  */


/**

    gnui_tagged_entry_tag_reference_destroy:
    @self:      (not nullable): The tagged entry
    @tagref:    (transfer full) (not nullable): The `TagReference` to destroy

    Destroy a `TagReference`

**/
static inline void gnui_tagged_entry_tag_reference_destroy (
	GnuiTaggedEntry * const self,
	TagReference * const tagref
) {
	gtk_widget_unparent(tagref->box);
	g_signal_emit(self, signals[SIGNAL_TAG_REMOVED], 0, tagref->tag);
	g_free(tagref->tag);
	g_free(tagref);
}


/**

    gnui_tagged_entry_dispatch_modified:
    @self:      (not nullable): The tagged entry
    @priv:      (not nullable): The tagged entry's private `struct`
    @modified:  The new value for the #GnuiTaggedEntry:modified property

    Change the state of the #GnuiTaggedEntry:modified property and notify it

**/
static inline void gnui_tagged_entry_dispatch_modified (
	GnuiTaggedEntry * const self,
	GnuiTaggedEntryPrivate * const priv,
	const bool modified
) {
	priv->tags_have_changed = true;
	if (self->modified == modified) return;
	self->modified = modified;
	g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_MODIFIED]);
	g_signal_emit(self, signals[SIGNAL_MODIFIED_CHANGED], 0, modified);
}


/**

    gnui_tagged_entry_delete_llnk:
    @self:      (not nullable): The tagged entry
    @priv:      (not nullable): The tagged entry's private `struct`
    @tagllnk:   (transfer full) (not nullable): The list link to delete

    Remove a doubly-linked link containing a `TagReference`

**/
static inline void gnui_tagged_entry_delete_llnk (
	GnuiTaggedEntry * const self,
	GnuiTaggedEntryPrivate * const priv,
	GList * const tagllnk
) {
	gnui_tagged_entry_tag_reference_destroy(self, ((GList *) tagllnk)->data);
	priv->taglist = g_list_delete_link(priv->taglist, tagllnk);
	priv->tag_count--;
	g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_TAGS]);
	gnui_tagged_entry_dispatch_modified(self, priv, true);
}


/**

    gnui_tagged_entry_retrieve_tags:  
    @list:      (not nullable): The tagged entry's tag list
    @llen:      The length of the list

    Retrieve all the tags of a list of `TagReference` objects and return is a
    newly allocated array of strings

    Returns:    A newly allocated array of strings containing the tags

**/
static inline gchar ** gnui_tagged_entry_retrieve_tags (
	const GList * const list,
	const gsize llen
) {
	if (!list) return NULL;
	gsize idx = llen;
	const GList * llnk = list;
	gchar ** const tags = g_new(gchar *, idx + 1);
	tags[idx] = NULL;
	while (idx > 0 && llnk) {
		tags[--idx] = g_strdup(((TagReference *) llnk->data)->tag);
		llnk = llnk->next;
	}
	return tags;
}


/**

    gnui_tagged_entry_sanitize_and_allocate_tag:
    @self:      (not nullable): The tagged entry
    @tagsrc:    (nullable): The source string of the tag to allocate

    Allocate a sanitized tag

    Returns:    A newly allocated tag or `NULL` if @tagsrc is invalid

**/
static inline gchar * gnui_tagged_entry_sanitize_and_allocate_tag (
	GnuiTaggedEntry * const self,
	const gchar * const tagsrc
) {
	return
		!tagsrc || !*tagsrc || (
			self->filter_function &&
			!self->filter_function(self, tagsrc, self->filter_data)
		) ?
			NULL
		: self->sanitize_function ?
			self->sanitize_function(
				self,
				tagsrc,
				self->sanitize_data
			)
		:
			g_strdup(tagsrc);
}



/*  Event listeners and utilities  */


/**

    gnui_tagged_entry__on_remover_click:
    @button:    (unused) (nullable): The remover `GtkButton`
    @v_llnk:    (not nullable): The list link to remove (passed as `gpointer`)

    Event handler for the #GtkButton::clicked event of each tag's remover
    `GtkButton`

**/
static void gnui_tagged_entry__on_remover_click (
	GtkButton * const button G_GNUC_UNUSED,
	const gpointer v_llnk
) {

	if (((TagReference *) ((GList *) v_llnk)->data)->pinned) {

		return;

	}

	GnuiTaggedEntry * const self =
		((TagReference *) ((GList *) v_llnk)->data)->parent;

	gnui_tagged_entry_delete_llnk(
		self,
		gnui_tagged_entry_get_instance_private(self),
		v_llnk
	);

}


/**

    gnui_tagged_entry_insert_tagref_sorted:
    @self:      (not nullable): The tagged entry
    @priv:      (not nullable): The tagged entry's private `struct`
    @tagref:    (not nullable): The tag reference to insert

    Insert a `TagReference` without a parent

    Returns:    The new list link inserted

**/
static GList * gnui_tagged_entry_insert_tagref_sorted (
	GnuiTaggedEntry * const self,
	GnuiTaggedEntryPrivate * const priv,
	TagReference * const tagref
) {

	GList * llink = priv->taglist;

	if (llink && self->sort_function) {


		/* \                                  /\
		\ */     insert_tagref_sorted:       /* \
		 \/     ________________________     \ */


		if (
			self->sort_function(
				((TagReference *) llink->data)->tag,
				tagref->tag,
				self->sort_data
			) > 0
		) {

			if (llink->next) {

				llink = llink->next;
				goto insert_tagref_sorted;

			}

			llink = g_list_append(
				llink,
				tagref
			);

			gtk_widget_insert_after(tagref->box, GTK_WIDGET(self), NULL);
			llink = llink->next;

		} else {

			priv->taglist = g_list_insert_before(priv->taglist, llink, tagref);

			gtk_widget_insert_after(
				tagref->box,
				GTK_WIDGET(self),
				((TagReference *) llink->data)->box
			);

			llink = llink->prev;

		}

	} else {

		priv->taglist = llink = g_list_prepend(llink, tagref);
		gtk_widget_insert_before(tagref->box, GTK_WIDGET(self), priv->textbox);

	}

	return llink;

}


/**

    gnui_tagged_entry_insert_llink_sorted:
    @self:      (not nullable): The tagged entry
    @priv:      (not nullable): The tagged entry's private `struct`
    @tagllnk:   (not nullable): The list link to insert

    Insert a doubly-linked link containing a `TagReference` without a parent

**/
static void gnui_tagged_entry_insert_llink_sorted (
	GnuiTaggedEntry * const self,
	GnuiTaggedEntryPrivate * const priv,
	GList * tagllnk
) {

	GList * sibling = priv->taglist;

	if (sibling && self->sort_function) {


		/* \                                  /\
		\ */     insert_llnk_sorted:         /* \
		 \/     ________________________     \ */


		if (
			self->sort_function(
				((TagReference *) sibling->data)->tag,
				((TagReference *) tagllnk->data)->tag,
				self->sort_data
			) > 0
		) {

			if (sibling->next) {

				sibling = sibling->next;
				goto insert_llnk_sorted;

			}

			sibling->next = tagllnk;
			tagllnk->prev = sibling;
			tagllnk->next = NULL;

			gtk_widget_insert_after(
				((TagReference *) tagllnk->data)->box,
				GTK_WIDGET(self),
				NULL
			);

		} else {

			priv->taglist = g_list_insert_before_link(
				priv->taglist,
				sibling,
				tagllnk
			);

			gtk_widget_insert_after(
				((TagReference *) tagllnk->data)->box,
				GTK_WIDGET(self),
				((TagReference *) sibling->data)->box
			);

		}

	} else {

		priv->taglist = gnui_list_prepend_llink(sibling, tagllnk);

		gtk_widget_insert_before(
			((TagReference *) tagllnk->data)->box,
			GTK_WIDGET(self),
			priv->textbox
		);

	}

}


/**

    gnui_tagged_entry_add_sanitized_tag:
    @self:          (not nullable): The tagged entry
    @priv:          (not nullable): The tagged entry's private `struct`
    @sanitized_tag: (transfer full) (not nullable): The sanitized tag to add
    @pinned:        Whether the added tags must appear as "pinned"

    Add a tag sanitized silently, without notifying GObject

    This function does not perform **any** check.

    If a new tag is successfully added this function will *not* notify GObject
    about changes in the #GnuiTaggedEntry:tags property (this must be done
    manually).

    Atomic, per-tag notifications (the #GnuiTaggedEntry:modified property) and
    signals (#GnuiTaggedEntry::modified-changed and
    #GnuiTaggedEntry::tag-added) will be dispatched normally.

**/
static void gnui_tagged_entry_add_sanitized_tag (
	GnuiTaggedEntry * const self,
	GnuiTaggedEntryPrivate * const priv,
	gchar * const sanitized_tag,
	const gboolean pinned
) {

	GtkWidget
		* const tagbox = g_object_new(
			GTK_TYPE_BOX,
			"orientation", GTK_ORIENTATION_HORIZONTAL,
			"spacing", 6,
			"hexpand", false,
			"vexpand", false,
			"halign", GTK_ALIGN_CENTER,
			NULL
		),
		* _widget_placeholder_;

	TagReference * tagref = g_new(TagReference, 1);

	tagref->parent = self;
	tagref->box = tagbox;
	tagref->tag = sanitized_tag;
	tagref->id = priv->next_id++;
	tagref->pinned = pinned;

	#define label _widget_placeholder_

	label = g_object_new(
		GTK_TYPE_LABEL,
		"label", sanitized_tag,
		"hexpand", true,
		NULL
	);

	gtk_widget_add_css_class(tagbox, "tag");
	gtk_widget_add_css_class(label, "content");
	gtk_box_append(GTK_BOX(tagbox), label);
	tagref->marker = GTK_LABEL(label);

	#undef label
	#define button _widget_placeholder_

	button = g_object_new(
		GTK_TYPE_BUTTON,
		"label", "\303\227",
		"sensitive", !pinned,
		NULL
	);

	gtk_widget_set_cursor_from_name(button, "pointer");
	gtk_widget_add_css_class(button, "remover");
	gtk_box_append(GTK_BOX(tagbox), button);
	tagref->remover = button;

	g_signal_connect(
		button,
		"clicked",
		G_CALLBACK(gnui_tagged_entry__on_remover_click),
		gnui_tagged_entry_insert_tagref_sorted(self, priv, tagref)
	);

	#undef button

	priv->tag_count++;
	g_signal_emit(self, signals[SIGNAL_TAG_ADDED], 0, sanitized_tag);
	gnui_tagged_entry_dispatch_modified(self, priv, true);

}


/**

    gnui_tagged_entry_add_sanitized_tag:
    @self:              (not nullable): The tagged entry
    @priv:              (not nullable): The tagged entry's private `struct`
    @unsanitized_tag:   (transfer none) (nullable): The sanitized tag to add
    @pinned:            Whether the added tags must appear as "pinned"

    Add a tag silently, without notifying GObject

    This function does not perform all checks.

    If a new tag is successfully added this function will *not* notify GObject
    about changes in the #GnuiTaggedEntry:tags property (this must be done
    manually).

    Atomic, per-tag notifications (the #GnuiTaggedEntry:modified property) and
    signals (#GnuiTaggedEntry::modified-changed and
    #GnuiTaggedEntry::tag-added) will be dispatched normally.


    Returns:    `true` if the tag is successfully allocated, `false` otherwise

**/
static inline bool gnui_tagged_entry_sanitize_and_add_tag (
	GnuiTaggedEntry * const self,
	GnuiTaggedEntryPrivate * const priv,
	const gchar * const unsanitized_tag,
	const gboolean pinned
) {
	gchar * const sanitized_tag =
		gnui_tagged_entry_sanitize_and_allocate_tag(self, unsanitized_tag);
	if (!sanitized_tag) return false;
	gnui_tagged_entry_add_sanitized_tag(self, priv, sanitized_tag, pinned);
	return true;
}


/**

    gnui_tagged_entry_tokenize_parse_expression_but_last:
    @self:              (not nullable): The tagged entry
    @priv:              (not nullable): The tagged entry's private `struct`
    @save_insert_len:   (not nullable): A placeholder for saving the length of
                        the fragment successfully parsed. This will be set to
                        @exprlen only if @expression ends with a delimiter.
    @expression:        (not nullable): The expression to parse
    @exprlen:           The length of the expression to parse
    @pinned:            Whether the added tags must appear as "pinned"

    Tokenize and parse a delimited string containing tags, stopping before the
    last tag

    This function does not perform all checks. Before invoking it it is
    important to make sure that `self->delimiters` is not set to `NULL`, or a
    crash will happen.

    If a new tag is successfully added this function will *not* notify GObject
    about changes in the #GnuiTaggedEntry:tags property (this must be done
    manually).

    Atomic, per-tag notifications (the #GnuiTaggedEntry:modified property) and
    signals (#GnuiTaggedEntry::modified-changed and
    #GnuiTaggedEntry::tag-added) will be dispatched normally.

    Returns:    `true` if all the tags but the last one were successfully
                inserted, `false` otherwise

**/
static bool gnui_tagged_entry_tokenize_parse_expression_but_last (
	GnuiTaggedEntry * const self,
	GnuiTaggedEntryPrivate * const priv,
	gsize * const save_insert_len,
	gchar * const expression,
	gsize const exprlen,
	const bool pinned
) {

	const gchar * ptr, * const delimiters = self->delimiter_chars;
	gsize last_pos = 0, idx = 0;
	gchar last_delimiter = '\0';

	while (idx < exprlen) {

		for (ptr = delimiters - 1; *++ptr && *ptr != expression[idx]; );

		if (*ptr != '\0') {

			expression[idx] = '\0';

			if (
				last_delimiter == '\0' &&
				!gnui_tagged_entry_sanitize_and_add_tag(
					self,
					priv,
					expression + last_pos,
					pinned
				)
			) {

				expression[idx] = *ptr;
				*save_insert_len = last_pos;
				return false;

			}

			last_pos = idx + 1;

		}

		last_delimiter = *ptr;
		idx++;

	}

	*save_insert_len = last_pos;
	return true;

}


/**

    gnui_tagged_entry_tokenize_parse_expression:
    @self:              (not nullable): The tagged entry
    @priv:              (not nullable): The tagged entry's private `struct`
    @expression:        (not nullable): The expression to parse
    @exprlen:           The length of the expression to parse
    @pinned:            Whether the added tags must appear as "pinned"

    Tokenize and parse a delimited expression containing tags

    This function does not perform all checks. Before invoking it it is
    important to make sure that `self->delimiters` is not set to `NULL`, or a
    crash will happen.

    If a new tag is successfully added this function will *not* notify GObject
    about changes in the #GnuiTaggedEntry:tags property (this must be done
    manually).

    Atomic, per-tag notifications (the #GnuiTaggedEntry:modified property) and
    signals (#GnuiTaggedEntry::modified-changed and
    #GnuiTaggedEntry::tag-added) will be dispatched normally.

    Returns:    The length of the fragment successfully parsed; this will be
                equal to @exprlen only if all the tags were added

**/
static inline gsize gnui_tagged_entry_tokenize_parse_expression (
	GnuiTaggedEntry * const self,
	GnuiTaggedEntryPrivate * const priv,
	gchar * const expression,
	gsize const exprlen,
	const bool pinned
) {

	gsize last_offset;

	if (

		gnui_tagged_entry_tokenize_parse_expression_but_last(
			self,
			priv,
			&last_offset,
			expression,
			exprlen,
			pinned
		)

		&&

		gnui_tagged_entry_sanitize_and_add_tag(
			self,
			priv,
			expression + last_offset,
			pinned
		)

	) {

		return exprlen;

	}

	return last_offset;

}


/**

    gnui_tagged_entry_private_add_expression:
    @self:              (not nullable): The tagged entry
    @priv:              (not nullable): The tagged entry's private `struct`
    @expression:        (not nullable): The expression to parse
    @exprlen:           The length of the expression to parse
    @pinned:            Whether the added tags must appear as "pinned"

    Parse an expression containing tags to add

    This function does not perform all checks. Before invoking it it is
    important to make sure that `self->delimiters` is not set to `NULL`, or a
    crash will happen.

    Returns:    The length of the fragment successfully parsed; this will be
                equal to @exprlen only if all the tags were added

**/
static inline gsize gnui_tagged_entry_private_add_expression (
	GnuiTaggedEntry * const self,
	GnuiTaggedEntryPrivate * const priv,
	const gchar * const expression,
	gsize const exprlen,
	const bool pinned
) {

	gchar * const probe = g_malloc(exprlen + 1);
	memcpy(probe, expression, exprlen + 1);

	gsize retval = gnui_tagged_entry_tokenize_parse_expression(
		self,
		priv,
		probe,
		exprlen,
		pinned
	);

	g_free(probe);

	if (retval > 0) {

		g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_TAGS]);
		gnui_tagged_entry_dispatch_modified(self, priv, true);

	}

	return retval;

}


/**

    gnui_tagged_entry__on_textbox_change:
    @textbox:   (not nullable): The `GtkText` child
    @v_self:    (not nullable): The tagged entry (passed as `gpointer`)

    Event handler for the `GtkText` child's #GtkText::changed event

**/
static void gnui_tagged_entry__on_textbox_change (
	GtkEditable * const textbox,
	const gpointer v_self
) {

	gchar * _string_placeholder_;

	#define ptr _string_placeholder_

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(GNUI_TAGGED_ENTRY(v_self));

	ptr = GNUI_TAGGED_ENTRY(v_self)->delimiter_chars;

	if (!ptr || !*ptr) {

		goto dispatch_and_exit;

	}

	const gchar * const curr = gtk_editable_get_text(textbox);
	const gsize len = strlen(curr);

	ptr--;

	while (*++ptr && !memchr(curr, *ptr, len));

	if (!*ptr) {

		goto not_applicable;

	}

	#undef ptr
	#define probe _string_placeholder_

	gsize remnant;
	guint pos;

	*((gint *) &pos) = gtk_editable_get_position(textbox);
	GNUI_NORMALIZE_INT_UINT_CAST__2(pos);
	probe = g_malloc(len + 1);
	memcpy(probe, curr, len);

	const bool is_valid = gnui_tagged_entry_tokenize_parse_expression_but_last(
		v_self,
		priv,
		&remnant,
		probe,
		len,
		false
	);

	g_free(probe);

	#undef probe

	if (remnant > 0) {

		gnui_signal_handlers_block_by_func(
			textbox,
			G_CALLBACK(gnui_tagged_entry__on_textbox_change),
			v_self
		);

		gtk_editable_delete_text(
			textbox,
			0,
			remnant == len ? -1 : (gint) remnant
		);

		gtk_editable_set_position(
			textbox,
			pos > remnant ? (gint) (pos - remnant) : 0
		);

		gnui_signal_handlers_unblock_by_func(
			textbox,
			G_CALLBACK(gnui_tagged_entry__on_textbox_change),
			v_self
		);

		g_object_notify_by_pspec(v_self, props[PROPERTY_TAGS]);

	}

	switch (is_valid) {

		case false:

			if (!GNUI_TAGGED_ENTRY(v_self)->invalid) {

				GNUI_TAGGED_ENTRY(v_self)->invalid = true;
				gtk_widget_add_css_class(v_self, "invalid");
				g_object_notify_by_pspec(v_self, props[PROPERTY_INVALID]);

				g_signal_emit(
					v_self,
					signals[SIGNAL_INVALID_CHANGED],
					0,
					true
				);

			}

			break;

		default:
		not_applicable:

			if (GNUI_TAGGED_ENTRY(v_self)->invalid) {

				GNUI_TAGGED_ENTRY(v_self)->invalid = false;
				gtk_widget_remove_css_class(v_self, "invalid");
				g_object_notify_by_pspec(v_self, props[PROPERTY_INVALID]);

				g_signal_emit(
					v_self,
					signals[SIGNAL_INVALID_CHANGED],
					0,
					false
				);

			}

	}


	/* \                                  /\
	\ */     dispatch_and_exit:          /* \
	 \/     ________________________     \ */


	gnui_tagged_entry_dispatch_modified(v_self, priv, true);

}


/**

    gnui_tagged_entry__on_textbox_activate:
    @textbox:   (not nullable): The `GtkText` child
    @v_self:    (not nullable): The tagged entry (passed as `gpointer`)

    Event handler for the `GtkText` child's #GtkText::activate event

    This function is invoked by `gnui_tagged_entry_activate()` as well.

**/
static void gnui_tagged_entry__on_textbox_activate (
	GtkEditable * const textbox,
	const gpointer v_self
) {

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(GNUI_TAGGED_ENTRY(v_self));

	const gchar * const current_text =
		gtk_editable_get_text(GTK_EDITABLE(textbox));

	if (!current_text || !*current_text) {

		goto emit_and_exit;

	}

	const gsize
		current_length = strlen(current_text),
		end_pos =
			GNUI_TAGGED_ENTRY(v_self)->delimiter_chars ?
				gnui_tagged_entry_private_add_expression(
					v_self,
					priv,
					current_text,
					current_length,
					false
				)
			: gnui_tagged_entry_sanitize_and_add_tag(
				v_self,
				priv,
				current_text,
				false
			) ?
				current_length
			:
				0;

	if (end_pos > 0) {

		gtk_editable_delete_text(GTK_EDITABLE(textbox), 0, (gint) end_pos);
		g_object_notify_by_pspec(v_self, props[PROPERTY_TAGS]);
		gnui_tagged_entry_dispatch_modified(v_self, priv, true);

	}

	if (end_pos != current_length && !GNUI_TAGGED_ENTRY(v_self)->invalid) {

		GNUI_TAGGED_ENTRY(v_self)->invalid = true;
		gtk_widget_add_css_class(v_self, "invalid");
		g_object_notify_by_pspec(v_self, props[PROPERTY_INVALID]);
		g_signal_emit(v_self, signals[SIGNAL_INVALID_CHANGED], 0, true);

	}


	/* \                                  /\
	\ */     emit_and_exit:              /* \
	 \/     ________________________     \ */


	g_signal_emit(v_self, signals[SIGNAL_ACTIVATE], 0);

}


/**

    gnui_tagged_entry__on_textbox_backspace:
    @textbox:   (not nullable): The `GtkText` child
    @v_self:    (not nullable): The tagged entry (passed as `gpointer`)

    Event handler for the `GtkText` child's #GtkText::backspace event

**/
static void gnui_tagged_entry__on_textbox_backspace (
	GtkText * const textbox,
	const gpointer v_self
) {

	if (gtk_editable_get_position(GTK_EDITABLE(textbox)) > 0) {

		return;

	}

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(GNUI_TAGGED_ENTRY(v_self));

	if (!priv->taglist) {

		return;

	}

	GList * llnk = priv->taglist, * delllnk = llnk;
	gsize last_id = ((TagReference *) llnk->data)->id;

	g_signal_stop_emission_by_name(textbox, "backspace");

	while ((llnk = llnk->next)) {

		if (((TagReference *) llnk->data)->id > last_id) {

			delllnk = llnk;
			last_id = ((TagReference *) llnk->data)->id;

		}

	}

	gnui_tagged_entry_delete_llnk(v_self, priv, delllnk);

}



/*  Class functions  */


/**

    gnui_tagged_entry_dispose:
    @object:    (auto) (not nullable): The tagged entry passed as `GObject`

    Class handler for the #Object.dispose() method on the tagged entry instance

**/
static void gnui_tagged_entry_dispose (
	GObject * const object
) {

	GnuiTaggedEntry * const self = GNUI_TAGGED_ENTRY(object);

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	GtkWidget * child;

	if (
		((GnuiTaggedEntryPrivate *)
			gnui_tagged_entry_get_instance_private(self))->textbox
	) {

		gtk_editable_finish_delegate(GTK_EDITABLE(self));

	}

	for (const GList * llnk = priv->taglist; llnk; llnk = llnk->next) {

		gtk_widget_unparent(((TagReference *) llnk->data)->box);
		g_free(((TagReference *) llnk->data)->tag);
		g_free(llnk->data);

	}

	while ((child = gtk_widget_get_first_child(GTK_WIDGET(object)))) {

		gtk_widget_unparent(child);

	}

	g_list_free(priv->taglist);
	g_free(GNUI_TAGGED_ENTRY(self)->delimiter_chars);
	g_strfreev(GNUI_TAGGED_ENTRY(self)->tags);
	G_OBJECT_CLASS(gnui_tagged_entry_parent_class)->dispose(object);

}


/**

    gnui_tagged_entry_get_property:
    @object:    (auto) (not nullable): The tagged entry passed as `GObject`
    @prop_id:   (auto): The id of the property to retrieve
    @value:     (auto) (out): The `GValue` that must be returned
    @pspec:     (auto) (not nullable): The `GParamSpec` of the property

    Class handler for `g_object_get()` on the tagged entry instance

**/
static void gnui_tagged_entry_get_property (
	GObject * const object,
	const guint prop_id,
	GValue * const value,
	GParamSpec * const pspec
) {

	if (gtk_editable_delegate_get_property (object, prop_id, value, pspec)) {

		return;

	}

	GnuiTaggedEntry * const self = GNUI_TAGGED_ENTRY(object);

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	switch (prop_id) {

		case PROPERTY_DELIMITER_CHARS:

			g_value_set_string(value, self->delimiter_chars);
			break;

		case PROPERTY_FILTER_DATA:

			g_value_set_pointer(value, self->filter_data);
			break;

		case PROPERTY_FILTER_FUNCTION:

			g_value_set_pointer(value, *((gpointer *) &self->filter_function));
			break;

		case PROPERTY_INVALID:

			g_value_set_boolean(value, self->invalid);
			break;

		case PROPERTY_MATCH_DATA:

			g_value_set_pointer(value, self->match_data);
			break;

		case PROPERTY_MATCH_FUNCTION:

			g_value_set_pointer(value, *((gpointer *) &self->match_function));
			break;

		case PROPERTY_MODIFIED:

			g_value_set_boolean(value, self->modified);
			break;

		case PROPERTY_PLACEHOLDER_TEXT:

			g_value_set_string(
				value,
				gtk_text_get_placeholder_text(GTK_TEXT(priv->textbox))
			);

			break;

		case PROPERTY_SANITIZE_DATA:

			g_value_set_pointer(value, self->sanitize_data);
			break;

		case PROPERTY_SANITIZE_FUNCTION:

			g_value_set_pointer(
				value,
				*((gpointer *) &self->sanitize_function)
			);

			break;

		case PROPERTY_SORT_DATA:

			g_value_set_pointer(value, self->sort_data);
			break;

		case PROPERTY_SORT_FUNCTION:

			g_value_set_pointer(value, *((gpointer *) &self->sort_function));
			break;

		case PROPERTY_TAGS:

			/*  Transfer none  */

			if (priv->tags_have_changed || !self->tags) {

				g_strfreev(self->tags);

				self->tags = gnui_tagged_entry_retrieve_tags(
					priv->taglist,
					priv->tag_count
				);

				priv->tags_have_changed = false;

			}

			g_value_set_static_boxed(value, self->tags);

			break;

		default:

			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);

	}

}


/**

    gnui_tagged_entry_set_property:
    @object:    (auto) (not nullable): The tagged entry passed as `GObject`
    @prop_id:   (auto): The id of the property to set
    @value:     (auto): The `GValue` containing the new value assigned to the
                property
    @pspec:     (auto) (not nullable): The `GParamSpec` of the property

    Class handler for `g_object_set()` on the tagged entry instance

**/
static void gnui_tagged_entry_set_property (
	GObject * const object,
	const guint prop_id,
	const GValue * const value,
	GParamSpec * const pspec
) {

	if (gtk_editable_delegate_set_property(object, prop_id, value, pspec)) {

		return;

	}

	GnuiTaggedEntry * const self = GNUI_TAGGED_ENTRY(object);

	union {
		bool b;
		gpointer p;
		const gchar * s;
		GCallback f;
	} val;

	switch (prop_id) {

		case PROPERTY_DELIMITER_CHARS:

			val.s = g_value_get_string(value);

			if (!g_strcmp0(self->delimiter_chars, val.s)) {

				return;

			}

			g_free(self->delimiter_chars);
			self->delimiter_chars = val.s && *val.s ? g_strdup(val.s) : NULL;
			break;

		case PROPERTY_FILTER_DATA:

			if ((val.p = g_value_get_pointer(value)) == self->filter_data) {

				return;

			}

			self->filter_data = val.p;
			break;

		case PROPERTY_FILTER_FUNCTION:

			*((gpointer *) &val.f) = g_value_get_pointer(value);

			if (self->filter_function == (GnuiTaggedEntryFilterFunc) val.f) {

				return;

			}

			self->filter_function = (GnuiTaggedEntryFilterFunc) val.f;
			gnui_tagged_entry_invalidate_filter(self);
			break;

		case PROPERTY_INVALID:

			if ((val.b = g_value_get_boolean(value)) == self->invalid) {

				return;

			}

			self->invalid = val.b;

			(val.b ? gtk_widget_add_css_class : gtk_widget_remove_css_class)(
				GTK_WIDGET(self),
				"invalid"
			);

			g_object_notify_by_pspec(G_OBJECT(self), props[prop_id]);
			g_signal_emit(self, signals[SIGNAL_INVALID_CHANGED], 0, val.b);

			/*  Keep `return` here!  */
			return;

		case PROPERTY_MATCH_DATA:

			if ((val.p = g_value_get_pointer(value)) == self->match_data) {

				return;

			}

			self->match_data = val.p;
			break;

		case PROPERTY_MATCH_FUNCTION:

			*((gpointer *) &val.f) = g_value_get_pointer(value);

			if (self->match_function == (GnuiTaggedEntryMatchFunc) val.f) {

				return;

			}

			self->match_function = (GnuiTaggedEntryMatchFunc) val.f;
			break;

		case PROPERTY_MODIFIED:

			gnui_tagged_entry_dispatch_modified(
				self,
				gnui_tagged_entry_get_instance_private(self),
				g_value_get_boolean(value)
			);

			return;

		case PROPERTY_PLACEHOLDER_TEXT:

			val.s = g_value_get_string(value);

			gtk_text_set_placeholder_text(
				GTK_TEXT(
					(
						(GnuiTaggedEntryPrivate *)
							gnui_tagged_entry_get_instance_private(self)
					)->textbox
				),
				val.s
			);

			gtk_accessible_update_property(
				GTK_ACCESSIBLE(self),
				GTK_ACCESSIBLE_PROPERTY_PLACEHOLDER,
				val.s,
				-1
			);

			break;

		case PROPERTY_SANITIZE_DATA:

			if ((val.p = g_value_get_pointer(value)) == self->sanitize_data) {

				return;

			}

			self->sanitize_data = val.p;
			break;

		case PROPERTY_SANITIZE_FUNCTION:

			*((gpointer *) &val.f) = g_value_get_pointer(value);

			if (
				self->sanitize_function == (GnuiTaggedEntrySanitizeFunc) val.f
			) {

				return;

			}

			self->sanitize_function = (GnuiTaggedEntrySanitizeFunc) val.f;
			break;

		case PROPERTY_SORT_DATA:

			if ((val.p = g_value_get_pointer(value)) == self->sort_data) {

				return;

			}

			self->sort_data = val.p;
			break;

		case PROPERTY_SORT_FUNCTION:

			*((gpointer *) &val.f) = g_value_get_pointer(value);

			if (self->sort_function == (GnuiTaggedEntrySortFunc) val.f) {

				return;

			}

			self->sort_function = (GnuiTaggedEntrySortFunc) val.f;
			gnui_tagged_entry_invalidate_sort(self);
			break;

		case PROPERTY_TAGS:

			/*  Transfer none  */

			gnui_tagged_entry_populate_strv(
				self,
				g_value_get_boxed(value),
				false
			);

			return;

		default:

			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			return;

	}

	g_object_notify_by_pspec(G_OBJECT(self), props[prop_id]);

}


/**

    gnui_tagged_entry_widget_compute_expand:
    @tagged:    (auto) (not nullable): The tagged entry passed as `GtkWidget`
    @hexpand:   (auto) (not nullable) (out): A pointer to set to whether the
				tagged entry must expand horizontally
    @vexpand:   (auto) (not nullable) (out): A pointer to set to whether the
				tagged entry must expand vertically

    Class handler for the #GtkWidget.compute_expand() method on the tagged
    entry instance

**/
static void gnui_tagged_entry_widget_compute_expand (
	GtkWidget * const tagged,
	gboolean * const hexpand,
	gboolean * const vexpand
) {

	bool is_vertical = gnui_flow_layout_get_orientation(
		GNUI_FLOW_LAYOUT(gtk_widget_get_layout_manager(GTK_WIDGET(tagged)))
	) == GTK_ORIENTATION_VERTICAL;

	*hexpand = !is_vertical || gtk_widget_get_hexpand(tagged);
	*vexpand = is_vertical || gtk_widget_get_vexpand(tagged);

}


/**

    gnui_tagged_entry_class_init:
    @klass:     (auto) (not nullable): The `GObject` klass

    The init function of the tagged entry class

**/
static void gnui_tagged_entry_class_init (
	GnuiTaggedEntryClass * const klass
) {

	GObjectClass * const object_class = G_OBJECT_CLASS(klass);
	GtkWidgetClass * const widget_class = GTK_WIDGET_CLASS(klass);

	object_class->dispose = gnui_tagged_entry_dispose;
	object_class->get_property = gnui_tagged_entry_get_property;
	object_class->set_property = gnui_tagged_entry_set_property;

	widget_class->compute_expand = gnui_tagged_entry_widget_compute_expand;

	/**

	    GnuiTaggedEntry:delimiter-chars: (transfer none) (nullable)

	**/
	props[PROPERTY_DELIMITER_CHARS] = g_param_spec_string(
		"delimiter-chars",
		"gchar *",
        "A NIL-terminated array of characters (i.e. a \342\200\234C "
			"string\342\200\235) to use as input delimiters between different "
			"tags",
		NULL,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	/**

	    GnuiTaggedEntry:filter-data: (nullable) (closure)

	**/
	props[PROPERTY_FILTER_DATA] = g_param_spec_pointer(
		"filter-data",
		"gpointer",
		"Closure data for \342\200\234filter-function\342\200\235 "
			"\342\200\223 use the \342\200\234destroy\342\200\235 signal if "
			"later you want to free it",
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	/**

	    GnuiTaggedEntry:filter-function: (nullable)

	**/
	props[PROPERTY_FILTER_FUNCTION] = g_param_spec_pointer(
		"filter-function",
		"GnuiTaggedEntryFilterFunc",
		"Function to use to determine if a tag is valid",
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	props[PROPERTY_INVALID] = g_param_spec_boolean(
		"invalid",
		"gboolean",
		"Whether the tagged entry is in \342\200\234invalid\342\200\235 state",
		false,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	/**

	    GnuiTaggedEntry:match-data: (nullable) (closure)

	**/
	props[PROPERTY_MATCH_DATA] = g_param_spec_pointer(
		"match-data",
		"gpointer",
		"Closure data for \342\200\234match-function\342\200\235 \342\200\223 "
			"use the \342\200\234destroy\342\200\235 signal if later you want "
			"to free it",
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	/**

	    GnuiTaggedEntry:match-function: (nullable)

	**/
	props[PROPERTY_MATCH_FUNCTION] = g_param_spec_pointer(
		"match-function",
		"GnuiTaggedEntryMatchFunc",
		"Function to use to determine if two tags match",
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	props[PROPERTY_MODIFIED] = g_param_spec_boolean(
		"modified",
		"gboolean",
		"Whether the tagged entry has been modified",
		false,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	/**

	    GnuiTaggedEntry:placeholder-text: (transfer none) (nullable)

	**/
	props[PROPERTY_PLACEHOLDER_TEXT] = g_param_spec_string(
		"placeholder-text",
		"gchar *",
		"The text to be displayed when the tagged entry is empty",
		false,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	/**

	    GnuiTaggedEntry:sanitize-data: (nullable) (closure)

	**/
	props[PROPERTY_SANITIZE_DATA] = g_param_spec_pointer(
		"sanitize-data",
		"gpointer",
		"Closure data for \342\200\234sanitize-function\342\200\235 "
			"\342\200\223 use the \342\200\234destroy\342\200\235 signal if "
			"later you want to free it",
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	/**

	    GnuiTaggedEntry:sanitize-function: (nullable)

	**/
	props[PROPERTY_SANITIZE_FUNCTION] = g_param_spec_pointer(
		"sanitize-function",
		"GnuiTaggedEntrySanitizeFunc",
		"Function to use to sanitize (and allocate) each new tag",
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	/**

	    GnuiTaggedEntry:sort-data: (nullable) (closure)

	**/
	props[PROPERTY_SORT_DATA] = g_param_spec_pointer(
		"sort-data",
		"gpointer",
		"Closure data for \342\200\234sort-function\342\200\235 \342\200\223 "
			"use the \342\200\234destroy\342\200\235 signal if later you want "
			"to free it",
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	/**

	    GnuiTaggedEntry:sort-function: (nullable)

	**/
	props[PROPERTY_SORT_FUNCTION] = g_param_spec_pointer(
		"sort-function",
		"GnuiTaggedEntrySortFunc",
		"Function to use to sort the tag order",
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	/**

	    GnuiTaggedEntry:tags: (transfer none) (nullable)

	**/
	props[PROPERTY_TAGS] = g_param_spec_boxed(
		"tags",
		"gchar **",
		"The current tags",
		G_TYPE_STRV,
		G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
	);

	g_object_class_install_properties(object_class, N_PROPERTIES, props);
	gtk_editable_install_properties(object_class, N_PROPERTIES);

	gtk_widget_class_set_layout_manager_type(
		widget_class,
		GNUI_TYPE_FLOW_LAYOUT
	);

	gtk_widget_class_set_accessible_role(
		widget_class,
		GTK_ACCESSIBLE_ROLE_TEXT_BOX
	);

	gtk_widget_class_set_css_name(widget_class, I_("entry"));

	/**

	    GnuiTaggedEntry::activate:
	    @self:      (auto) (non-nullable): The tagged entry that emitted the
	                signal

	    Signal emitted when a tagged entry is activated (usually by pressing
	    `ENTER`)

	    #GnuiTaggedEntrySignalHandlerActivate is the function type of
	    reference for this signal, which takes parameters' constness into
	    account.

	**/
	signals[SIGNAL_ACTIVATE] = g_signal_new(
		I_("activate"),
		G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_FIRST,
		0,
		NULL,
		NULL,
		NULL,
		G_TYPE_NONE,
		0
	);

	/**

	    GnuiTaggedEntry::invalid-changed:
	    @self:      (auto) (non-nullable): The tagged entry that
	                emitted the signal
	    @invalid:   (auto): The new value of the #GnuiTaggedEntry:invalid
	                property

	    Signal emitted when the #GnuiTaggedEntry:invalid property changes

	    #GnuiTaggedEntrySignalHandlerInvalidChanged is the function type of
	    reference for this signal, which takes parameters' constness into
	    account.

	**/
	signals[SIGNAL_INVALID_CHANGED] = g_signal_new(
		I_("invalid-changed"),
		G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_FIRST,
		0,
		NULL,
		NULL,
		g_cclosure_marshal_VOID__BOOLEAN,
		G_TYPE_NONE,
		1,
		/*  Maps `gboolean invalid`  */
		G_TYPE_BOOLEAN
	);

	/**

	    GnuiTaggedEntry::modified-changed:
	    @self:      (auto) (non-nullable): The tagged entry that
	                emitted the signal
	    @modified:  (auto): The new value of the #GnuiTaggedEntry:modified
	                property

	    Signal emitted when the #GnuiTaggedEntry:modified property changes

	    #GnuiTaggedEntrySignalHandlerModifiedChanged is the function type of
	    reference for this signal, which takes parameters' constness into
	    account.

	**/
	signals[SIGNAL_MODIFIED_CHANGED] = g_signal_new(
		I_("modified-changed"),
		G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_FIRST,
		0,
		NULL,
		NULL,
		g_cclosure_marshal_VOID__BOOLEAN,
		G_TYPE_NONE,
		1,
		/*  Maps `gboolean modified`  */
		G_TYPE_BOOLEAN
	);

	/**

	    GnuiTaggedEntry::tag-added:
	    @self:      (auto) (non-nullable): The tagged entry that emitted the
	                signal
	    @tag:       (auto) (not nullable) (transfer none): The tag added

	    Signal emitted when a new tag is added to the tagged entry

	    #GnuiTaggedEntrySignalHandlerTagAdded is the function type of
	    reference for this signal, which takes parameters' constness into
	    account.

	**/
	signals[SIGNAL_TAG_ADDED] = g_signal_new(
		I_("tag-added"),
		G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_FIRST,
		0,
		NULL,
		NULL,
		g_cclosure_marshal_VOID__STRING,
		G_TYPE_NONE,
		1,
		/*  Maps `const gchar * tag`  */
		G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE
	);

	/**

	    GnuiTaggedEntry::tag-removed:
	    @self:      (auto) (non-nullable): The tagged entry that emitted the
	                signal
	    @tag:       (auto) (not nullable) (transfer none): The tag removed

	    Signal emitted when a tag is removed from the tagged entry

	    #GnuiTaggedEntrySignalHandlerTagRemoved is the function type of
	    reference for this signal, which takes parameters' constness into
	    account.

	**/
	signals[SIGNAL_TAG_REMOVED] = g_signal_new(
		I_("tag-removed"),
		G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_FIRST,
		0,
		NULL,
		NULL,
		g_cclosure_marshal_VOID__STRING,
		G_TYPE_NONE,
		1,
		/*  Maps `const gchar * tag`  */
		G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE
	);

}


/**

    gnui_tagged_entry_init:
    @self:  (auto) (not nullable): The newly allocated tagged entry

    The init function of the tagged entry instance

**/
static void gnui_tagged_entry_init (
	GnuiTaggedEntry * const self
) {

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	/*  `-DGNUI_TAGGED_ENTRY_BUILD_FLAG_MANUAL_ENVIRONMENT` erases this  */
	GNUI_MODULE_ENSURE_ENVIRONMENT

	g_object_set(
		G_OBJECT(gtk_widget_get_layout_manager(GTK_WIDGET(self))),
		"orientation", GTK_ORIENTATION_HORIZONTAL,
		"spacing", 4,
		"leading", 3,
		/*"line-justify", GTK_JUSTIFY_FILL,*/
		NULL
	);

	priv->textbox = g_object_new(
		GTK_TYPE_TEXT,
		"hexpand", true,
		"propagate-text-width", true,
		NULL
	);

	gtk_editable_init_delegate(GTK_EDITABLE(self));
	gtk_widget_add_css_class(priv->textbox, "provider");

	g_signal_connect(
		priv->textbox,
		"activate",
		G_CALLBACK(gnui_tagged_entry__on_textbox_activate),
		self
	);

	g_signal_connect(
		priv->textbox,
		"backspace",
		G_CALLBACK(gnui_tagged_entry__on_textbox_backspace),
		self
	);

	g_signal_connect(
		priv->textbox,
		"changed",
		G_CALLBACK(gnui_tagged_entry__on_textbox_change),
		self
	);

	gtk_widget_insert_before(priv->textbox, GTK_WIDGET(self), NULL);
	gtk_widget_add_css_class(GTK_WIDGET(self), "tagged");

}


/**

    gnui_tagged_entry_editable_get_delegate:
    @tagged:    (auto) (not nullable): The tagged entry passed as `GtkEditable`

    Class handler for the #GtkEditable.get_delegate() method on the tagged
    entry's editable interface

    Returns:    The delegate widget passed as `GtkEditable`

**/
static GtkEditable * gnui_tagged_entry_editable_get_delegate (
	GtkEditable * const tagged
) {

	return GTK_EDITABLE(
		(
			(GnuiTaggedEntryPrivate *)
				gnui_tagged_entry_get_instance_private(
					GNUI_TAGGED_ENTRY(tagged)
				)
		)->textbox
	);

}


/**

    gnui_tagged_entry_editable_iface_init:
    @iface:     (auto) (not nullable): The tagged entry's editable interface

    The init function of the tagged entry's editable interface

**/
static void gnui_tagged_entry_editable_iface_init (
	GtkEditableInterface * const iface
) {

	iface->get_delegate = gnui_tagged_entry_editable_get_delegate;

}



/*\
|*|
|*| PUBLIC FUNCTIONS
|*|
|*| (See the public header for the documentation)
|*|
\*/


gboolean gnui_tagged_entry_has_tag (
	GnuiTaggedEntry * const self,
	const gchar * const tag
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), false);

	if (!tag || !*tag) {

		return false;

	}

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	if (self->match_function) {

		for (const GList * llnk = priv->taglist; llnk; llnk = llnk->next) {

			if (
				self->match_function(
					((TagReference *) llnk->data)->tag,
					tag,
					self->match_data
				)
			) {

				return true;

			}

		}

		return false;

	}

	for (const GList * llnk = priv->taglist; llnk; llnk = llnk->next) {

		if (!strcmp(((TagReference *) llnk->data)->tag, tag)) {

			return true;

		}

	}

	return false;

}


gboolean gnui_tagged_entry_remove_tag (
	GnuiTaggedEntry * const self,
	const gchar * const tag
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), false);
	g_return_val_if_fail(tag != NULL, false);

	if (!*tag) {

		return false;

	}

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	if (self->match_function) {

		for (GList * llnk = priv->taglist; llnk; llnk = llnk->next) {

			if (
				self->match_function(
					((TagReference *) llnk->data)->tag,
					tag,
					self->match_data
				)
			) {

				gnui_tagged_entry_delete_llnk(self, priv, llnk);
				return true;

			}

		}

		return false;

	}

	for (GList * llnk = priv->taglist; llnk; llnk = llnk->next) {

		if (!strcmp(((TagReference *) llnk->data)->tag, tag)) {

			gnui_tagged_entry_delete_llnk(self, priv, llnk);
			return true;

		}

	}

	return false;

}


void gnui_tagged_entry_remove_all_tags (
	GnuiTaggedEntry * const self
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	if (!priv->taglist) {

		return;

	}

	for (const GList * llnk = priv->taglist; llnk; llnk = llnk->next) {

		gnui_tagged_entry_tag_reference_destroy(self, llnk->data);

	}

	g_clear_pointer(&priv->taglist, g_list_free);
	priv->tag_count = 0;
	priv->next_id = 0;
	g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_TAGS]);
	gnui_tagged_entry_dispatch_modified(self, priv, true);

}


G_GNUC_NULL_TERMINATED gboolean gnui_tagged_entry_populate (
	GnuiTaggedEntry * const self,
	const gboolean pinned,
	...
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), false);

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	const gchar * tag;
	gchar * sanitized_tag;
	GList * llnk, * old_tags = g_steal_pointer(&priv->taglist);
	TagReference * tagref;
	bool changed = false, all_added = true;
	va_list args;

	va_start(args, pinned);
	old_tags = g_list_reverse(old_tags);


	/* \                                  /\
	\ */     next_new_tag:               /* \
	 \/     ________________________     \ */


	if ((tag = va_arg(args, const gchar *))) {

		llnk = old_tags;
		sanitized_tag = gnui_tagged_entry_sanitize_and_allocate_tag(self, tag);

		if (!sanitized_tag) {

			all_added = false;
			goto next_new_tag;

		}


		/* \                                  /\
		\ */     next_old_tag:               /* \
		 \/     ________________________     \ */


		if (llnk) {

			tagref = llnk->data;

			if (strcmp(sanitized_tag, tagref->tag)) {

				llnk = llnk->next;
				goto next_old_tag;

			}

			/*  This tag is already present -- keep the widgets  */

			g_free(tagref->tag);
			tagref->tag = sanitized_tag;
			old_tags = g_list_remove_link(old_tags, llnk);
			g_object_ref(tagref->box);
			gtk_widget_unparent(tagref->box);
			gnui_tagged_entry_insert_llink_sorted(self, priv, llnk);
			g_object_unref(tagref->box);
			goto next_new_tag;

		}

		/*  The tag is new -- add it  */

		gnui_tagged_entry_add_sanitized_tag(self, priv, sanitized_tag, pinned);
		changed = true;
		goto next_new_tag;

	}

	changed |= old_tags != NULL;

	/*  Destroy the remaining old tags  */

	for (const GList * llnk = old_tags; llnk; llnk = llnk->next) {

		gnui_tagged_entry_tag_reference_destroy(self, llnk->data);
		priv->tag_count--;

	}

	g_list_free(old_tags);

	if (changed) {

		g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_TAGS]);
		gnui_tagged_entry_dispatch_modified(self, priv, true);

	}

	va_end(args);
	return all_added;

}


gboolean gnui_tagged_entry_populate_strv (
	GnuiTaggedEntry * const self,
	const gchar * const * const tags,
	const gboolean pinned
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), false);

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	TagReference * tagref;
	gchar * sanitized_tag;
	GList * llnk, * old_tags = g_steal_pointer(&priv->taglist);
	bool changed = false, all_added = true;

	if (!tags) {

		goto clear_and_exit;

	}

	const gchar * const * tagptr = tags - 1;
	old_tags = g_list_reverse(old_tags);


	/* \                                  /\
	\ */     next_new_tag:               /* \
	 \/     ________________________     \ */


	if (*++tagptr) {

		llnk = old_tags;

		sanitized_tag = gnui_tagged_entry_sanitize_and_allocate_tag(
			self,
			*tagptr
		);

		if (!sanitized_tag) {

			all_added = false;
			goto next_new_tag;

		}


		/* \                                  /\
		\ */     next_old_tag:               /* \
		 \/     ________________________     \ */


		if (llnk) {

			tagref = llnk->data;

			if (strcmp(sanitized_tag, tagref->tag)) {

				llnk = llnk->next;
				goto next_old_tag;

			}

			/*  This tag is already present -- keep the widgets  */

			old_tags = g_list_remove_link(old_tags, llnk);
			g_object_ref(tagref->box);
			gtk_widget_unparent(tagref->box);
			gnui_tagged_entry_insert_llink_sorted(self, priv, llnk);
			g_object_unref(tagref->box);
			goto next_new_tag;

		}

		/*  The tag is new -- add it  */

		gnui_tagged_entry_add_sanitized_tag(self, priv, sanitized_tag, pinned);
		changed = true;
		goto next_new_tag;

	}


	/* \                                  /\
	\ */     clear_and_exit:             /* \
	 \/     ________________________     \ */


	changed |= old_tags != NULL;

	/*  Destroy the remaining old tags  */

	for (const GList * llnk = old_tags; llnk; llnk = llnk->next) {

		gnui_tagged_entry_tag_reference_destroy(self, llnk->data);
		priv->tag_count--;

	}

	g_list_free(old_tags);

	if (changed) {

		g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_TAGS]);
		gnui_tagged_entry_dispatch_modified(self, priv, true);

	}

	return all_added;

}


gboolean gnui_tagged_entry_add_tag (
	GnuiTaggedEntry * const self,
	const gchar * const tag,
	const gboolean pinned
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), false);
	g_return_val_if_fail(tag != NULL, false);

	if (self->delimiter_chars) {

		const gchar * ptr = self->delimiter_chars - 1;

		while (*++ptr) {

			if (strchr(tag, *ptr)) {

				g_warning(
					_("Delimiter found in tag \"%s\" - skipped\n"),
					tag
				);

				return false;

			}

		}

	}

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	if (
		gnui_tagged_entry_sanitize_and_add_tag(
			self,
			priv,
			tag,
			pinned
		)
	) {

		g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_TAGS]);
		return true;

	}

	return false;

}


G_GNUC_NULL_TERMINATED gboolean gnui_tagged_entry_add_tags (
	GnuiTaggedEntry * const self,
	const gboolean pinned,
	...
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), false);

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	const gchar * tag;
	va_list args;
	bool result, changed = false, all_added = true;

	const gchar
		* ptr,
		* const delimiters_minus_one =
			self->delimiter_chars ? self->delimiter_chars - 1 : "~";

	va_start(args, pinned);

	while ((tag = va_arg(args, const gchar *))) {

		for (ptr = delimiters_minus_one; *++ptr && !strchr(tag, *ptr); );

		if (*ptr) {

			all_added = false;
			g_warning(_("Delimiter found in tag \"%s\" - skipped\n"), tag);


		} else {

			result = gnui_tagged_entry_sanitize_and_add_tag(
				self,
				priv,
				tag,
				pinned
			);

			changed |= result;
			all_added &= result;

		}

	}

	if (changed) {

		g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_TAGS]);

	}

	va_end(args);
	return (gboolean) all_added;

}


gsize gnui_tagged_entry_parse_expression (
	GnuiTaggedEntry * const self,
	const gchar * const expression,
	const gboolean pinned
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), false);
	g_return_val_if_fail(expression != NULL, false);

	const gsize exprlen = strlen(expression);

	return
		self->delimiter_chars ?
			gnui_tagged_entry_private_add_expression(
				self,
				gnui_tagged_entry_get_instance_private(self),
				expression,
				exprlen,
				pinned
			)
		: gnui_tagged_entry_sanitize_and_add_tag(
			self,
			gnui_tagged_entry_get_instance_private(self),
			expression,
			pinned
		) ?
			exprlen
		:
			0;

}


extern gboolean gnui_tagged_entry_rename_tag (
	GnuiTaggedEntry * const self,
	const gchar * const old_name,
	const gchar * const new_name
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), false);
	g_return_val_if_fail(old_name != NULL && new_name != NULL, false);


	if (
		!*old_name ||!*new_name || (
			self->filter_function &&
			!self->filter_function(self, new_name, self->filter_data)
		)
	) {

		return false;

	}

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	TagReference * tagref;
	const GList * llnk = priv->taglist;

	if (self->match_function) {

		for (; llnk; llnk = llnk->next) {

			if (
				self->match_function(
					((TagReference *) llnk->data)->tag,
					old_name,
					self->match_data
				)
			) {

				goto rename_and_exit;

			}

		}

		return false;

	}

	for (; llnk; llnk = llnk->next) {

		if (!strcmp(((TagReference *) llnk->data)->tag, old_name)) {

			goto rename_and_exit;

		}

	}

	return false;


	/* \                                  /\
	\ */     rename_and_exit:            /* \
	 \/     ________________________     \ */


	tagref = llnk->data;
	g_free(tagref->tag);

	tagref->tag =
		self->sanitize_function ?
			self->sanitize_function(
				self,
				new_name,
				self->sanitize_data
			)
		:
			g_strdup(new_name);

	gtk_label_set_text(tagref->marker, tagref->tag);
	g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_TAGS]);
	gnui_tagged_entry_dispatch_modified(self, priv, true);
	return true;

}


gboolean gnui_tagged_entry_set_pin (
	GnuiTaggedEntry * const self,
	const gchar * const tag,
	const gboolean pinned
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), false);
	g_return_val_if_fail(tag != NULL, false);

	if (!*tag) {

		return false;

	}

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	const GList * llnk = priv->taglist;

	if (self->match_function) {

		for (; llnk; llnk = llnk->next) {

			if (
				self->match_function(
					((TagReference *) llnk->data)->tag,
					tag,
					self->match_data
				)
			) {

				goto set_pin_and_exit;

			}

		}

		return false;

	}

	for (; llnk; llnk = llnk->next) {

		if (!strcmp(((TagReference *) llnk->data)->tag, tag)) {

			goto set_pin_and_exit;

		}

	}

	return false;


	/* \                                  /\
	\ */     set_pin_and_exit:           /* \
	 \/     ________________________     \ */


	gtk_widget_set_sensitive(
		((TagReference *) llnk->data)->remover,
		!pinned
	);

	((TagReference *) llnk->data)->pinned = pinned;
	return true;

}


G_GNUC_NULL_TERMINATED gboolean gnui_tagged_entry_set_pins (
	GnuiTaggedEntry * const self,
	const gboolean pinned,
	...
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), false);

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	const gchar * tag;
	va_list args;
	bool all_found = true;

	va_start(args, pinned);

	if (self->match_function) {

		/* \                                  /\
		\ */     next_tag__match:            /* \
		 \/     ________________________     \ */


		if ((tag = va_arg(args, const gchar *))) {

			if (!*tag) {

				all_found = false;
				goto next_tag__match;

			}

			for (const GList * llnk = priv->taglist; llnk; llnk = llnk->next) {

				if (
					self->match_function(
						((TagReference *) llnk->data)->tag,
						tag,
						self->match_data
					)
				) {

					gtk_widget_set_sensitive(
						((TagReference *) llnk->data)->remover,
						!pinned
					);

					((TagReference *) llnk->data)->pinned = pinned;
					goto next_tag__match;

				}

			}

			all_found = false;
			goto next_tag__match;

		}

		goto va_end_and_exit;

	}


	/* \                                  /\
	\ */     next_tag__strcmp:           /* \
	 \/     ________________________     \ */


	if ((tag = va_arg(args, const gchar *))) {

		for (const GList * llnk = priv->taglist; llnk; llnk = llnk->next) {

			if (!strcmp(((TagReference *) llnk->data)->tag, tag)) {

				gtk_widget_set_sensitive(
					((TagReference *) llnk->data)->remover,
					!pinned
				);

				((TagReference *) llnk->data)->pinned = pinned;
				goto next_tag__strcmp;

			}

		}

		all_found = false;
		goto next_tag__strcmp;

	}


	/* \                                  /\
	\ */     va_end_and_exit:            /* \
	 \/     ________________________     \ */


	va_end(args);
	return (gboolean) all_found;

}


void gnui_tagged_entry_activate (
	GnuiTaggedEntry * const self
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	gnui_tagged_entry__on_textbox_activate(
		GTK_EDITABLE(
			((GnuiTaggedEntryPrivate *)
				gnui_tagged_entry_get_instance_private(self))->textbox
		),
		(gpointer) self
	);

}


void gnui_tagged_entry_invalidate_filter (
	GnuiTaggedEntry * const self
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	if (!self->filter_function) {

		return;

	}

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	GList * llnk = priv->taglist;
	TagReference * tagref;
	bool modified = false;

	while (llnk) {

		tagref = llnk->data;

		if (self->filter_function(self, tagref->tag, self->filter_data)) {

			llnk = llnk->next;

		} else {

			modified = true;
			gnui_tagged_entry_tag_reference_destroy(self, tagref);
			priv->tag_count--;
			GNUI_LIST_DELETE_AND_MOVE_TO_NEXT(&priv->taglist, &llnk);

		}

	}

	if (modified) {

		g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_TAGS]);
		gnui_tagged_entry_dispatch_modified(self, priv, true);

	}

}


void gnui_tagged_entry_invalidate_sort (
	GnuiTaggedEntry * const self
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	TagReference * tagref;
	GList * old_tags = g_steal_pointer(&priv->taglist), * llnk = old_tags;

	while (llnk) {

		tagref = llnk->data;
		g_object_ref(tagref->box);
		gtk_widget_unparent(tagref->box);

		gnui_tagged_entry_insert_llink_sorted(
			self,
			priv,
			gnui_list_detach_and_move_to_next(&old_tags, &llnk)
		);

		g_object_unref(tagref->box);

	}

	priv->tags_have_changed = true;

}


const gchar * gnui_tagged_entry_get_delimiter_chars (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), NULL);

	return self->delimiter_chars;

}


void gnui_tagged_entry_set_delimiter_chars (
	GnuiTaggedEntry * const self,
	const gchar * delimiter_chars
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	if (g_strcmp0(self->delimiter_chars, delimiter_chars)) {

		g_free(self->delimiter_chars);

		self->delimiter_chars =
			delimiter_chars && *delimiter_chars ?
				g_strdup(delimiter_chars)
			:
				NULL;

		g_object_notify_by_pspec(
			G_OBJECT(self),
			props[PROPERTY_DELIMITER_CHARS]
		);

	}

}


gpointer gnui_tagged_entry_get_filter_data (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), NULL);

	return self->filter_data;

}


void gnui_tagged_entry_set_filter_data (
	GnuiTaggedEntry * const self,
	const gpointer filter_data
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	if (self->filter_data != filter_data) {

		self->filter_data = filter_data;
		g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_FILTER_DATA]);

	}

}


GnuiTaggedEntryFilterFunc gnui_tagged_entry_get_filter_function (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), NULL);

	return self->filter_function;

}


void gnui_tagged_entry_set_filter_function (
	GnuiTaggedEntry * const self,
	const GnuiTaggedEntryFilterFunc filter_func
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	if (self->filter_function != filter_func) {

		self->filter_function = filter_func;
		gnui_tagged_entry_invalidate_filter(self);

		g_object_notify_by_pspec(
			G_OBJECT(self),
			props[PROPERTY_FILTER_FUNCTION]
		);

	}

}


gboolean gnui_tagged_entry_get_invalid (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), false);

	return self->invalid;

}


void gnui_tagged_entry_set_invalid (
	GnuiTaggedEntry * const self,
	const gboolean invalid
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	if (self->invalid != invalid) {

		self->invalid = invalid;

		(invalid ? gtk_widget_add_css_class : gtk_widget_remove_css_class)(
			GTK_WIDGET(self),
			"invalid"
		);

		g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_INVALID]);
		g_signal_emit(self, signals[SIGNAL_INVALID_CHANGED], 0, invalid);

	}

}


gpointer gnui_tagged_entry_get_match_data (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), NULL);

	return self->match_data;

}


void gnui_tagged_entry_set_match_data (
	GnuiTaggedEntry * const self,
	const gpointer match_data
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	if (self->match_data != match_data) {

		self->match_data = match_data;
		g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_MATCH_DATA]);

	}

}


GnuiTaggedEntryMatchFunc gnui_tagged_entry_get_match_function (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), NULL);

	return self->match_function;

}


void gnui_tagged_entry_set_match_function (
	GnuiTaggedEntry * const self,
	const GnuiTaggedEntryMatchFunc match_func
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	if (self->match_function != match_func) {

		self->match_function = match_func;

		g_object_notify_by_pspec(
			G_OBJECT(self),
			props[PROPERTY_MATCH_FUNCTION]
		);

	}

}


gboolean gnui_tagged_entry_get_modified (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), false);

	return self->modified;

}


void gnui_tagged_entry_set_modified (
	GnuiTaggedEntry * const self,
	const gboolean modified
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	gnui_tagged_entry_dispatch_modified(
		self,
		gnui_tagged_entry_get_instance_private(self),
		modified
	);

}


const char * gnui_tagged_entry_get_placeholder_text (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), NULL);

	return gtk_text_get_placeholder_text(
		GTK_TEXT(
			((GnuiTaggedEntryPrivate *)
				gnui_tagged_entry_get_instance_private(self))->textbox
		)
	);

}


void gnui_tagged_entry_set_placeholder_text (
	GnuiTaggedEntry * const self,
	const char * const placeholder_text
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	gtk_text_set_placeholder_text(
		GTK_TEXT(
			((GnuiTaggedEntryPrivate *)
				gnui_tagged_entry_get_instance_private(self))->textbox
		),
		placeholder_text
	);

	gtk_accessible_update_property(
		GTK_ACCESSIBLE(self),
		GTK_ACCESSIBLE_PROPERTY_PLACEHOLDER,
		placeholder_text,
		-1
	);

	g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_PLACEHOLDER_TEXT]);

}


gpointer gnui_tagged_entry_get_sanitize_data (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), NULL);

	return self->sanitize_data;

}


void gnui_tagged_entry_set_sanitize_data (
	GnuiTaggedEntry * const self,
	const gpointer sanitize_data
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	if (self->sanitize_data != sanitize_data) {

		self->sanitize_data = sanitize_data;

		g_object_notify_by_pspec(
			G_OBJECT(self),
			props[PROPERTY_SANITIZE_DATA]
		);

	}

}


GnuiTaggedEntrySanitizeFunc gnui_tagged_entry_get_sanitize_function (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), NULL);

	return self->sanitize_function;

}


void gnui_tagged_entry_set_sanitize_function (
	GnuiTaggedEntry * const self,
	const GnuiTaggedEntrySanitizeFunc sanitize_func
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	if (self->sanitize_function != sanitize_func) {

		self->sanitize_function = sanitize_func;

		g_object_notify_by_pspec(
			G_OBJECT(self),
			props[PROPERTY_SANITIZE_FUNCTION]
		);

	}

}


gpointer gnui_tagged_entry_get_sort_data (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), NULL);

	return self->sort_data;

}


void gnui_tagged_entry_set_sort_data (
	GnuiTaggedEntry * const self,
	const gpointer sort_data
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	if (self->sort_data != sort_data) {

		self->sort_data = sort_data;
		g_object_notify_by_pspec(G_OBJECT(self), props[PROPERTY_SORT_DATA]);

	}

}


GnuiTaggedEntrySortFunc gnui_tagged_entry_get_sort_function (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), NULL);

	return self->sort_function;

}


void gnui_tagged_entry_set_sort_function (
	GnuiTaggedEntry * const self,
	const GnuiTaggedEntrySortFunc sort_func
) {

	g_return_if_fail(GNUI_IS_TAGGED_ENTRY(self));

	if (self->sort_function != sort_func) {

		self->sort_function = sort_func;
		gnui_tagged_entry_invalidate_sort(self);

		g_object_notify_by_pspec(
			G_OBJECT(self),
			props[PROPERTY_SORT_FUNCTION]
		);

	}

}


const gchar * const * gnui_tagged_entry_get_tags (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), NULL);

	GnuiTaggedEntryPrivate * const priv =
		gnui_tagged_entry_get_instance_private(self);

	if (priv->tags_have_changed || !self->tags) {

		g_strfreev(self->tags);

		self->tags = gnui_tagged_entry_retrieve_tags(
			priv->taglist,
			priv->tag_count
		);

		priv->tags_have_changed = false;

	}

	return (const gchar * const *) self->tags;

}


void gnui_tagged_entry_set_tags (
	GnuiTaggedEntry * const self,
	const gchar * const * const tags
) {

	gnui_tagged_entry_populate_strv(self, tags, false);

}


GtkText * gnui_tagged_entry_get_text_delegate (
	GnuiTaggedEntry * const self
) {

	g_return_val_if_fail(GNUI_IS_TAGGED_ENTRY(self), NULL);

	return GTK_TEXT(
		((GnuiTaggedEntryPrivate *)
			gnui_tagged_entry_get_instance_private(self))->textbox
	);

}


G_GNUC_WARN_UNUSED_RESULT GtkWidget * gnui_tagged_entry_new (void) {

	return g_object_new(GNUI_TYPE_TAGGED_ENTRY, NULL);

}


/*  EOF  */

