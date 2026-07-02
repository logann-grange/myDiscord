#include "../../include/menu_login/app.h"
#include "../../include/menu_login/login.h"
#include "../../include/menu_login/register.h"
#include "../../include/main_chat/chat_page.h"
#include "../../include/main_chat/settings.h"
#include "../../../logic/include/menu_login/network.h"

static void on_app_window_destroy(GtkWidget *widget, gpointer data) {
    AppWidgets *w = (AppWidgets *)data;
    network_disconnect();
    g_free(w);
}

void show_main_window(GtkApplication *app) {
    AppWidgets *w = g_malloc(sizeof(AppWidgets));
    w->app = app;

    if (!network_connect("10.10.6.228", 8080)) {
        GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "Impossible de se connecter au serveur.");
        gtk_dialog_run(GTK_DIALOG(err));
        gtk_widget_destroy(err);
        // On continue quand meme : les boutons login/register echoueront proprement
        // avec un message d'erreur plutot que de planter.
    }

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "./client/graphic/src/style/style.css", NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    w->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(w->window), "Dev Community");
    gtk_window_maximize(GTK_WINDOW(w->window));

    w->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(w->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(w->stack), 300);

    gtk_stack_add_named(GTK_STACK(w->stack), build_login_page(w), "login");
    gtk_stack_add_named(GTK_STACK(w->stack), build_register_page(w), "register");
    gtk_stack_add_named(GTK_STACK(w->stack), build_chat_page(w), "chat");
    gtk_stack_add_named(GTK_STACK(w->stack), build_settings_page(w), "settings");
    gtk_stack_set_visible_child_name(GTK_STACK(w->stack), "login");

    gtk_container_add(GTK_CONTAINER(w->window), w->stack);
    g_signal_connect(w->window, "destroy", G_CALLBACK(on_app_window_destroy), w);
    gtk_widget_show_all(w->window);
}