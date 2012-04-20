#include <gtk/gtk.h>

/* When "connect" button is clicked a message will appear */
static void print_hello (GtkWidget *widget, gpointer data)
{
  g_print ("Connect button clicked\n");
}

int main (int argc, char *argv[]){

/* Declare widgets */
GtkWidget *window;
GtkWidget *grid;
GtkWidget *table;
GtkWidget *button;
GtkWidget *u_name;
GtkWidget *h_name;
GtkWidget *pass;
GtkWidget *label_user;
GtkWidget *label_host;
GtkWidget *label_pass;

/*This is called in all GTK applications. Arguments are parsed from the command line and are returned to the application.*/
gtk_init (&argc, &argv);

/* create a new window, set its title and put it on center */
window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
gtk_window_set_title (GTK_WINDOW (window), "FTP Client");
gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
gtk_container_set_border_width (GTK_CONTAINER (window), 20);

/* Here we construct the container that is going pack our buttons */
table = gtk_table_new(4, 2, TRUE);
gtk_table_set_row_spacings(GTK_TABLE(table), 1);

/* Pack the container in the window */
gtk_container_add (GTK_CONTAINER (window), table);

/* Add labels */
label_user = gtk_label_new("Username");
label_host = gtk_label_new("Hostname");
label_pass = gtk_label_new("Password");

/* Add hostname entry and label */
h_name = gtk_entry_new();
gtk_entry_set_text(GTK_ENTRY(h_name), "Hostname"); /* Set label into entry */

/* Add username entry and label */
u_name = gtk_entry_new();
gtk_entry_set_text(GTK_ENTRY(u_name), "Username");

/* Add password entry and label (visibility set to 0 = not visible */
pass = gtk_entry_new();
gtk_entry_set_visibility (GTK_ENTRY (pass), 0);
gtk_entry_set_text(GTK_ENTRY(pass), "Password");

/* Add elements to table */
gtk_table_attach_defaults(GTK_TABLE(table), h_name, 1, 2, 0, 1);
gtk_table_attach_defaults(GTK_TABLE(table), u_name, 1, 2, 1, 2);
gtk_table_attach_defaults(GTK_TABLE(table), pass, 1, 2, 2, 3);
gtk_table_attach_defaults(GTK_TABLE(table), label_host, 0, 1, 0, 1);
gtk_table_attach_defaults(GTK_TABLE(table), label_user, 0, 1, 1, 2);
gtk_table_attach_defaults(GTK_TABLE(table), label_pass, 0, 1, 2, 3);

/* Add connect button */
button = gtk_button_new_with_label ("Connect");
g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
gtk_table_attach_defaults(GTK_TABLE(table), button, 0, 1, 3, 4);

/* Add quit button */
button = gtk_button_new_with_label ("Quit");
g_signal_connect (button, "clicked", G_CALLBACK (gtk_main_quit), NULL);
gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 2, 3, 4);

/* Now that we are done packing our widgets, we show them all in one go, by calling gtk_widget_show_all() on the window.*/
gtk_widget_show_all (window);

/* All GTK applications must have a gtk_main(). Control ends here and waits for an event to occur (like a key press or a mouse event),
 * until gtk_main_quit() is called. */
gtk_main ();

return 0;
}
