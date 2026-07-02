#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bddManager.h"

typedef struct {
    int id;
    char *name;
    char *firstName;
    char *pseudo;
    char *password;
    char *email;
    char *ip;
    char *rank;
    char *status;
} User;

User *createUser(int id, char *name, char *firstName, char *pseudo, char *email, char *password, char *ip, char *rank, char *status);

User **createUserList(); // prendre les valeur de la bdd

void changeUserRank(User *user, char *rank);

void changeUserStatus(User *user, char *status);

void changePseudo(User *user, char *pseudo);

void freeUser(User *user);