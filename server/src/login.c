#include "../include/login.h"
#include <string.h>
#include <stdlib.h>
#include "../../utils/include/protocol.h"

#ifndef SALT_LEN
#define SALT_LEN 16
#endif

int getStoredPassword(const char *pseudo, char *out, size_t outLen) {
    char *fields[] = {"pseudo"};
    char *params[] = {(char*)pseudo};
    int nrows, ncols;
    char ***result = bddSelect("user", fields, params, 1, &nrows, &ncols);
    if (result == NULL) return -1;

    int found = -1;
    for (int i = 0; i < nrows; i++) {
        if (result[i][4] && strcmp(result[i][4], pseudo) == 0) {
            strncpy(out, result[i][5] ? result[i][5] : "", outLen - 1);
            out[outLen - 1] = '\0';
            found = 0;
            break;
        }
    }
    for (int i = 0; i < nrows; i++) { for (int j = 0; j < ncols; j++) free(result[i][j]); free(result[i]); }
    free(result);
    return found;
}

User *login(char *pseudo, char *hash, char *ip)
{
    char stored[256];
    if (getStoredPassword(pseudo, stored, sizeof(stored)) != 0) return NULL;
    if (strlen(stored) <= (size_t)(SALT_LEN * 2)) return NULL;

    const char *storedHash = stored + (SALT_LEN * 2);
    if (strcmp(storedHash, hash) != 0) return NULL;

    char *fields[] = {"pseudo"};
    char *params[] = {pseudo};
    int nrows, ncols;
    char ***result = bddSelect("user", fields, params, 1, &nrows, &ncols);
    if (result == NULL) return NULL;

    User *user = NULL;
    for (int i = 0; i < nrows; i++) {
        if (result[i][4] && strcmp(result[i][4], pseudo) == 0) {
            user = createUser(
                atoi(result[i][0]), result[i][2], result[i][3], result[i][4],
                result[i][1], result[i][5], ip, result[i][6], result[i][7]
            );
            break;
        }
    }
    for (int i = 0; i < nrows; i++) { for (int j = 0; j < ncols; j++) free(result[i][j]); free(result[i]); }
    free(result);
    return user;
}

static int pseudoExists(const char *pseudo) {
    char *fields[] = {"pseudo"};
    char *params[] = {(char*)pseudo};
    int nrows, ncols;
    char ***result = bddSelect("user", fields, params, 1, &nrows, &ncols);
    int exists = (result != NULL && nrows > 0);
    if (result) { for (int i = 0; i < nrows; i++) { for (int j = 0; j < ncols; j++) free(result[i][j]); free(result[i]); } free(result); }
    return exists;
}

static int emailExists(const char *email) {
    char *fields[] = {"email"};
    char *params[] = {(char*)email};
    int nrows, ncols;
    char ***result = bddSelect("user", fields, params, 1, &nrows, &ncols);
    int exists = (result != NULL && nrows > 0);
    if (result) { for (int i = 0; i < nrows; i++) { for (int j = 0; j < ncols; j++) free(result[i][j]); free(result[i]); } free(result); }
    return exists;
}

int registerUser(char *name, char *firstName, char *pseudo, char *email, char *password)
{
    if (pseudoExists(pseudo)) return -1;
    if (emailExists(email))   return -1;

    char *fields[] = {"name", "first_name", "pseudo", "email", "password", "status", "rank"};
    char *params[] = {name, firstName, pseudo, email, password, "actif", "member"};
    // bddInsert renvoie maintenant l'id ou -1 : on garde ici une interface booléenne
    return bddInsert("user", fields, params, 7) >= 0;
}