#include "../include/channel.h"
#include <stdlib.h>
#include <string.h>

// Minimal, safe implementations to satisfy server link requirements.
Channel *createChannel(int id, char* name, char *permission, char *status) {
    Channel *c = malloc(sizeof(Channel));
    if (!c) return NULL;
    c->id = id;
    c->name = strdup(name ? name : "");
    c->permission = strdup(permission ? permission : "");
    c->status = strdup(status ? status : "");
    c->listUserId = NULL;
    c->listMessage = NULL;
    return c;
}

void changeChannelName(Channel *channel, char *name) {
    if (!channel) return;
    free(channel->name);
    channel->name = strdup(name ? name : "");
}

void changeChannelPermission(Channel *channel, char *permission) {
    if (!channel) return;
    free(channel->permission);
    channel->permission = strdup(permission ? permission : "");
}

void deleteChannel(Channel *channel) {
    if (!channel) return;
    free(channel->name);
    free(channel->permission);
    free(channel->status);
    free(channel);
}

void fillListUser(Channel *channel) { (void)channel; }
void fillListMessage(Channel *channel) { (void)channel; }

char **listAllChannelNames(int *outCount) {
    *outCount = 1;
    char **names = malloc(sizeof(char*) * 1);
    names[0] = strdup("général");
    return names;
}

void freeChannelNames(char **names, int count) {
    for (int i = 0; i < count; i++) free(names[i]);
    free(names);
}

int getChannelIdByName(const char *name) {
    if (!name) return -1;
    if (strcmp(name, "général") == 0) return 1;
    return -1;
}

int getChannelNameByMessageId(int messageId, char *outChannelName, size_t outLen) {
    (void)messageId;
    if (outChannelName && outLen > 0) {
        strncpy(outChannelName, "général", outLen-1);
        outChannelName[outLen-1] = '\0';
        return 0;
    }
    return -1;
}

HistoryEntry *fetchChannelHistory(const char *channelName, int *outCount) {
    (void)channelName;
    *outCount = 0;
    return NULL;
}

int createChannelInDb(const char *name) { (void)name; return 1; }
int deleteChannelInDb(const char *name) { (void)name; return 1; }
int setUserRoleByPseudo(const char *pseudo, const char *rankStr) { (void)pseudo; (void)rankStr; return 1; }
