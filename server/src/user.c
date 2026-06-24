#include "../include/user.h"

User *createUser(int id, char *name, char *email, char *password, char *ip, char *rank, char *status) 
{
    User *user = malloc(sizeof(User));
    user->id = id;
    user->name = *name;
    user->email = *email;
    user->password = *password;
    user->ip = *ip;
    user->rank = *rank;
    user->status = *status;

    return user;
}

void changeUserRank(User *user, char *rank)
{
    user->rank = rank;
    //ajouter la fonction de la requette sql
}

void changeUserStatus(User *user, char *status)
{
    user->status = status;
}

void changeUserName(User *user, char *name)
{
    user->name = name;
}