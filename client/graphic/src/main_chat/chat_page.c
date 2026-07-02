#include "../../include/main_chat/chat_page.h"
#include "../../include/main_chat/side_bar.h"
#include "../../include/main_chat/messagerie.h"
#include <string.h>


static AppWidgets *global_app_widgets = NULL;


static gboolean add_channel_idle(gpointer data) {
    typedef struct { AppWidgets *w; char category[64]; char channel_name[64]; } ChannelData2;
    ChannelData2 *cd = (ChannelData2 *)data;
    add_channel_to_sidebar(cd->w, cd->category, cd->channel_name);
    g_free(cd);
    return FALSE;
}



static void on_channel_received(const char *category, const char *channel_name) {
    if (!global_app_widgets) return;

    typedef struct { AppWidgets *w; char category[64]; char channel_name[64]; } ChannelData2;
    ChannelData2 *cd = g_malloc(sizeof(ChannelData2));
    cd->w = global_app_widgets;
    strncpy(cd->category, category, sizeof(cd->category) - 1);
    strncpy(cd->channel_name, channel_name, sizeof(cd->channel_name) - 1);

    g_idle_add((GSourceFunc)add_channel_idle, cd);
}

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

typedef struct { AppWidgets *w; char channel[64]; int message_id; } DeletedMsgData;

static gboolean apply_message_deleted_idle(gpointer data) {
    DeletedMsgData *d = (DeletedMsgData *)data;
    apply_message_deleted(d->w, d->channel, d->message_id);
    g_free(d);
    return FALSE;
}

static void on_message_deleted_received(const char *channel, int message_id) {
    if (!global_app_widgets) return;

    DeletedMsgData *d = g_malloc(sizeof(DeletedMsgData));
    d->w = global_app_widgets;
    strncpy(d->channel, channel, sizeof(d->channel) - 1);
    d->channel[sizeof(d->channel) - 1] = '\0';
    d->message_id = message_id;

    g_idle_add(apply_message_deleted_idle, d);
}

GtkWidget *build_chat_page(AppWidgets *w) {
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

static void on_mod_response(gboolean success, const char *message) {
    if (!global_app_widgets) return;
    if (success) return;

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(global_app_widgets->window),
        GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
}

void chat_start_network_listening(AppWidgets *w) {
    global_app_widgets = w;
    network_set_channel_callback(on_channel_received);
    network_set_mod_callback((ModResponseCallback)on_mod_response);
    network_set_delete_callback(on_message_deleted_received);
    network_start_listening(on_network_message_received);
    network_request_channels();
}