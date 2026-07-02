#include "../include/user.h"

User *createUser(int id, char *name, char *firstName, char *pseudo, char *email,
                  char *password, char *ip, char *rank, char *status)
{
    User *user = malloc(sizeof(User));
    if (!user) return NULL;

    user->id = id;
    user->name      = name      ? strdup(name)      : NULL;
    user->firstName = firstName ? strdup(firstName) : NULL;
    user->pseudo    = pseudo    ? strdup(pseudo)    : NULL;
    user->email     = email     ? strdup(email)     : NULL;
    user->password  = password  ? strdup(password)  : NULL;
    user->ip        = ip        ? strdup(ip)        : NULL;
    user->rank      = rank      ? strdup(rank)      : NULL;
    user->status    = status    ? strdup(status)    : NULL;

    return user;
}

void freeUser(User *user) {
    if (!user) return;
    free(user->name);
    free(user->firstName);
    free(user->pseudo);
    free(user->email);
    free(user->password);
    free(user->ip);
    free(user->rank);
    free(user->status);
    free(user);
}

void changeUserRank(User *user, char *rank) {
    free(user->rank);
    user->rank = strdup(rank);
    char *fields[] = {"rank"};
    char *params[] = {user->rank};
    bddUpdate("user", fields, params, 1, user->id);
}

void changeUserStatus(User *user, char *status) {
    free(user->status);
    user->status = strdup(status);
    char *fields[] = {"status"};
    char *params[] = {user->status};
    bddUpdate("user", fields, params, 1, user->id);
}

void changePseudo(User *user, char *pseudo) {
    free(user->pseudo);
    user->pseudo = strdup(pseudo);
    char *fields[] = {"pseudo"};
    char *params[] = {user->pseudo};
    bddUpdate("user", fields, params, 1, user->id);
}