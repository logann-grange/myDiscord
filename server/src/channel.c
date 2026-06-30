#include "../include/channel.h"

Channel *createChannel(int id, char* name, char *permission, char *status)
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

void deleteChannel(Channel *channel)
{
    channel->status = "supprimé";

    char *fields[] = {"status"};
    char *params[] = {channel->status};
    bddUpdate("channel", fields, params, 1, channel->id);
}

// charge la liste d'utilisateur lié au channel depuis la bdd
void fillListUser(Channel *channel)
{
    if (channel == NULL || channel->permission == NULL)
        return;

    char *fields[1];
    char *params[1];

    fields[0] = "rank";
    params[0] = channel->permission;

    int nrows = 0, ncols = 0;

    char ***result = bddSelect("user", fields, params, 1, &nrows, &ncols);

    if (result == NULL)
    {
        channel->listUserId = malloc(sizeof(int));
        if (channel->listUserId != NULL)
            channel->listUserId[0] = -1;
        return;
    }

    int *ids = malloc((nrows + 1) * sizeof(int));
    if (ids == NULL)
    {
        for (int i = 0; i < nrows; i++)
        {
            for (int j = 0; j < ncols; j++)
                free(result[i][j]);
            free(result[i]);
        }
        free(result);
        return;
    }

    int idCol = 0;

    for (int i = 0; i < nrows; i++)
        ids[i] = result[i][idCol] ? atoi(result[i][idCol]) : -1;
    ids[nrows] = -1; // sentinelle de fin de liste

    channel->listUserId = ids;

    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < ncols; j++)
            free(result[i][j]);
        free(result[i]);
    }
    free(result);
}

void fillListMessage(Channel *channel)
{
    if (channel == NULL)
        return;

    char *fields[1];
    char *params[1];
    char idStr[12];

    snprintf(idStr, sizeof(idStr), "%d", channel->id);

    fields[0] = "id_channel";
    params[0] = idStr;

    int nrows = 0, ncols = 0;

    char ***result = bddSelect("message", fields, params, 1, &nrows, &ncols);

    if (result == NULL)
    {
        channel->listMessage = malloc(sizeof(Message *));
        if (channel->listMessage != NULL)
            channel->listMessage[0] = NULL;
        return;
    }

    Message **messages = malloc((nrows + 1) * sizeof(Message *));
    if (messages == NULL)
    {
        for (int i = 0; i < nrows; i++)
        {
            for (int j = 0; j < ncols; j++)
                free(result[i][j]);
            free(result[i]);
        }
        free(result);
        return;
    }

    // 0: id | 1: id_user | 2: id_channel | 3: text | 4: date | 5: reaction | 6: status
    const int COL_ID = 0;
    const int COL_ID_USER = 1;
    const int COL_ID_CHANNEL = 2;
    const int COL_TEXT = 3;
    const int COL_DATE = 4;
    const int COL_REACTION = 5;
    const int COL_STATUS = 6;

    for (int i = 0; i < nrows; i++)
    {
        Message *msg = malloc(sizeof(Message));
        if (msg == NULL)
        {
            for (int k = 0; k < i; k++)
            {
                free(messages[k]->text);
                free(messages[k]->reaction);
                free(messages[k]->status);
                free(messages[k]);
            }
            free(messages);

            for (int r = 0; r < nrows; r++)
            {
                for (int j = 0; j < ncols; j++)
                    free(result[r][j]);
                free(result[r]);
            }
            free(result);
            return;
        }

        msg->id = result[i][COL_ID] ? atoi(result[i][COL_ID]) : -1;
        msg->idUser = result[i][COL_ID_USER] ? atoi(result[i][COL_ID_USER]) : -1;
        msg->idChannel = result[i][COL_ID_CHANNEL] ? atoi(result[i][COL_ID_CHANNEL]) : -1;

        msg->text = result[i][COL_TEXT] ? strdup(result[i][COL_TEXT]) : NULL;
        msg->reaction = result[i][COL_REACTION] ? strdup(result[i][COL_REACTION]) : NULL;
        msg->status = result[i][COL_STATUS] ? strdup(result[i][COL_STATUS]) : NULL;

        msg->date = result[i][COL_DATE] ? (time_t)atol(result[i][COL_DATE]) : 0;

        messages[i] = msg;
    }
    messages[nrows] = NULL; // sentinelle de fin de liste

    channel->listMessage = messages;

    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < ncols; j++)
            free(result[i][j]);
        free(result[i]);
    }
    free(result);
}