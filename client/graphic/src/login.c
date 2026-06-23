#include "login.h"


static void on_login_clicked(GtkButton *btn, gpointer data) {
    AppWidgets *w = (AppWidgets *)data;
    const char *pseudo   = gtk_entry_get_text(GTK_ENTRY(w->entry_login_pseudo));
    const char *password = gtk_entry_get_text(GTK_ENTRY(w->entry_login_password));

    if (strlen(pseudo) == 0 || strlen(password) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(w->window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "Veuillez remplir tous les champs.");
        gtk_widget_show_all(dialog);
        g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
        return;
    }
    g_print("Login: %s\n", pseudo);
    // TODO: envoyer au serveur
}

static void go_to_register(GtkButton *btn, gpointer data) {
    AppWidgets *w = (AppWidgets *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(w->stack), "register");
}

GtkWidget *build_login_page(AppWidgets *w) {
    GtkWidget *outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(outer, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(outer, GTK_ALIGN_CENTER);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_size_request(box, 400, -1);
    gtk_widget_set_margin_top(box, 48);
    gtk_widget_set_margin_bottom(box, 48);
    gtk_widget_set_margin_start(box, 48);
    gtk_widget_set_margin_end(box, 48);
    gtk_container_add(GTK_CONTAINER(outer), box);

    GtkWidget *title = gtk_label_new("Bon retour !");
    gtk_widget_set_name(title, "title");
    gtk_box_pack_start(GTK_BOX(box), title, FALSE, FALSE, 0);

    GtkWidget *subtitle = gtk_label_new("Connecte-toi à ton compte");
    gtk_widget_set_name(subtitle, "subtitle");
    gtk_box_pack_start(GTK_BOX(box), subtitle, FALSE, FALSE, 8);

    GtkWidget *lbl_pseudo = gtk_label_new("PSEUDO");
    gtk_widget_set_halign(lbl_pseudo, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), lbl_pseudo, FALSE, FALSE, 0);

    w->entry_login_pseudo = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(w->entry_login_pseudo), "Ton pseudo");
    gtk_box_pack_start(GTK_BOX(box), w->entry_login_pseudo, FALSE, FALSE, 0);

    GtkWidget *lbl_pass = gtk_label_new("MOT DE PASSE");
    gtk_widget_set_halign(lbl_pass, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), lbl_pass, FALSE, FALSE, 0);

    w->entry_login_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(w->entry_login_password), "Ton mot de passe");
    gtk_entry_set_visibility(GTK_ENTRY(w->entry_login_password), FALSE);
    gtk_box_pack_start(GTK_BOX(box), w->entry_login_password, FALSE, FALSE, 0);

    GtkWidget *btn_login = gtk_button_new_with_label("Se connecter");
    gtk_widget_set_name(btn_login, "suggested-action");
    g_signal_connect(btn_login, "clicked", G_CALLBACK(on_login_clicked), w);
    gtk_box_pack_start(GTK_BOX(box), btn_login, FALSE, FALSE, 8);

    GtkWidget *btn_reg = gtk_button_new_with_label("Pas de compte ? S'inscrire");
    g_signal_connect(btn_reg, "clicked", G_CALLBACK(go_to_register), w);
    gtk_box_pack_start(GTK_BOX(box), btn_reg, FALSE, FALSE, 0);

    return outer;
}