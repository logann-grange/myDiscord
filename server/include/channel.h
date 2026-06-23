#include <stdio.h>
#include <stdlib.h>
#include "user.h"
#include "message.h"

typedef struct {
    int id;
    char *name;
    char *permission;
    int *listUserId;
    Message **listMessage;
    int status; // notification
} Channel;

Channel *createChannel(int id, char* name, char *permission, int status, User **ListUser);

void changeChannelName(Channel *channel, char *name);

void changeChannelPermission(Channel *channel, char *permission);

void switchStatus(Channel *channel);