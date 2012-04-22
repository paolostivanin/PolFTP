#include <gtk/gtk.h>

static void b_clicked (GtkWidget *widget, gpointer data)
{
  g_print ("Connect button clicked\n");
}

static void a_clicked (GtkWidget *widget, gpointer data)
{
  GtkWidget *a_dialog;
  const gchar *authors[] ={
	  "Paolo Stivanin",
	  "Filippo Roncari",
	  "Stefano Agostini",
	  NULL,
  };
  a_dialog = gtk_about_dialog_new ();
  gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (a_dialog), "FTPUtils");
  gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (a_dialog), "1.0-alpha1");
  gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (a_dialog), "2012");
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (a_dialog), "Sviluppato per il progetto di Reti di Calcolatori");
  gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG (a_dialog), GTK_LICENSE_GPL_3_0);
  gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (a_dialog), "https://github.com/polslinux/FTPUtils");
  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (a_dialog), authors);
  gtk_dialog_run(GTK_DIALOG (a_dialog));
  gtk_widget_destroy(a_dialog);
}


int main (int argc, char *argv[]){

GtkWidget *window;
GtkWidget *grid;
GtkWidget *c_button;
GtkWidget *q_button;
GtkWidget *a_button;
GtkWidget *u_name;
GtkWidget *h_name;
GtkWidget *pass;
GtkWidget *label_user;
GtkWidget *label_host;
GtkWidget *label_pass;
//GtkWidget *a_dialog;
/*GtkWidget *menub;
GtkWidget *file_menu;
GtkWidget *file_item;*/

gtk_init (&argc, &argv);

window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
gtk_window_set_title (GTK_WINDOW (window), "FTP Client");
gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
gtk_container_set_border_width (GTK_CONTAINER (window), 10);
gtk_window_set_resizable(GTK_WINDOW(window), FALSE); /* window is NOT resizable */

grid = gtk_grid_new ();
gtk_container_add (GTK_CONTAINER (window), grid);
gtk_grid_set_row_spacing (GTK_GRID (grid), 3);

label_user = gtk_label_new("Username  ");
label_host = gtk_label_new("Hostname  ");
label_pass = gtk_label_new("Password  ");

h_name = gtk_entry_new();
gtk_entry_set_placeholder_text (GTK_ENTRY (h_name), "Hostname"); /* imposto il testo trasparente che scompare quando clicco o scrivo */
u_name = gtk_entry_new();
gtk_entry_set_placeholder_text (GTK_ENTRY (u_name), "Username");
pass = gtk_entry_new();
gtk_entry_set_placeholder_text (GTK_ENTRY (pass), "Password");
gtk_entry_set_visibility (GTK_ENTRY (pass), 0);
c_button = gtk_button_new_with_label ("Connect");
g_signal_connect (c_button, "clicked", G_CALLBACK (b_clicked), NULL);
q_button = gtk_button_new_with_label ("Quit");
g_signal_connect (q_button, "clicked", G_CALLBACK (gtk_main_quit), NULL);
a_button = gtk_button_new_with_label ("About");
g_signal_connect (a_button, "clicked", G_CALLBACK (a_clicked), NULL);

/*
menub = gtk_menu_bar_new ();
file_item = gtk_menu_item_new_with_label ("File");
file_menu = gtk_menu_new ();
gtk_menu_item_set_submenu (GTK_MENU_ITEM (file_item), file_menu);
gtk_menu_shell_append(GTK_MENU_SHELL(menub), file_item);
*/

gtk_grid_attach (GTK_GRID (grid), label_host, 0, 0, 1, 1);
gtk_grid_attach (GTK_GRID (grid), h_name, 1, 0, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label_user, 0, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), u_name, 1, 1, 2, 1);
gtk_grid_attach (GTK_GRID (grid), label_pass, 0, 2, 1, 1);
gtk_grid_attach (GTK_GRID (grid), pass, 1, 2, 1, 1);
gtk_grid_attach (GTK_GRID (grid), c_button, 0, 3, 2, 1);
gtk_grid_attach (GTK_GRID (grid), q_button, 0, 4, 2, 1);
gtk_grid_attach (GTK_GRID (grid), a_button, 0, 5, 2, 1);

/*
gtk_grid_attach (GTK_GRID (grid), menub, 0, 5, 2, 1);
*/

gtk_widget_show_all (window);

gtk_main ();

return 0;
}
