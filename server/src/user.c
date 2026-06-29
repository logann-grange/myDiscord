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
    
    char *fields[] = {"rank"};
    char *params[] = {user->rank};
    bddUpdate("user", fields, params, 1, user->id);
}

void changeUserStatus(User *user, char *status)
{
    user->status = status;

    char *fields[] = {"status"};
    char *params[] = {user->status};
    bddUpdate("user", fields, params, 1, user->id);
}

void changePseudo(User *user, char *pseudo)
{
    user->pseudo = pseudo;
    char *fields[] = {"pseudo"};
    char *params[] = {user->pseudo};
    bddUpdate("user", fields, params, 1, user->id);
}