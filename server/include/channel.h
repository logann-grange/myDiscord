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

Channel *createChannel(int id, char* name, char *permission, char *status);

void changeChannelName(Channel *channel, char *name);

void changeChannelPermission(Channel *channel, char *permission);

void deleteChannel(Channel *channel);

void fillListUser(Channel *channel);

void fillListMessage(Channel *channel);


typedef struct {
    int id;
    char auteur[64];
    char texte[1024];
    char date[32];
} HistoryEntry;

char **listAllChannelNames(int *outCount);

void freeChannelNames(char **names, int count);

int getChannelIdByName(const char *name);

int getChannelNameByMessageId(int messageId, char *outChannelName, size_t outLen);

HistoryEntry *fetchChannelHistory(const char *channelName, int *outCount);

int createChannelInDb(const char *name);

int deleteChannelInDb(const char *name);

int setUserRoleByPseudo(const char *pseudo, const char *rankStr);