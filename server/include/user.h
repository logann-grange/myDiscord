#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char *name;
    char *password;
    char *email;
    char *rank;
    char *status;
} User;

User *createUser(int id, char *name, char *rank, char *status);

User **createUserList(); // prendre les valeur de la bdd

void changeUserRank(User *user, char *rank);

void changeUserStatus(User *user, char *status);

void changeUserName(User *user, char *name);