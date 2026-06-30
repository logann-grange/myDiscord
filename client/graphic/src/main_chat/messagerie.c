#include "../../include/main_chat/messagerie.h"
#include <string.h>
#include <stdio.h>

// Réactions disponibles
static const char *reactions[] = {"👍", "❤️", "😂", "😮", "😢", "🔥"};
static const int nb_reactions = 6;

typedef struct {
    GtkWidget *reactions_box;
    const char *emoji;
} ReactionData;

static void on_reaction_data_free(gpointer data, GClosure *closure) {
    g_free(data);
}

static void on_reaction_clicked(GtkButton *btn, gpointer data) {
    ReactionData *rd = (ReactionData *)data;

    GtkWidget *reaction_btn = gtk_button_new_with_label("");
    char label[32];
    snprintf(label, sizeof(label), "%s 1", rd->emoji);
    gtk_button_set_label(GTK_BUTTON(reaction_btn), label);
    gtk_widget_set_name(reaction_btn, "reaction-count-btn");
    gtk_box_pack_start(GTK_BOX(rd->reactions_box), reaction_btn, FALSE, FALSE, 0);
    gtk_widget_show(reaction_btn);

    printf("Réaction ajoutée : %s\n", rd->emoji);
    // TODO: envoyer la réaction au serveur
}

static void on_send_clicked(GtkButton *btn, gpointer data) {
    GtkEntry *entry = GTK_ENTRY(data);
    const char *text = gtk_entry_get_text(entry);
    if (strlen(text) == 0) return;
    printf("Message envoyé : %s\n", text);
    gtk_entry_set_text(entry, "");
    // TODO: envoyer au serveur
}

static void on_entry_activate(GtkEntry *entry, gpointer data) {
    on_send_clicked(NULL, GTK_BUTTON(data));
}

static void on_react_btn_clicked(GtkButton *btn, gpointer data) {
    GtkWidget *popover = GTK_WIDGET(data);
    gtk_widget_show_all(popover);
}

GtkWidget *build_message(const char *auteur, const char *heure, const char *texte, const char avatar_lettre) {
    GtkWidget *msg_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_name(msg_box, "message-box");
    gtk_widget_set_margin_top(msg_box, 6);
    gtk_widget_set_margin_bottom(msg_box, 6);
    gtk_widget_set_margin_start(msg_box, 16);
    gtk_widget_set_margin_end(msg_box, 16);

    char avatar_str[2] = {avatar_lettre, '\0'};
    GtkWidget *avatar = gtk_label_new(avatar_str);
    gtk_widget_set_name(avatar, "avatar");
    gtk_widget_set_valign(avatar, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(msg_box), avatar, FALSE, FALSE, 0);

    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_box_pack_start(GTK_BOX(msg_box), content, TRUE, TRUE, 0);

    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *lbl_auteur = gtk_label_new(auteur);
    gtk_widget_set_name(lbl_auteur, "msg-author");
    GtkWidget *lbl_heure = gtk_label_new(heure);
    gtk_widget_set_name(lbl_heure, "msg-time");
    gtk_box_pack_start(GTK_BOX(header), lbl_auteur, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header), lbl_heure, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content), header, FALSE, FALSE, 0);

    GtkWidget *lbl_texte = gtk_label_new(texte);
    gtk_widget_set_name(lbl_texte, "msg-text");
    gtk_widget_set_halign(lbl_texte, GTK_ALIGN_START);
    gtk_label_set_line_wrap(GTK_LABEL(lbl_texte), TRUE);
    gtk_label_set_xalign(GTK_LABEL(lbl_texte), 0.0);
    gtk_box_pack_start(GTK_BOX(content), lbl_texte, FALSE, FALSE, 0);

    GtkWidget *reactions_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_margin_top(reactions_box, 4);
    gtk_box_pack_start(GTK_BOX(content), reactions_box, FALSE, FALSE, 0);

    GtkWidget *react_btn = gtk_button_new_with_label("😊+");
    gtk_widget_set_name(react_btn, "react-add-btn");

    GtkWidget *popover = gtk_popover_new(react_btn);
    GtkWidget *popover_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_margin_top(popover_box, 6);
    gtk_widget_set_margin_bottom(popover_box, 6);
    gtk_widget_set_margin_start(popover_box, 6);
    gtk_widget_set_margin_end(popover_box, 6);

    for (int i = 0; i < nb_reactions; i++) {
        GtkWidget *emoji_btn = gtk_button_new_with_label(reactions[i]);
        gtk_widget_set_name(emoji_btn, "emoji-btn");

        ReactionData *rd = g_malloc(sizeof(ReactionData));
        rd->emoji = reactions[i];
        rd->reactions_box = reactions_box;

        g_signal_connect_data(emoji_btn, "clicked",
            G_CALLBACK(on_reaction_clicked),
            rd,
            on_reaction_data_free,
            0);

        gtk_box_pack_start(GTK_BOX(popover_box), emoji_btn, FALSE, FALSE, 0);
    }

    gtk_container_add(GTK_CONTAINER(popover), popover_box);
    gtk_widget_show_all(popover_box);
    g_signal_connect(react_btn, "clicked", G_CALLBACK(on_react_btn_clicked), popover);
    gtk_box_pack_start(GTK_BOX(reactions_box), react_btn, FALSE, FALSE, 0);

    return msg_box;
}

GtkWidget *build_messagerie(AppWidgets *w) {
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(main_box, "chat-main");

    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_name(header, "chan-header");
    gtk_widget_set_margin_start(header, 16);
    gtk_widget_set_margin_end(header, 16);
    gtk_widget_set_margin_top(header, 12);
    gtk_widget_set_margin_bottom(header, 12);

    w->chan_name_label = gtk_label_new("# général");
    gtk_widget_set_name(w->chan_name_label, "chan-name");
    gtk_box_pack_start(GTK_BOX(header), w->chan_name_label, FALSE, FALSE, 0);

    GtkWidget *sep = gtk_label_new("—");
    gtk_widget_set_name(sep, "chan-sep");
    gtk_box_pack_start(GTK_BOX(header), sep, FALSE, FALSE, 0);

    GtkWidget *chan_desc = gtk_label_new("canal de discussion");
    gtk_widget_set_name(chan_desc, "chan-desc");
    gtk_box_pack_start(GTK_BOX(header), chan_desc, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), scroll, TRUE, TRUE, 0);

    w->messages_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(w->messages_box, "messages-box");
    gtk_container_add(GTK_CONTAINER(scroll), w->messages_box);

    // Messages de test
    gtk_box_pack_start(GTK_BOX(w->messages_box),
        build_message("clara_dev", "19:31", "Salut tout le monde !", 'C'),
        FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(w->messages_box),
        build_message("theo.js", "19:32", "Ouais, super conférence !", 'T'),
        FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(w->messages_box),
        build_message("naomi_ux", "20:01", "Y'a un replay quelque part ?", 'N'),
        FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(main_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);

    GtkWidget *input_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_name(input_area, "input-area");
    gtk_widget_set_margin_start(input_area, 16);
    gtk_widget_set_margin_end(input_area, 16);
    gtk_widget_set_margin_top(input_area, 8);
    gtk_widget_set_margin_bottom(input_area, 8);

    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_name(input_box, "input-box");

    GtkWidget *btn_plus = gtk_button_new_with_label("+");
    gtk_widget_set_name(btn_plus, "input-plus");
    gtk_box_pack_start(GTK_BOX(input_box), btn_plus, FALSE, FALSE, 0);

    w->input_entry = gtk_entry_new();
    gtk_widget_set_name(w->input_entry, "input-entry");
    gtk_entry_set_placeholder_text(GTK_ENTRY(w->input_entry), "Écrire dans #général...");
    gtk_box_pack_start(GTK_BOX(input_box), w->input_entry, TRUE, TRUE, 0);

    GtkWidget *btn_send = gtk_button_new_with_label("➤");
    gtk_widget_set_name(btn_send, "input-send");
    g_signal_connect(btn_send, "clicked", G_CALLBACK(on_send_clicked), w->input_entry);
    g_signal_connect(w->input_entry, "activate", G_CALLBACK(on_entry_activate), btn_send);
    gtk_box_pack_start(GTK_BOX(input_box), btn_send, FALSE, FALSE, 0);

    GtkWidget *btn_emoji = gtk_button_new_with_label("😊");
    gtk_widget_set_name(btn_emoji, "input-emoji");
    gtk_box_pack_start(GTK_BOX(input_box), btn_emoji, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(input_area), input_box, FALSE, FALSE, 0);

    GtkWidget *hint = gtk_label_new("↵ Entrée pour envoyer · Maj+Entrée pour un saut de ligne");
    gtk_widget_set_name(hint, "input-hint");
    gtk_widget_set_halign(hint, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(input_area), hint, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(main_box), input_area, FALSE, FALSE, 0);

    strncpy(w->current_channel, "général", sizeof(w->current_channel) - 1);

    return main_box;
}