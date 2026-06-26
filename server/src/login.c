#include "../include/login.h"

User *login(char *email, char *password, char *ip)
{
    PGconn *conn = bddConnexion();
    char *fields[] = {"email", "password"};
    char *params[] = {email, password};

    int nrows, ncols;
    char ***result = bddSelect(conn, fields, params, 2, &nrows, &ncols);

    if (result == NULL) {
        return NULL;
    }

    User *user = NULL;
    for (int i = 0; i < nrows; i++) {
        if (strcmp(result[i][1], email) == 0 && strcmp(result[i][5], password) == 0) {
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

int registerUser(char name, char *firstName, char *pseudo, char *email, char *password)
{
    char *fields[] = {"name", "firstName", "pseudo", "email", "password", "status", "rank"};
    char *params[] = {name, firstName, pseudo, email, password, "", "member" };
    PGconn *conn = bddConnexion();
    return bddInsert(conn, "user", fields, params, 7);
}