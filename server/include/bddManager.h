#include <libpq-fe.h>
#include <string.h>
#include <stdlib.h>

PGconn bddConnexion();

char *bddSelect(char **fields, char **params, int size);

PGresult *dbbSelect(PGconn *conn, char **fields, char **params, int size);