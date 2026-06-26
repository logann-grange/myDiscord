#ifndef SIDEBAR_H
#define SIDEBAR_H


#include <string.h>
#include "chat.h"

typedef struct {
    ChatWidgets *w;
    char channel_name[64];
} ChannelData;

GtkWidget *build_sidebar(ChatWidgets *w);
#endif