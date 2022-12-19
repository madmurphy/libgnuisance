/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*  Please make sure that the TAB width in your editor is set to 4 spaces  */


#include <gtk/gtk.h>
#include <gnuisance/gnui-tagged-entry.h>


static gboolean tagged_entry_filter_func (
    GnuiTaggedEntry * self,
    const gchar * tag,
    gpointer user_data G_GNUC_UNUSED
) {

	if (gnui_tagged_entry_has_tag(self, tag)) {

		return false;

	}

	return true;

}


static void on_do_something_1_click (
	GtkButton * const button G_GNUC_UNUSED,
	const gpointer v_tagged_entry G_GNUC_UNUSED
) {

	printf("Do something 1 clicked!\n");

}


static void on_do_something_2_click (
	GtkButton * const button G_GNUC_UNUSED,
	const gpointer v_tagged_entry G_GNUC_UNUSED
) {

	printf("Do something 2 clicked!\n");

}


static void on_clear_click (
	GtkButton * const button G_GNUC_UNUSED,
	const gpointer v_tagged_entry
) {

	gnui_tagged_entry_remove_all_tags(v_tagged_entry);

}

static void signal_test__tag_added (
	GnuiTaggedEntry * self G_GNUC_UNUSED,
	const gchar * const tag,
	const gpointer user_data G_GNUC_UNUSED
) {

	printf("Signal \"tag-added\" -- added tag \"%s\"\n", tag);

}


static void signal_test__tag_removed (
	GnuiTaggedEntry * self G_GNUC_UNUSED,
	const gchar * const tag,
	const gpointer user_data G_GNUC_UNUSED
) {

	printf("Signal \"tag-removed\" -- removed tag \"%s\"\n", tag);

}


static void signal_test__activate (
	GnuiTaggedEntry * self G_GNUC_UNUSED,
	gpointer user_data G_GNUC_UNUSED
) {

	printf("Signal \"activate\"\n");

}


static void signal_test__modified_changed (
	GnuiTaggedEntry * self G_GNUC_UNUSED,
	gboolean modified,
	gpointer user_data G_GNUC_UNUSED
) {

	printf(
		"Signal \"modified-changed\" -- modified: %s\n",
		modified ?
			"true"
		:
			"false"
	);

}


static void on_app_activate (
	GtkApplication * app,
	gpointer user_data G_GNUC_UNUSED
) {

	GtkWidget
		* const window = gtk_application_window_new(app),
		* const my_headerbar = gtk_header_bar_new(),
		* const my_titlebox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0),
		* const my_titlelabel = gtk_label_new("My app"),
		* const my_subtitlelabel = gtk_label_new("Just some app"),
		* const vcontainer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6),
		* const tagged_entry = g_object_new(
			GNUI_TYPE_TAGGED_ENTRY,
			"hexpand", true,
			"vexpand", true,
			"valign", GTK_ALIGN_CENTER,
			"placeholder-text", "Write something here",
			"delimiter-chars", ", ",
			"filter-function", tagged_entry_filter_func,
			NULL
		),
		* const buttonbox = g_object_new(
			GTK_TYPE_BOX,
			"orientation", GTK_ORIENTATION_HORIZONTAL,
			"spacing", 0,
			"hexpand", true,
			NULL
		),
		* iter_button;

	gtk_widget_add_css_class(my_titlelabel, "title");
	gtk_widget_add_css_class(my_subtitlelabel, "subtitle");

	gtk_widget_set_valign(my_titlebox, GTK_ALIGN_CENTER);
	gtk_box_append(GTK_BOX(my_titlebox), my_titlelabel);
	gtk_box_append(GTK_BOX(my_titlebox), my_subtitlelabel);

	gtk_header_bar_set_title_widget(
		GTK_HEADER_BAR(my_headerbar),
		my_titlebox
	);

	gtk_window_set_titlebar(GTK_WINDOW(window), my_headerbar);
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

	g_signal_connect(
		tagged_entry,
		"tag-added",
		G_CALLBACK(signal_test__tag_added),
		NULL
	);

	g_signal_connect(
		tagged_entry,
		"tag-removed",
		G_CALLBACK(signal_test__tag_removed),
		NULL
	);

	g_signal_connect(
		tagged_entry,
		"modified-changed",
		G_CALLBACK(signal_test__modified_changed),
		NULL
	);

	g_signal_connect(
		tagged_entry,
		"activate",
		G_CALLBACK(signal_test__activate),
		NULL
	);

	iter_button = gtk_button_new_with_label("Do something 1");

	g_signal_connect(
		iter_button,
		"clicked",
		G_CALLBACK(on_do_something_1_click),
		tagged_entry
	);

	gtk_box_append(GTK_BOX(buttonbox), iter_button);

	iter_button = gtk_button_new_with_label("Do something 2");

	g_signal_connect(
		iter_button,
		"clicked",
		G_CALLBACK(on_do_something_2_click),
		tagged_entry
	);

	gtk_box_append(GTK_BOX(buttonbox), iter_button);

	iter_button = gtk_button_new_with_label("Clear");

	g_signal_connect(
		iter_button,
		"clicked",
		G_CALLBACK(on_clear_click),
		tagged_entry
	);

	gtk_box_append(GTK_BOX(buttonbox), iter_button);
	gtk_box_append(GTK_BOX(vcontainer), buttonbox);
	gtk_box_append(GTK_BOX(vcontainer), tagged_entry);
	gtk_window_set_child(GTK_WINDOW(window), vcontainer);
	gtk_widget_show(window);

}


int main (
	int argc,
	char ** argv
) {

	int status;

	GtkApplication * app = gtk_application_new(
		"org.gtk.example",
		G_APPLICATION_DEFAULT_FLAGS
	);

	g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;

}
