#include "../include/channel.h"

Channel *createChannel(int id, char* name, char *permission, int status)
{
    Channel *channel = malloc(sizeof(Channel));
    channel->id = id;
    channel->name = name;
    channel->permission = permission;
    channel->status = status;

    return channel;
}

void changeChannelName(Channel *channel, char *name)
{
    channel->name = name;

    char *fields[] = {"name"};
    char *params[] = {channel->name};
    bddUpdate("channel", fields, params, 1, channel->id);
}

void changeChannelPermission(Channel *channel, char *permission)
{
    channel->permission = permission;

    char *fields[] = {"permission"};
    char *params[] = {channel->permission};
    bddUpdate("channel", fields, params, 1, channel->id);
}