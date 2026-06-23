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
}

void changeChannelPermission(Channel *channel, char *permission)
{
    channel->permission = permission;
}

void switchStatus(Channel *channel)
{
    channel->status = (channel->status == 0) ? 1 : 0;
}
