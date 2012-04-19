#include <gtk/gtk.h>

static void
print_hello (GtkWidget *widget,
     gpointer   data)
{
  g_print ("Connect button clicked\n");
}

int
main (int   argc,
      char *argv[])
{
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *button;
  GtkWidget *u_name;
  GtkWidget *h_name;
  GtkWidget *pass;

  /* This is called in all GTK applications. Arguments are parsed
   * from the command line and are returned to the application.
   */
  gtk_init (&argc, &argv);

  /* create a new window, and set its title */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Grid");
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();

  /* Pack the container in the window */
  gtk_container_add (GTK_CONTAINER (window), grid);

  h_name = gtk_entry_new();
  gtk_grid_attach (GTK_GRID (grid), h_name, 0, 0, 1, 1); /* Place the first button in the grid cell (0, 0), and make it fill just 1 cell horizontally and vertically (ie no spanning)*/

  u_name = gtk_entry_new();
  gtk_grid_attach (GTK_GRID (grid), u_name, 0, 1, 1, 1);

  pass = gtk_entry_new();
  gtk_grid_attach (GTK_GRID (grid), pass, 0, 2, 1, 1);
  
  button = gtk_button_new_with_label ("Connect");
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  gtk_grid_attach (GTK_GRID (grid), button, 0, 3, 2, 1);
  
  button = gtk_button_new_with_label ("Quit");
  g_signal_connect (button, "clicked", G_CALLBACK (gtk_main_quit), NULL);
  gtk_grid_attach (GTK_GRID (grid), button, 0, 4, 2, 1); /*Place the Quit button in the grid cell (0, 4), and make it span 2 columns.*/

  /* Now that we are done packing our widgets, we show them all
   * in one go, by calling gtk_widget_show_all() on the window.
   * This call recursively calls gtk_widget_show() on all widgets
   * that are contained in the window, directly or indirectly.
   */
  gtk_widget_show_all (window);

  /* All GTK applications must have a gtk_main(). Control ends here
   * and waits for an event to occur (like a key press or a mouse event),
   * until gtk_main_quit() is called.
   */
  gtk_main ();

  return 0;
}
