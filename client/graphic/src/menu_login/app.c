#include "../../include/menu_login/app.h"
#include "../../include/menu_login/login.h"
#include "../../include/menu_login/register.h"

void show_main_window(GtkApplication *app) {
    AppWidgets *w = g_malloc(sizeof(AppWidgets));

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "./client/graphic/src/style.css", NULL);
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
    gtk_stack_set_visible_child_name(GTK_STACK(w->stack), "login");

    gtk_container_add(GTK_CONTAINER(w->window), w->stack);
    gtk_widget_show_all(w->window);
}