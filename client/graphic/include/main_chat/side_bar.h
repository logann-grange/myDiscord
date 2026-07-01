#ifndef SIDEBAR_H
#define SIDEBAR_H


#include <string.h>
#include "../menu_login/app.h"

typedef struct {
    AppWidgets *w;
    char channel_name[64];
    GtkWidget *btn;
} ChannelData;

GtkWidget *build_sidebar(AppWidgets *w);
void add_channel_to_sidebar(AppWidgets *w, const char *category, const char *channel_name);
void clear_channels(AppWidgets *w);

#endif