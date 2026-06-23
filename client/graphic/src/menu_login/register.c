#include "../../include/menu_login/register.h"
#include "menu_login/register_logic.h"
#include <string.h>

static void on_register_clicked(GtkButton *btn, gpointer data) {
    AppWidgets *w = (AppWidgets *)data;
    const char *nom      = gtk_entry_get_text(GTK_ENTRY(w->entry_reg_nom));
    const char *prenom   = gtk_entry_get_text(GTK_ENTRY(w->entry_reg_prenom));
    const char *pseudo   = gtk_entry_get_text(GTK_ENTRY(w->entry_reg_pseudo));
    const char *email    = gtk_entry_get_text(GTK_ENTRY(w->entry_reg_email));
    const char *password = gtk_entry_get_text(GTK_ENTRY(w->entry_reg_password));

    const char *message = NULL;

    switch (validate_register(nom, prenom, pseudo, email, password)) {
        case REGISTER_CHAMPS_VIDES:
            message = "Veuillez remplir tous les champs."; break;
        case REGISTER_PASSWORD_TROP_COURT:
            message = "Le mot de passe doit contenir au moins 6 caractères."; break;
        case REGISTER_PASSWORD_FAIBLE:
            message = "Le mot de passe doit contenir au moins un chiffre ou un caractère spécial."; break;
        case REGISTER_EMAIL_INVALIDE:
            message = "Veuillez entrer une adresse email valide."; break;
        case REGISTER_OK:
            g_print("Register: %s %s / %s / %s\n", prenom, nom, pseudo, email);
            // TODO: envoyer au serveur
            return;
    }

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(w->window),
        GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
}

static void go_to_login(GtkButton *btn, gpointer data) {
    AppWidgets *w = (AppWidgets *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(w->stack), "login");
}

GtkWidget *build_register_page(AppWidgets *w) {
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

    GtkWidget *title = gtk_label_new("Créer un compte");
    gtk_widget_set_name(title, "title");
    gtk_box_pack_start(GTK_BOX(box), title, FALSE, FALSE, 0);

    GtkWidget *subtitle = gtk_label_new("Rejoins la communauté !");
    gtk_widget_set_name(subtitle, "subtitle");
    gtk_box_pack_start(GTK_BOX(box), subtitle, FALSE, FALSE, 8);

    GtkWidget *lbl_nom = gtk_label_new("NOM");
    gtk_widget_set_halign(lbl_nom, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), lbl_nom, FALSE, FALSE, 0);

    w->entry_reg_nom = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(w->entry_reg_nom), "Ton nom");
    gtk_box_pack_start(GTK_BOX(box), w->entry_reg_nom, FALSE, FALSE, 0);

    GtkWidget *lbl_prenom = gtk_label_new("PRÉNOM");
    gtk_widget_set_halign(lbl_prenom, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), lbl_prenom, FALSE, FALSE, 0);

    w->entry_reg_prenom = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(w->entry_reg_prenom), "Ton prénom");
    gtk_box_pack_start(GTK_BOX(box), w->entry_reg_prenom, FALSE, FALSE, 0);

    GtkWidget *lbl_pseudo = gtk_label_new("PSEUDO");
    gtk_widget_set_halign(lbl_pseudo, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), lbl_pseudo, FALSE, FALSE, 0);

    w->entry_reg_pseudo = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(w->entry_reg_pseudo), "Choisis un pseudo");
    gtk_box_pack_start(GTK_BOX(box), w->entry_reg_pseudo, FALSE, FALSE, 0);

    GtkWidget *lbl_email = gtk_label_new("EMAIL");
    gtk_widget_set_halign(lbl_email, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), lbl_email, FALSE, FALSE, 0);

    w->entry_reg_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(w->entry_reg_email), "ton@email.com");
    gtk_box_pack_start(GTK_BOX(box), w->entry_reg_email, FALSE, FALSE, 0);

    GtkWidget *lbl_pass = gtk_label_new("MOT DE PASSE");
    gtk_widget_set_halign(lbl_pass, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), lbl_pass, FALSE, FALSE, 0);

    w->entry_reg_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(w->entry_reg_password), "Choisis un mot de passe");
    gtk_entry_set_visibility(GTK_ENTRY(w->entry_reg_password), FALSE);
    gtk_box_pack_start(GTK_BOX(box), w->entry_reg_password, FALSE, FALSE, 0);

    GtkWidget *btn_register = gtk_button_new_with_label("S'inscrire");
    gtk_widget_set_name(btn_register, "suggested-action");
    g_signal_connect(btn_register, "clicked", G_CALLBACK(on_register_clicked), w);
    gtk_box_pack_start(GTK_BOX(box), btn_register, FALSE, FALSE, 8);

    GtkWidget *btn_back = gtk_button_new_with_label("Déjà un compte ? Se connecter");
    g_signal_connect(btn_back, "clicked", G_CALLBACK(go_to_login), w);
    gtk_box_pack_start(GTK_BOX(box), btn_back, FALSE, FALSE, 0);

    return outer;
}