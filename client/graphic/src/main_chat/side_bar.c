#include "../../include/main_chat/side_bar.h"
#include "../../include/main_chat/messagerie.h"
#include "../../include/moderateur/mod_panel.h"
#include "../../include/moderateur/admin_panel.h"
#include <string.h>
#include <stdio.h>

static void on_mod_admin_btn_clicked(GtkButton *btn, gpointer data) {
    AppWidgets *w = (AppWidgets *)data;
    gtk_widget_show_all(w->panel_popover);
}

static void on_settings_btn_clicked(GtkButton *btn, gpointer data) {
    AppWidgets *w = (AppWidgets *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(w->stack), "settings");
}

static void on_channel_data_free(gpointer data, GClosure *closure) {
    g_free(data);
}

static void on_channel_clicked(GtkButton *btn, gpointer data) {
    ChannelData *cd = (ChannelData *)data;
    AppWidgets *w = cd->w;

    strncpy(w->current_channel, cd->channel_name, sizeof(w->current_channel) - 1);

    char header[72];
    snprintf(header, sizeof(header), "# %s", cd->channel_name);
    gtk_label_set_text(GTK_LABEL(w->chan_name_label), header);

    char placeholder[80];
    snprintf(placeholder, sizeof(placeholder), "Écrire dans #%s...", cd->channel_name);
    gtk_entry_set_placeholder_text(GTK_ENTRY(w->input_entry), placeholder);

    GList *children = gtk_container_get_children(GTK_CONTAINER(w->messages_box));
    for (GList *l = children; l != NULL; l = l->next) {
        gtk_widget_destroy(GTK_WIDGET(l->data));
    }
    g_list_free(children);

    network_request_channel_history(cd->channel_name);
    // Pour l'instant message par défaut
    GtkWidget *msg = build_message(0, "Système", "00:00",
    "Bienvenue dans ce canal !", 'S');
    gtk_box_pack_start(GTK_BOX(w->messages_box), msg, FALSE, FALSE, 0);
    gtk_widget_show_all(w->messages_box);

    g_print("Canal changé : %s\n", cd->channel_name);
}

static GtkWidget *build_channel_button(const char *name, gboolean active, AppWidgets *w) {
    char label[72];
    snprintf(label, sizeof(label), "# %s", name);

    GtkWidget *btn = gtk_button_new_with_label(label);
    gtk_widget_set_name(btn, active ? "channel-active" : "channel");
    gtk_widget_set_halign(btn, GTK_ALIGN_FILL);

    ChannelData *cd = g_malloc(sizeof(ChannelData));
    cd->w = w;
    strncpy(cd->channel_name, name, sizeof(cd->channel_name) - 1);

    g_signal_connect_data(btn, "clicked",
        G_CALLBACK(on_channel_clicked),
        cd,
        on_channel_data_free,
        0);

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

GtkWidget *build_sidebar(AppWidgets *w) {
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(sidebar, "sidebar");
    gtk_widget_set_size_request(sidebar, 240, -1);

    GtkWidget *server_header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(server_header, "server-header");

    GtkWidget *server_name = gtk_label_new("Dev Community");
    gtk_widget_set_name(server_name, "server-name");
    gtk_widget_set_margin_start(server_name, 16);
    gtk_widget_set_margin_top(server_name, 14);
    gtk_widget_set_margin_bottom(server_name, 14);
    gtk_widget_set_halign(server_name, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(server_header), server_name, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), server_header, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(sidebar), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(sidebar), scroll, TRUE, TRUE, 0);

    GtkWidget *channels_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_margin_start(channels_box, 8);
    gtk_widget_set_margin_end(channels_box, 8);
    gtk_container_add(GTK_CONTAINER(scroll), channels_box);

    gtk_box_pack_start(GTK_BOX(channels_box), build_category("GÉNÉRAL"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("accueil", FALSE, w), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("général", TRUE, w), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("annonces", FALSE, w), FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(channels_box), build_category("DÉVELOPPEMENT"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("frontend", FALSE, w), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("backend", FALSE, w), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("code-review", FALSE, w), FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(channels_box), build_category("RESSOURCES"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("liens-utiles", FALSE, w), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(channels_box), build_channel_button("outils", FALSE, w), FALSE, FALSE, 0);

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
    GtkWidget *username = gtk_label_new(w->pseudo);
    gtk_widget_set_name(username, "username");
    gtk_widget_set_halign(username, GTK_ALIGN_START);
    GtkWidget *status = gtk_label_new("en ligne");
    gtk_widget_set_name(status, "user-status");
    gtk_widget_set_halign(status, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(user_info), username, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(user_info), status, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(user_area), user_info, TRUE, TRUE, 0);

    if (w->role == ROLE_MODERATEUR || w->role == ROLE_ADMINISTRATEUR) {
        GtkWidget *mod_admin_btn = gtk_button_new_with_label("🛡");
        gtk_widget_set_name(mod_admin_btn, "mod-admin-btn");

        w->panel_popover = gtk_popover_new(mod_admin_btn);
        GtkWidget *panel = (w->role == ROLE_ADMINISTRATEUR)
            ? build_admin_panel(w)
            : build_mod_panel(w);
        gtk_container_add(GTK_CONTAINER(w->panel_popover), panel);

        g_signal_connect(mod_admin_btn, "clicked",
            G_CALLBACK(on_mod_admin_btn_clicked), w);

        gtk_box_pack_start(GTK_BOX(user_area), mod_admin_btn, FALSE, FALSE, 0);
    }

    GtkWidget *settings_btn = gtk_button_new_with_label("⚙");
    gtk_widget_set_name(settings_btn, "settings-btn");
    g_signal_connect(settings_btn, "clicked", G_CALLBACK(on_settings_btn_clicked), w);
    gtk_box_pack_start(GTK_BOX(user_area), settings_btn, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(sidebar), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), user_area, FALSE, FALSE, 0);

    return sidebar;
}