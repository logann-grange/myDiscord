#include <gtk/gtk.h>
#include "../../include/menu_login/app.h"

static void on_activate(GtkApplication *app, gpointer data) {
    show_main_window(app);
}

int main(int argc, char *argv[]) {
    GtkApplication *application = gtk_application_new("com.devcommunity.app", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(application, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return status;
}