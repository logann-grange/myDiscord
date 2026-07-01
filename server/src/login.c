#include "../include/login.h"
#include <string.h>
#include <stdlib.h>

// Doit correspondre a la taille du sel utilisee cote client (register_logic.h)
#ifndef SALT_LEN
#define SALT_LEN 16
#endif

// ---------------------------------------------------------------------
// Recupere le mot de passe securise stocke (salt_hex + hash_hex concatenes)
// pour un pseudo donne. Retourne 0 et remplit `out` si trouve, -1 sinon.
// ---------------------------------------------------------------------
int getStoredPassword(const char *pseudo, char *out, size_t outLen) {
    PGconn *conn = bddConnexion();
    char *fields[] = {"pseudo"};
    char *params[] = {(char*)pseudo};

    int nrows, ncols;
    char ***result = bddSelect(conn, fields, params, 1, &nrows, &ncols);
    if (result == NULL) return -1;

    for (int i = 0; i < nrows; i++) {
        if (strcmp(result[i][4], pseudo) == 0) { // colonne pseudo = index 4
            strncpy(out, result[i][5], outLen - 1); // colonne password = index 5
            out[outLen - 1] = '\0';
            return 0;
        }
    }
    return -1;
}

// ---------------------------------------------------------------------
// Authentifie un utilisateur via son pseudo + le hash calcule cote client
// (avec le sel recupere au prealable via getStoredPassword/MSG_LOGIN_SALT_REQUEST)
// ---------------------------------------------------------------------
User *login(char *pseudo, char *hash, char *ip)
{
    char stored[256];
    if (getStoredPassword(pseudo, stored, sizeof(stored)) != 0) {
        return NULL; // pseudo inconnu
    }

    if (strlen(stored) <= (size_t)(SALT_LEN * 2)) {
        return NULL; // donnee corrompue en base
    }

    const char *storedHash = stored + (SALT_LEN * 2); // on saute le sel
    if (strcmp(storedHash, hash) != 0) {
        return NULL; // mot de passe incorrect
    }

    // Hash valide : on recupere les infos completes de l'utilisateur
    PGconn *conn = bddConnexion();
    char *fields[] = {"pseudo"};
    char *params[] = {pseudo};
    int nrows, ncols;
    char ***result = bddSelect(conn, fields, params, 1, &nrows, &ncols);
    if (result == NULL) return NULL;

    User *user = NULL;
    for (int i = 0; i < nrows; i++) {
        if (strcmp(result[i][4], pseudo) == 0) {
            user = createUser(
                atoi(result[i][0]),
                result[i][2],
                result[i][3],
                result[i][4],
                result[i][1],
                result[i][5],
                ip,
                result[i][7],
                result[i][6]
            );
            break;
        }
    }

    return user;
}

// ---------------------------------------------------------------------
// Verifie si un pseudo ou un email est deja utilise
// ---------------------------------------------------------------------
static int pseudoExists(const char *pseudo) {
    PGconn *conn = bddConnexion();
    char *fields[] = {"pseudo"};
    char *params[] = {(char*)pseudo};
    int nrows, ncols;
    char ***result = bddSelect(conn, fields, params, 1, &nrows, &ncols);
    return (result != NULL && nrows > 0);
}

static int emailExists(const char *email) {
    PGconn *conn = bddConnexion();
    char *fields[] = {"email"};
    char *params[] = {(char*)email};
    int nrows, ncols;
    char ***result = bddSelect(conn, fields, params, 1, &nrows, &ncols);
    return (result != NULL && nrows > 0);
}

// ---------------------------------------------------------------------
// Enregistre un nouvel utilisateur. `password` doit deja contenir
// salt_hex + hash_hex concatenes (calcule cote serveur, voir server.c).
// Retourne 0 en cas de succes, -1 si pseudo/email deja pris ou erreur BDD.
// ---------------------------------------------------------------------
int registerUser(char name, char *firstName, char *pseudo, char *email, char *password)
{
    if (pseudoExists(pseudo)) return -1;
    if (emailExists(email))   return -1;

    char *fields[] = {"name", "firstName", "pseudo", "email", "password", "status", "rank"};
    char *params[] = {&name, firstName, pseudo, email, password, "", "member"};
    PGconn *conn = bddConnexion();
    return bddInsert("user", fields, params, 7);
}