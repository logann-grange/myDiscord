#include <login.h> 

void show_login_window(GtkApplication *app) {

    GtkWidget *pWindow;
    gtk_init(NULL, NULL);

    pWindow = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(pWindow), "Login");
    gtk_window_show(pWindow);


}