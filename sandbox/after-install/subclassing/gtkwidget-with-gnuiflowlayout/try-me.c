/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*  Please make sure that the TAB width in your editor is set to 4 spaces  */


#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include "xyz-foo-bar.h"


static void activate (
	GtkApplication * app,
	gpointer user_data G_GNUC_UNUSED
) {

	GtkWidget
		* const window = gtk_application_window_new(app),
		* const foo_bar = xyz_foo_bar_new();

	gtk_window_set_title(GTK_WINDOW(window), "Hello world");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
	gtk_window_set_child(GTK_WINDOW (window), foo_bar);
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

	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;

}


/*  EOF  */
