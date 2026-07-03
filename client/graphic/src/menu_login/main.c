#include "../../include/menu_login/app.h"
#include "../../../logic/include/menu_login/network.h"

static void on_activate(GtkApplication *app, gpointer data) {
    printf("Tentative de connexion au serveur...\n");
    fflush(stdout);
    if (!network_connect("127.0.0.1", 8080)) {
        printf("ECHEC connexion serveur!\n");
        fflush(stdout);
    } else {
        printf("Connecte au serveur!\n");
        fflush(stdout);
    }
    show_main_window(app);
}

int main(int argc, char *argv[]) {
    GtkApplication *application = gtk_application_new("com.devcommunity.app", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(application, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    network_disconnect();
    return status;
}