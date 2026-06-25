#include <libpq-fe.h>
#include <string.h>
#include <stdlib.h>

PGconn bddConnexion();

char ***dbbSelect(PGconn *conn, char **fields, char **params, int size, int *out_nrows, int *out_ncols);

//PGresult *dbbSelect(PGconn *conn, char **fields, char **params, int size);

void dbbFreeResult(char ***table, int nrows, int ncols);