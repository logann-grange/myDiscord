#include "../include/login.h"

User login(char *email, char* password, char *ip)
{
    conn = PGconn bddConnexion();
    char **field = ["email", "password"];
    char **params = [email, password];

    int nrows, ncols; // nombre de colonnes et ligne du tableau
    char ***result = ***dbbSelect(conn, fields, params, 2, &nrows, &ncols);
    
    for (int i=0;i<=ncols;i++) {
        if (ncols[i][1] == email && ncols[i][3] == password) { // à changer suivant la composition de la bdd
            return createUser((ncols[i][0], ncols[i][2], ncols[i][1], ncols[i][3], ip, ncols[i][5], ncols[i][4]));
        }
    }

    return NULL;
}

void registerUser(char *name, char *firstName, char *pseudo, char *email, char *password)
{
    // appeler la requette
}