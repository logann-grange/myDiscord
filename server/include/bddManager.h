#include <libpq-fe.h>
#include <string.h>
#include <stdlib.h>

PGconn *bddConnexion();

char ***bddSelect(PGconn *conn, char **fields, char **params, int size, int *out_nrows, int *out_ncols);

void bddInsert(PGconn *conn, const char *tableName, char **fields, char **params, int size);

void bddFreeResult(char ***table, int nrows, int ncols);