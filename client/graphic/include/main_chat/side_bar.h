#ifndef SIDEBAR_H
#define SIDEBAR_H


#include <string.h>
#include "../menu_login/app.h"

typedef struct {
    AppWidgets *w;
    char channel_name[64];
} ChannelData;

GtkWidget *build_sidebar(AppWidgets *w);

#endif