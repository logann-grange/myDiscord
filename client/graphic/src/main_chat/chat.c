#include "../../include/main_chat/chat.h"
#include "../../include/main_chat/side_bar.h"
#include "../../include/main_chat/messagerie.h"

void show_chat_window(GtkApplication *app) {
    ChatWidgets *w = g_malloc(sizeof(ChatWidgets));

    // CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "./client/graphic/src/main_chat/style_chat.css", NULL);
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
}