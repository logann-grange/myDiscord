#ifndef SERVER_INCLUDE_BDDMANAGER_H
#define SERVER_INCLUDE_BDDMANAGER_H

#include <string.h>
#include <stdlib.h>
//#include <libpq-fe.h>
#include <C:/msys64/mingw64/include/libpq-fe.h>

PGconn *bddConnexion();

char ***bddSelect(const char *tableName, char **fields, char **params, int size, int *out_nrows, int *out_ncols);

int bddInsert(const char *tableName, char **fields, char **params, int size);

int bddUpdate(const char *tableName, char **fields, char **params, int size, int id);

void bddFreeResult(char ***table, int nrows, int ncols);

#endif /* SERVER_INCLUDE_BDDMANAGER_H */