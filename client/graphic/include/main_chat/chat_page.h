#ifndef CHAT_PAGE_H
#define CHAT_PAGE_H

#include "../menu_login/app.h"
#include "../../logic/include/network/network_client.h"
#include "side_bar.h"

GtkWidget *build_chat_page(AppWidgets *w);
void chat_start_network_listening(AppWidgets *w);

#endif