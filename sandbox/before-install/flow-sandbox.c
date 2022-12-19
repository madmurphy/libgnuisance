/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*  Please make sure that the TAB width in your editor is set to 4 spaces  */


#include <gtk/gtk.h>
#include "../../src/widgets/flow/gnui-flow.h"


static void activate (
	GtkApplication * const app,
	gpointer user_data G_GNUC_UNUSED
) {

	GtkWidget
		* const window = gtk_application_window_new(app),
		* const flow = gnui_flow_new_with_params_and_children(
			GTK_ORIENTATION_HORIZONTAL,
			6,
			6,
			gtk_label_new("One"),
			gtk_label_new("Two"),
			gtk_label_new("Three"),
			gtk_label_new("Four"),
			gtk_label_new("Five"),
			gtk_label_new("Six"),
			gtk_label_new("Seven"),
			gtk_label_new("Eight"),
			gtk_label_new("Nine"),
			gtk_label_new("Ten"),
			gtk_label_new("Eleven"),
			gtk_label_new("Twelve"),
			gtk_label_new("Thirteen"),
			gtk_label_new("Fourteen"),
			gtk_label_new("Fifteen"),
			gtk_label_new("Sixteen"),
			gtk_label_new("Seventeen"),
			gtk_label_new("Eighteen"),
			gtk_label_new("Nineteen"),
			gtk_label_new("Twenty"),
			NULL
		);

	gnui_flow_insert(GNUI_FLOW(flow), gtk_label_new("Test"), -1);
	gtk_window_set_title(GTK_WINDOW(window), "Window");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
	gtk_window_set_child(GTK_WINDOW(window), flow);
	gtk_widget_show(window);

}

int main (
	const int argc,
	char ** const argv
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
