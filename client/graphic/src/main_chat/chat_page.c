#include "../../include/main_chat/chat_page.h"
#include "../../include/main_chat/side_bar.h"
#include "../../include/main_chat/messagerie.h"
#include <string.h>


static AppWidgets *global_app_widgets = NULL;

static void on_network_message_received(int message_id, const char *auteur, const char *channel,
                                          const char *texte, const char *date) {
    if (!global_app_widgets) return;

    IncomingMessageData *msg = g_malloc(sizeof(IncomingMessageData));
    msg->w = global_app_widgets;
    msg->message_id = message_id;
    strncpy(msg->auteur, auteur, sizeof(msg->auteur) - 1);
    strncpy(msg->channel, channel, sizeof(msg->channel) - 1);
    strncpy(msg->texte, texte, sizeof(msg->texte) - 1);
    strncpy(msg->date, date, sizeof(msg->date) - 1);

    g_idle_add(display_incoming_message, msg);
}

GtkWidget *build_chat_page(AppWidgets *w) {
    // Rôle de test (à remplacer plus tard par les données du serveur)
    w->role = ROLE_ADMINISTRATEUR;
    strncpy(w->pseudo, "moi", sizeof(w->pseudo) - 1);

    w->main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    w->sidebar = build_sidebar(w);
    gtk_box_pack_start(GTK_BOX(w->main_box), w->sidebar, FALSE, FALSE, 0);

    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX(w->main_box), separator, FALSE, FALSE, 0);

    w->chat_area = build_messagerie(w);
    gtk_box_pack_start(GTK_BOX(w->main_box), w->chat_area, TRUE, TRUE, 0);

    return w->main_box;
}

void chat_start_network_listening(AppWidgets *w) {
    global_app_widgets = w;
    network_start_listening(on_network_message_received);
}