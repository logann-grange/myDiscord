#ifndef SERVER_INCLUDE_LOGIN_H
#define SERVER_INCLUDE_LOGIN_H

#include "user.h"
#include "bddManager.h"

User *login(char *email, char *password, char *ip);

/* corrige : le premier parametre doit etre une chaîne (nom) */
int registerUser(char *name, char *firstName, char *pseudo, char *email, char *password);

/* utilitaires exposés pour d'autres modules */
int getStoredPassword(const char *pseudo, char *out, size_t outLen);

#endif /* SERVER_INCLUDE_LOGIN_H */

