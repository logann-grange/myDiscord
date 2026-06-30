#include "../../include/main_chat/chat.h"
#include "../../include/main_chat/side_bar.h"
#include "../../include/main_chat/messagerie.h"

static void on_chat_window_destroy(GtkWidget *widget, gpointer data) {
    ChatWidgets *w = (ChatWidgets *)data;
    g_free(w);
    gtk_main_quit();
}

void show_chat_window(GtkApplication *app) {
    ChatWidgets *w = g_malloc(sizeof(ChatWidgets));

    //TODO: récupérer le rôle et le pseudo de l'utilisateur connecté
    w->role = ROLE_ADMINISTRATEUR   ;
    strncpy(w->pseudo, "moi", sizeof(w->pseudo) - 1);

    // CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "./client/graphic/src/style/style_chat.css", NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    w->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(w->window), "Dev Community");
    gtk_window_maximize(GTK_WINDOW(w->window));

    // Layout principal horizontal
    w->main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(w->window), w->main_box);

    // Sidebar gauche
    w->sidebar = build_sidebar(w);
    gtk_box_pack_start(GTK_BOX(w->main_box), w->sidebar, FALSE, FALSE, 0);

    // Séparateur
    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX(w->main_box), separator, FALSE, FALSE, 0);

    // Zone chat
    w->chat_area = build_messagerie(w);
    gtk_box_pack_start(GTK_BOX(w->main_box), w->chat_area, TRUE, TRUE, 0);

    gtk_widget_show_all(w->window);
    g_signal_connect(w->window, "destroy", G_CALLBACK(on_chat_window_destroy), w);
}
