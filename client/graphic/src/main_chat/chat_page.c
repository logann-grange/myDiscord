#include "../../include/main_chat/chat_page.h"
#include "../../include/main_chat/side_bar.h"
#include "../../include/main_chat/messagerie.h"
#include <string.h>

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