#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "user.h"
#include "message.h"

typedef struct {
    int id;
    char *name;
    char *permission;
    int *listUserId;
    Message **listMessage;
    char *status;
} Channel;

Channel *createChannel(int id, char* name, char *permission, char *status, User **ListUser);

void changeChannelName(Channel *channel, char *name);

void changeChannelPermission(Channel *channel, char *permission);

void deleteChannel(Channel *channel);

void fillListUser(Channel *channel);

void fillListMessage(Channel *channel);