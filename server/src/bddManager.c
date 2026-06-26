#include "../include/bddManager.h"

PGconn *bddConnexion(void)
{
    PGconn *conn = PQconnectdb(
        "host=10.10.41.112 port=5432 dbname=discord user=admin password=C-L-M|130142254517%3");

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connexion échouée: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    return conn;
}

char ***bddSelect(PGconn *conn, char **fields, char **params, int size, int *out_nrows, int *out_ncols)
{
    const char *base = "SELECT * FROM \"user\" WHERE 1=1";

    char *sql = malloc(strlen(base) + 1);
    if (!sql) return NULL;
    strcpy(sql, base);

    for (int i = 0; i < size; i++)
    {
        size_t old_len = strlen(sql);
        int frag_len = snprintf(NULL, 0, " AND %s=$%d", fields[i], i + 1);

        char *tmp = realloc(sql, old_len + frag_len + 1);
        if (!tmp) { free(sql); return NULL; }
        sql = tmp;

        snprintf(sql + old_len, frag_len + 1, " AND %s=$%d", fields[i], i + 1);
    }

    PGresult *res = PQexecParams(
        conn, sql, size, NULL,
        (const char *const *)params,
        NULL, NULL, 0
    );

    free(sql);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Erreur requête: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    int nrows = PQntuples(res);
    int ncols = PQnfields(res);

    char ***table = malloc(nrows * sizeof(char **));
    if (!table) { PQclear(res); return NULL; }

    for (int i = 0; i < nrows; i++)
    {
        table[i] = malloc(ncols * sizeof(char *));
        if (!table[i])
        {
            // libération de ce qui a déjà été alloué avant de sortir
            for (int k = 0; k < i; k++)
            {
                for (int j = 0; j < ncols; j++) free(table[k][j]);
                free(table[k]);
            }
            free(table);
            PQclear(res);
            return NULL;
        }

        for (int j = 0; j < ncols; j++)
        {
            if (PQgetisnull(res, i, j))
            {
                table[i][j] = NULL;
            }
            else
            {
                const char *val = PQgetvalue(res, i, j);
                table[i][j] = strdup(val); // copie car PGresult va être libéré
            }
        }
    }

    PQclear(res);

    *out_nrows = nrows;
    *out_ncols = ncols;
    return table;
}

int bddInsert(PGconn *conn, const char *tableName, char **fields, char **params, int size)
{
    if (size <= 0) return 0;

    size_t cap = 32 + strlen(tableName);
    char *sql = malloc(cap);
    if (!sql) return 0;

    int len = snprintf(sql, cap, "INSERT INTO %s (", tableName);

    // Liste des champs : (champ1, champ2, ...)
    for (int i = 0; i < size; i++)
    {
        int frag_len = snprintf(NULL, 0, "%s%s", i > 0 ? ", " : "", fields[i]);
        char *tmp = realloc(sql, len + frag_len + 1);
        if (!tmp) { free(sql); return 0; }
        sql = tmp;
        len += snprintf(sql + len, frag_len + 1, "%s%s", i > 0 ? ", " : "", fields[i]);
    }

    int tail_len = snprintf(NULL, 0, ") VALUES (");
    char *tmp = realloc(sql, len + tail_len + 1);
    if (!tmp) { free(sql); return 0; }
    sql = tmp;
    len += snprintf(sql + len, tail_len + 1, ") VALUES (");

    for (int i = 0; i < size; i++)
    {
        int frag_len = snprintf(NULL, 0, "%s$%d", i > 0 ? ", " : "", i + 1);
        char *t2 = realloc(sql, len + frag_len + 1);
        if (!t2) { free(sql); return 0; }
        sql = t2;
        len += snprintf(sql + len, frag_len + 1, "%s$%d", i > 0 ? ", " : "", i + 1);
    }

    char *t3 = realloc(sql, len + 2);
    if (!t3) { free(sql); return 0; }
    sql = t3;
    snprintf(sql + len, 2, ")");

    PGresult *res = PQexecParams(conn, sql, size, NULL, (const char * const *)params, NULL, NULL, 0);

    int ok = (PQresultStatus(res) == PGRES_COMMAND_OK);
    if (!ok)
        fprintf(stderr, "Insertion échouée: %s\n", PQerrorMessage(conn));

    PQclear(res);
    free(sql);
    return ok;
}

void bddFreeResult(char ***table, int nrows, int ncols)
{
    if (!table) return;
    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < ncols; j++) free(table[i][j]);
        free(table[i]);
    }
    free(table);
}

int main() {
    PGconn *conn = bddConnexion();
    char *fields[] = {"email", "nom", "prenom", "pseudo", "mot de passe", "statut", "role"};
    char *params[] = {"admin@discord.fr", "admin", "admin", "admin", "admin", "", "admin"};


    bddInsert(conn, "user", fields, params, 7);
    return 0;
}