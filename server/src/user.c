#include "../include/user.h"

User *createUser(int id, char *name, char *firstName, char *pseudo, char *email, char *password, char *ip, char *rank, char *status) 
{
    User *user = malloc(sizeof(User));
    user->id = id;
    user->name = *name;
    user->firstName = firstName;
    user->pseudo = pseudo;
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

void changePseudo(User *user, char *pseudo)
{
    user->pseudo = pseudo;
}