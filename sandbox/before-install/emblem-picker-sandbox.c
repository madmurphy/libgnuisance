/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*  Please make sure that the TAB width in your editor is set to 4 spaces  */


#include <stdbool.h>
#include <adwaita.h>
#include "../../src/widgets/emblem-picker/gnui-emblem-picker.h"


GtkWindow * test_win;


gboolean test_callback__save (
	GFile * const self,
	const gchar * const * const added_emblems,
	const gchar * const * const removed_emblems,
	const GnuiEmblemPickerSaveResult result,
	const GError * const error,
	gpointer user_data G_GNUC_UNUSED
) {

	printf(
		"\n\nSave callback\nFile: %s\nEmblems added: \n",
		g_file_peek_path(self)
	);

	if (added_emblems) {

		for (gsize tmp = 0; added_emblems[tmp]; tmp++) {

			printf(" - %s\n", added_emblems[tmp]);

		}

	} else {

		printf("None\n");

	}

	printf("\nEmblems removed: \n");

	if (removed_emblems) {

		for (gsize tmp = 0; removed_emblems[tmp]; tmp++) {

			printf(" - %s\n", removed_emblems[tmp]);

		}

	} else {

		printf("None\n");

	}

	printf(
		"Result: %s\n",
		result == GNUI_EMBLEM_PICKER_SUCCESS ?
			"success"
		: result == GNUI_EMBLEM_PICKER_ERROR ?
			"error"
		:
			"no action"
	);

	if (error) {

		g_message(
			"%s // %s",
			"Could not save file's emblems",
			error->message
		);

	}

	return true;

}


static void on_change_files_response (
	GtkDialog * const dialog,
	const int response,
	const gpointer v_picker
) {

	if (response == GTK_RESPONSE_ACCEPT) {

		GListModel * const files = gtk_file_chooser_get_files(
			GTK_FILE_CHOOSER(dialog)
		);

		GList * flist = NULL;
		GFile * location;
		gsize idx = 0;

		while ((location = g_list_model_get_item(files, idx++))) {

			flist = g_list_prepend(flist, location);

		}

		g_object_unref(files);

		gnui_emblem_picker_set_mapped_files(
			v_picker,
			flist
		);

		g_list_free_full(flist, g_object_unref);

	}

	gtk_window_destroy(GTK_WINDOW(dialog));

}


static gboolean callback_test__foreach (
	GnuiEmblemPicker * const picker G_GNUC_UNUSED,
	const gchar * const emblem_name,
	const GnuiEmblemState saved_state,
	const GnuiEmblemState current_state,
	const GList * const inconsistent_group,
	gpointer user_data G_GNUC_UNUSED
) {

	printf(
		"Emblem: %s | [%d - %d] // %zu\n",
		emblem_name,
		saved_state,
		current_state,
		(gsize) inconsistent_group
	);

	return true;
}


static void on_do_something_button_click (
	GtkButton * const button,
	const gpointer v_picker
) {

	(void) button;
	(void) v_picker;
	printf("Do something!\n");

}


static void on_foreach_button_click (
	GtkButton * const button G_GNUC_UNUSED,
	const gpointer v_picker
) {

	gnui_emblem_picker_foreach(v_picker, callback_test__foreach, NULL);

}


static void on_change_files_button_click (
	GtkButton * const button G_GNUC_UNUSED,
	const gpointer v_picker
) {

	GtkWidget * const dialog = gtk_file_chooser_dialog_new(
		"Open File",
		test_win,
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"_Cancel",
		GTK_RESPONSE_CANCEL,
		"_Open",
		GTK_RESPONSE_ACCEPT,
		NULL
	);

	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), true);
	gtk_widget_show(dialog);

	g_signal_connect(
		dialog,
		"response",
		G_CALLBACK(on_change_files_response),
		v_picker
	);

}


static void on_save_button_click (
	GtkButton * const button G_GNUC_UNUSED,
	const gpointer v_picker
) {

	if (
		!gnui_emblem_picker_save(
			v_picker,
			test_callback__save,
			//GNUI_EMBLEM_PICKER_SAVE_FLAG_DONT_REFRESH |
			//GNUI_EMBLEM_PICKER_SAVE_FLAG_CLEAN_INCONSISTENCY |
			GNUI_EMBLEM_PICKER_SAVE_FLAGGROUP_ALL_CALLBACKS |
			GNUI_EMBLEM_PICKER_SAVE_FLAG_SAVE_UNMODIFIED,
			NULL,
			NULL,
			NULL
		)
	) {

		g_message("Could not save one or more emblems");

	}

}


static void signal_test__emblem_selected (
	GnuiEmblemPicker * const picker G_GNUC_UNUSED,
	const gchar * const emblem_name,
	const GnuiEmblemState saved_state,
	const GnuiEmblemState current_state,
	const GList * const inconsistent_group,
	gpointer user_data G_GNUC_UNUSED
) {

	printf("Signal \"emblem-selected\" -- emblem \"%s\"\n", emblem_name);
	printf("Saved state: %u\n", saved_state);
	printf("Current state: %u\n", current_state);

	if (inconsistent_group) {

		printf("Inconsistent group:\n");

		for (const GList * llnk = inconsistent_group; llnk; llnk = llnk->next) {

			printf(" - %s\n", g_file_peek_path(llnk->data));

		}

	}

	putchar('\n');

}


static void signal_test__modified_changed (
	GnuiEmblemPicker * const picker G_GNUC_UNUSED,
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


static void on_open_accept (
	GList * file_selection,
	GtkWidget * my_window
) {

	GtkCssProvider * const app_css_provider = gtk_css_provider_new();

	GFile * const css_file =
		g_file_new_for_path("../../src/widgets/emblem-picker/style.css");

	gtk_css_provider_load_from_file(app_css_provider, css_file);

	gtk_style_context_add_provider_for_display(
		gdk_display_get_default(),
		GTK_STYLE_PROVIDER(app_css_provider),
		GTK_STYLE_PROVIDER_PRIORITY_SETTINGS
	);

	g_object_unref(css_file);

	GtkWidget
		* const my_headerbar = gtk_header_bar_new(),
		* const my_titlebox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0),
		* const my_titlelabel = gtk_label_new("My app"),
		* const my_subtitlelabel = gtk_label_new("Just some app"),
		* const picker = g_object_new(
			GNUI_TYPE_EMBLEM_PICKER,
			"mapped-files", file_selection,
			"ensure-standard", true,
			"reveal-changes", true,
			"forbidden-emblems", (const char * const []) {
				"emblem-annotations",
				"emblem-annotations-symbolic",
				NULL
			},
			"halign", GTK_ALIGN_CENTER,
			"valign", GTK_ALIGN_CENTER,
			NULL
		),
		* iter_button;

	gtk_widget_add_css_class(my_titlelabel, "title");
	gtk_widget_add_css_class(my_subtitlelabel, "subtitle");
	gtk_widget_set_valign(my_titlebox, GTK_ALIGN_CENTER);
	gtk_box_append(GTK_BOX(my_titlebox), my_titlelabel);
	gtk_box_append(GTK_BOX(my_titlebox), my_subtitlelabel);
	gtk_header_bar_set_title_widget(GTK_HEADER_BAR(my_headerbar), my_titlebox);
	gtk_window_set_titlebar(GTK_WINDOW(my_window), my_headerbar);

	g_signal_connect(
		picker,
		"emblem-selected",
		G_CALLBACK(signal_test__emblem_selected),
		NULL
	);

	g_signal_connect(
		picker,
		"modified-changed",
		G_CALLBACK(signal_test__modified_changed),
		NULL
	);

	iter_button = gtk_button_new_with_mnemonic("_Save");

	g_signal_connect(
		iter_button,
		"clicked",
		G_CALLBACK(on_save_button_click),
		picker
	);

	gtk_header_bar_pack_end(GTK_HEADER_BAR(my_headerbar), iter_button);

	iter_button = gtk_button_new_with_mnemonic("_Change files");

	g_signal_connect(
		iter_button,
		"clicked",
		G_CALLBACK(on_change_files_button_click),
		picker
	);

	gtk_header_bar_pack_end(GTK_HEADER_BAR(my_headerbar), iter_button);

	iter_button = gtk_button_new_with_mnemonic("For-each");

	g_signal_connect(
		iter_button,
		"clicked",
		G_CALLBACK(on_foreach_button_click),
		picker
	);

	gtk_header_bar_pack_end(GTK_HEADER_BAR(my_headerbar), iter_button);

	iter_button = gtk_button_new_with_mnemonic("Do something");

	g_signal_connect(
		iter_button,
		"clicked",
		G_CALLBACK(on_do_something_button_click),
		picker
	);

	gtk_header_bar_pack_end(GTK_HEADER_BAR(my_headerbar), iter_button);
	gtk_window_set_child(GTK_WINDOW(my_window), picker);
	gtk_window_set_default_size(GTK_WINDOW(my_window), 600, 400);
	gtk_widget_show(my_window);

}


static void on_open_response (
	GtkDialog * const dialog,
	const int response,
	const gpointer my_window
) {

	if (response == GTK_RESPONSE_ACCEPT) {

		GListModel * const files = gtk_file_chooser_get_files(
			GTK_FILE_CHOOSER(dialog)
		);

		GList * flist = NULL;
		GFile * location;
		gsize idx = 0;

		while ((location = g_list_model_get_item(files, idx++))) {

			flist = g_list_prepend(flist, location);

		}

		g_object_unref(files);
		on_open_accept(flist, my_window);
		g_list_free_full(flist, g_object_unref);

	} else {

		gtk_window_destroy(GTK_WINDOW(my_window));

	}

	gtk_window_destroy(GTK_WINDOW(dialog));

}


void on_app_activate (
	GtkApplication * app,
	gpointer data G_GNUC_UNUSED
) {

	test_win = GTK_WINDOW(gtk_application_window_new(app));

	GtkWidget * const dialog = gtk_file_chooser_dialog_new(
		"Open File",
		test_win,
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"_Cancel",
		GTK_RESPONSE_CANCEL,
		"_Open",
		GTK_RESPONSE_ACCEPT,
		NULL
	);

	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), true);
	gtk_widget_show(dialog);

	g_signal_connect(
		dialog,
		"response",
		G_CALLBACK(on_open_response),
		test_win
	);

}


int main (
	const int argc,
	char ** const argv
) {

	int status;

	AdwApplication * app = adw_application_new(
		"org.example.Hello",
		G_APPLICATION_DEFAULT_FLAGS
	);

	g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;

}
