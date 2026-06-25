#include "../include/bddManager.h"

PGconn *bddManager(void)
{
    PGconn *conn = PQconnectdb(
        "host=10.10.42.112 port=5432 dbname=Discord user=postgres password=C-L-M|130142254517%3");

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connexion échouée: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    return conn;
}

char ***dbbSelect(PGconn *conn, char **fields, char **params, int size, int *out_nrows, int *out_ncols)
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

void dbbFreeResult(char ***table, int nrows, int ncols)
{
    if (!table) return;
    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < ncols; j++) free(table[i][j]);
        free(table[i]);
    }
    free(table)
}

// PGresult *dbbSelect(PGconn *conn, char **fields, char **params, int size)
// {
//     const char *base = "SELECT * FROM user WHERE 1=1";

//     char *sql = malloc(strlen(base) + 1);
//     if (!sql) return NULL;
//     strcpy(sql, base);

//     for (int i = 0; i < size; i++)
//     {
//         size_t old_len = strlen(sql);

//         int frag_len = snprintf(NULL, 0, " AND %s=$%d", fields[i], i + 1);

//         char *tmp = realloc(sql, old_len + frag_len + 1);
//         if (!tmp) { free(sql); return NULL; }
//         sql = tmp;

//         snprintf(sql + old_len, frag_len + 1, " AND %s=$%d", fields[i], i + 1);
//     }

//     PGresult *res = PQexecParams(
//         conn,
//         sql,
//         size,                        // nombre de paramètres
//         NULL,                        // types des params (NULL = laisser PG deviner)
//         (const char *const *)params, // valeurs
//         NULL, NULL,                  // longueurs/formats (text par défaut)
//         0                            // résultat en format texte
//     );

//     free(sql);

//     if (PQresultStatus(res) != PGRES_TUPLES_OK)
//     {
//         fprintf(stderr, "Erreur requête: %s\n", PQerrorMessage(conn));
//         PQclear(res);
//         return NULL;
//     }

//     return res; // faire PQclear(res) après utilisation
// }

