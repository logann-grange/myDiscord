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

    w->active_channel_btn = NULL;  
    global_app_widgets = w; 

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
    network_set_channel_callback(on_channel_received);
    network_request_channels();  // demande la liste des canaux au serveur
    network_set_channel_callback(on_channel_received);
    network_start_listening(on_network_message_received);
}

static void on_channel_received(const char *category, const char *channel_name) {
    if (!global_app_widgets) return;

    // Repasse dans le thread GTK
    typedef struct { AppWidgets *w; char category[64]; char channel_name[64]; } ChannelData2;
    ChannelData2 *cd = g_malloc(sizeof(ChannelData2));
    cd->w = global_app_widgets;
    strncpy(cd->category, category, sizeof(cd->category) - 1);
    strncpy(cd->channel_name, channel_name, sizeof(cd->channel_name) - 1);

    g_idle_add((GSourceFunc)add_channel_idle, cd);
}

static gboolean add_channel_idle(gpointer data) {
    typedef struct { AppWidgets *w; char category[64]; char channel_name[64]; } ChannelData2;
    ChannelData2 *cd = (ChannelData2 *)data;
    add_channel_to_sidebar(cd->w, cd->category, cd->channel_name);
    g_free(cd);
    return FALSE;
}