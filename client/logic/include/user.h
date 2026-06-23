#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char *name;
    char *rank;
    char *status;
} User;

User *createUser(int id, char *name, char *rank, char *status);

void changeUserRank(User *user, char *rank);

void changeUserStatus(User *user, char *status);

void changeUserName(User *user, char *name);
