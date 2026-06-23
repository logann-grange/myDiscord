#include "../../include/main_chat/side_bar.h"

static void on_channel_clicked(GtkButton *btn, gpointer data) {
    // TODO: changer de canal
    const char *channel = gtk_button_get_label(btn);
    g_print("Canal sélectionné : %s\n", channel);
}

static GtkWidget *build_channel_button(const char *name, gboolean active) {
    GtkWidget *btn = gtk_button_new_with_label(name);
    gtk_widget_set_name(btn, active ? "channel-active" : "channel");
    gtk_widget_set_halign(btn, GTK_ALIGN_FILL);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_channel_clicked), NULL);
    return btn;
}

static GtkWidget *build_category(const char *name) {
    GtkWidget *label = gtk_label_new(name);
    gtk_widget_set_name(label, "category-label");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_margin_top(label, 12);
    gtk_widget_set_margin_start(label, 8);
    return label;
}

GtkWidget *build_sidebar(ChatWidgets *w) {
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(sidebar, "sidebar");
    gtk_widget_set_size_request(sidebar, 240, -1);

    // Header serveur
    GtkWidget *server_header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(server_header, "server-header");
    gtk_widget_set_margin_top(server_header, 0);

    GtkWidget *server_name = gtk_label_new("Dev Community");
    gtk_widget_set_name(server_name, "server-name");
    gtk_widget_set_margin_start(server_name, 16);
    gtk_widget_set_margin_top(server_name, 14);
    gtk_widget_set_margin_bottom(server_name, 14);
    gtk_widget_set_halign(server_name, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(server_header), server_name, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), server_header, FALSE, FALSE, 0);

    // Séparateur
    gtk_box_pack_start(GTK_BOX(sidebar), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);

    // Zone scrollable pour les canaux
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(sidebar), scroll, TRUE, TRUE, 0);

    GtkWidget *channels_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_margin_start(channels_box, 8);
    gtk_widget_set_margin_end(channels_box, 8);
    gtk_container_add(GTK_CONTAINER(scroll), channels_box);

    // Catégorie GÉNÉRAL
    gtk_box_pack_start(GTK_BOX(channels_box), build_category("GÉNÉRAL"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("# accueil", FALSE), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("# général", TRUE), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("# annonces", FALSE), FALSE, FALSE, 0);

    // Catégorie DÉVELOPPEMENT
    gtk_box_pack_start(GTK_BOX(channels_box), build_category("DÉVELOPPEMENT"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("# frontend", FALSE), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("# backend", FALSE), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("# code-review", FALSE), FALSE, FALSE, 0);

    // Catégorie RESSOURCES
    gtk_box_pack_start(GTK_BOX(channels_box), build_category("RESSOURCES"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("# liens-utiles", FALSE), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("# outils", FALSE), FALSE, FALSE, 0);

    // Zone utilisateur en bas
    GtkWidget *user_area = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_name(user_area, "user-area");
    gtk_widget_set_margin_start(user_area, 8);
    gtk_widget_set_margin_end(user_area, 8);
    gtk_widget_set_margin_top(user_area, 8);
    gtk_widget_set_margin_bottom(user_area, 8);

    GtkWidget *avatar = gtk_label_new("M");
    gtk_widget_set_name(avatar, "avatar");
    gtk_box_pack_start(GTK_BOX(user_area), avatar, FALSE, FALSE, 0);

    GtkWidget *user_info = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *username = gtk_label_new("moi");
    gtk_widget_set_name(username, "username");
    gtk_widget_set_halign(username, GTK_ALIGN_START);
    GtkWidget *status = gtk_label_new("en ligne");
    gtk_widget_set_name(status, "user-status");
    gtk_widget_set_halign(status, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(user_info), username, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(user_info), status, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(user_area), user_info, TRUE, TRUE, 0);

    GtkWidget *settings_btn = gtk_button_new_with_label("⚙");
    gtk_widget_set_name(settings_btn, "settings-btn");
    gtk_box_pack_start(GTK_BOX(user_area), settings_btn, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(sidebar), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), user_area, FALSE, FALSE, 0);

    return sidebar;
}