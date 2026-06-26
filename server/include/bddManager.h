#include <string.h>
#include <stdlib.h>
//#include <libpq-fe.h>
#include <C:/msys64/mingw64/include/libpq-fe.h>

PGconn *bddConnexion();

char ***bddSelect(PGconn *conn, char **fields, char **params, int size, int *out_nrows, int *out_ncols);

int bddInsert(PGconn *conn, const char *tableName, char **fields, char **params, int size);

void bddFreeResult(char ***table, int nrows, int ncols);