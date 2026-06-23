#include "../include/bddManager.h"


PGconn bddManager()
{
    PGconn *conn = PQconnectdb("host=88.162.239.108 port=5432 dbname=ma_base user=postgres password=C-L-M|130142254517%3");

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connexion échouée: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
}

PGresult *dbbSelect(PGconn *conn, char **fields, char **params, int size)
{
    const char *base = "SELECT * FROM user WHERE 1=1";

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

    // exécution avec les valeurs passées séparément (pas dans la string SQL)
    PGresult *res = PQexecParams(
        conn,
        sql,
        size,                       // nombre de paramètres
        NULL,                       // types des params (NULL = laisser PG deviner)
        (const char *const *)params,// valeurs
        NULL, NULL,                 // longueurs/formats (text par défaut)
        0                           // résultat en format texte
    );

    free(sql);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Erreur requête: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    return res; // faire PQclear(res) après utilisation
}