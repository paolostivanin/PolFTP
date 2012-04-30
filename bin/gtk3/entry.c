/* Descrizione: GUI del Client FTP sviluppato come progetto per il corso di Reti di Calcolatori
 * Sviluppatori: Filippo Roncari, Paolo Stivanin, Stefano Agostini 
 * Copyright: 2012
 * Licenza: GNU GPL v3 <http://www.gnu.org/licenses/gpl-3.0.html>
 * Sito web: <https://github.com/polslinux/FTPUtils>
 */

#include <gtk/gtk.h>

/* Definisco la struttura getinfo */
typedef struct {
	GtkWidget *host;
	GtkWidget *user;
	GtkWidget *passwd;
} getinfo;

/* Funzione attivata alla pressione del pulsante connect */
void b_clicked (GtkButton *c_button, getinfo *data){
	g_print("Hostname: %s\nUsername: %s\nPassword: %s\n", 
		gtk_entry_get_text(GTK_ENTRY(data->host)), gtk_entry_get_text(GTK_ENTRY(data->user)), gtk_entry_get_text(GTK_ENTRY(data->passwd)));
}

/* Funzione attivata alla pressione del pulsante about */
static void a_clicked (GtkWidget *a_dialog, gpointer data){

	const gchar *authors[] = /* Qui definisco gli autori*/
	{
		"Filippo Roncari",
		"Paolo Stivanin",
		"Stefano Agostini",
		NULL,
	};

	a_dialog = gtk_about_dialog_new ();
	gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (a_dialog), "FTP UTILS");
	gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (a_dialog), "1.0-prealpha");
	gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (a_dialog), "2012");
	gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (a_dialog), "Semplice client FTP sviluppato in C/GTK+");
	gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG (a_dialog), GTK_LICENSE_GPL_3_0);
	gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (a_dialog), "https://github.com/polslinux/FTPUtils");
	gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (a_dialog), authors);
  
	gtk_dialog_run(GTK_DIALOG (a_dialog)); /* Avvio il dialog a_dialog */
	gtk_widget_destroy(a_dialog); /* Alla pressione del pulsante chiudi il widget viene chiuso */
}


int main (int argc, char *argv[]){

	/* Dichiaro i widget che saranno usati */
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
	
	/* Definiamo un nuovo tipo di nome (in pratica una struttura) */
	getinfo data;
	
	/* Inizializziamo le librerie GTK+ */
	gtk_init (&argc, &argv);

	/* Creiamo la finestra principale, impostiamo il titolo, la posizione e la dimensione */
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "FTP Client");
	gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW(window), 350, 250);
	/* Quando premiamo chiudi si 'disturgge' tutto */
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	/* Impostiamo il bordo e dichiariamo le finestra ridimensionabile */
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);
	gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

	/* Creiamo una nuova griglia, la aggiungiamo alla finestra e impostiamo gli spazi fra i vari widget */
	grid = gtk_grid_new ();
	gtk_container_add (GTK_CONTAINER (window), grid);
	gtk_grid_set_row_spacing (GTK_GRID (grid), 3);

	/* Creiamo delle label con il relativo nome */
	label_user = gtk_label_new("Username  ");
	label_host = gtk_label_new("Hostname  ");
	label_pass = gtk_label_new("Password  ");

	/* Creo una entry_text */
	h_name = gtk_entry_new();
	data.host = h_name;
	/* Impostiamo il testo trasparente che scompare quando clicco o scrivo */
	gtk_entry_set_placeholder_text (GTK_ENTRY (h_name), "Hostname");
	u_name = gtk_entry_new();
	data.user = u_name;
	gtk_entry_set_placeholder_text (GTK_ENTRY (u_name), "Username");
	pass = gtk_entry_new();
	data.passwd = pass;
	gtk_entry_set_placeholder_text (GTK_ENTRY (pass), "Password");
	/* Impostiamo il campo password non visibile */
	gtk_entry_set_visibility (GTK_ENTRY (pass), 0);
	/* Creiamo i pulsanti e li connettiamo alle relative funzioni */
	c_button = gtk_button_new_with_label ("Connect");
	g_signal_connect (c_button, "clicked", G_CALLBACK (b_clicked), &data);
	q_button = gtk_button_new_with_label ("Quit");
	g_signal_connect (q_button, "clicked", G_CALLBACK (gtk_main_quit), NULL);
	a_button = gtk_button_new_with_label ("About");
	g_signal_connect (a_button, "clicked", G_CALLBACK (a_clicked), NULL);

	/* Imposto i widget come espandibili sia in orizzontale che in verticale */
	gtk_widget_set_hexpand(u_name, TRUE);
	gtk_widget_set_vexpand(u_name, TRUE);
	gtk_widget_set_hexpand(h_name, TRUE);
	gtk_widget_set_vexpand(h_name, TRUE);
	gtk_widget_set_hexpand(pass, TRUE);
	gtk_widget_set_vexpand(pass, TRUE);

	/* Posizioniamo i widget all'interno della griglia */
	gtk_grid_attach (GTK_GRID (grid), label_host, 0, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), h_name, 1, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), label_user, 0, 1, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), u_name, 1, 1, 2, 1);
	gtk_grid_attach (GTK_GRID (grid), label_pass, 0, 2, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), pass, 1, 2, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), c_button, 0, 3, 2, 1);
	gtk_grid_attach (GTK_GRID (grid), q_button, 0, 4, 2, 1);
	gtk_grid_attach (GTK_GRID (grid), a_button, 0, 5, 2, 1);

	/* Mostriamo tutti i widget */
	gtk_widget_show_all (window);

	/* Entriamo nel loop principale delle GTK+, da questo punto  l'applicazione attende l'accadere di un qualsiasi evento */
	gtk_main ();

	return 0;
}
