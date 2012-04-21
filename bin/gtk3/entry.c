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
gtk_container_set_border_width (GTK_CONTAINER (window), 10);
gtk_window_set_resizable(GTK_WINDOW(window), FALSE); /* window is NOT resizable */

/* Here we construct the container that is going pack our buttons */
grid = gtk_grid_new ();
gtk_grid_set_row_spacing (GTK_GRID (grid), 3);

/* Pack the container in the window */
gtk_container_add (GTK_CONTAINER (window), grid);

/* Add labels */
label_user = gtk_label_new("Username  ");
label_host = gtk_label_new("Hostname  ");
label_pass = gtk_label_new("Password  ");

/* Add hostname entry and label */
h_name = gtk_entry_new();
gtk_grid_attach (GTK_GRID (grid), label_host, 0, 0, 1, 1);
gtk_grid_attach (GTK_GRID (grid), h_name, 1, 0, 1, 1);

/* Add username entry and label */
u_name = gtk_entry_new();
gtk_grid_attach (GTK_GRID (grid), label_user, 0, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), u_name, 1, 1, 2, 1);

/* Add password entry and label (visibility set to 0 = not visible */
pass = gtk_entry_new();
gtk_grid_attach (GTK_GRID (grid), label_pass, 0, 2, 1, 1);
gtk_entry_set_visibility (GTK_ENTRY (pass), 0);
gtk_grid_attach (GTK_GRID (grid), pass, 1, 2, 1, 1);

/* Add connect button */
button = gtk_button_new_with_label ("Connect");
g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
gtk_grid_attach (GTK_GRID (grid), button, 0, 3, 2, 1);

/* Add quit button */
button = gtk_button_new_with_label ("Quit");
g_signal_connect (button, "clicked", G_CALLBACK (gtk_main_quit), NULL);
gtk_grid_attach (GTK_GRID (grid), button, 0, 4, 2, 1);

/* Now that we are done packing our widgets, we show them all in one go, by calling gtk_widget_show_all() on the window.*/
gtk_widget_show_all (window);

/* All GTK applications must have a gtk_main(). Control ends here and waits for an event to occur (like a key press or a mouse event),
 * until gtk_main_quit() is called. */
gtk_main ();

return 0;
}
