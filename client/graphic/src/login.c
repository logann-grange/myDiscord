#include <login.h> 


static void on_login_clicked(GtkButton *btn, gpointer data) 
{
    LoginWidgets *w = (LoginWidgets *)data;
    const char *pseudo   = gtk_entry_get_text(GTK_ENTRY(w->entry_pseudo));
    const char *password = gtk_entry_get_text(GTK_ENTRY(w->entry_password));

    if (strlen(pseudo) == 0 || strlen(password) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(w->window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "Veuillez remplir tous les champs.");
        gtk_widget_show_all(dialog);
        g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
        return;
    }

    // TODO: envoyer au serveur via socket
    g_print("Login: %s\n", pseudo);
}

static void on_register_clicked(GtkButton *btn, gpointer data) 
{
    LoginWidgets *w = (LoginWidgets *)data;
    gtk_widget_destroy(w->window);
    show_register_window(w->app);
}

void show_login_window(GtkApplication *app) 
{
    LoginWidgets *w = g_malloc(sizeof(LoginWidgets));
    w->app = app;

    w->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(w->window), "Conexion");
    gtk_window_set_default_size(GTK_WINDOW(w->window), 400, 300);
    gtk_window_set_resizable(GTK_WINDOW(w->window), FALSE);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);
    gtk_container_add(GTK_CONTAINER(w->window), box);

    GtkWidget *title= gtk_label_new("Bon retour");
    gtk_box_pack_start(GTK_BOX(box), title, FALSE, FALSE, 0);

    GtkWidget *subtitle= gtk_label_new("Conne-toi à ton compte");
    gtk_box_pack_start(GTK_BOX(box), subtitle, FALSE, FALSE, 0);

    // Pseudo 
    GtkWidget *label_pseudo = gtk_label_new("Pseudo");
    gtk_widget_set_halign(label_pseudo, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), label_pseudo, FALSE, FALSE, 0);
    

}