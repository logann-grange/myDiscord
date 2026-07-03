#include "../include/bddManager.h"
#include "../../client/logic/include/menu_login/register_logic.h"

PGconn *bddConnexion(void)
{
    PGconn *conn = PQconnectdb(
        /*"host=10.10.41.112 port=5432 dbname=discord user=admin password=C-L-M|130142254517%3");*/
        "host=10.10.76.132 port=5432 dbname=discordlike user=app password=devpassword");

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connexion échouée: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    return conn;
}

char ***bddSelect(const char *tableName, char **fields, char **params, int size, int *out_nrows, int *out_ncols)
{
    PGconn *conn = bddConnexion();

    int base_len = snprintf(NULL, 0, "SELECT * FROM \"%s\" WHERE 1=1", tableName);
    char *sql = malloc(base_len + 1);
    if (!sql) return NULL;
    snprintf(sql, base_len + 1, "SELECT * FROM \"%s\" WHERE 1=1", tableName);

    for (int i = 0; i < size; i++)
    {
        size_t old_len = strlen(sql);
        int frag_len = snprintf(NULL, 0, " AND %s=$%d", fields[i], i + 1);

        char *tmp = realloc(sql, old_len + frag_len + 1);
        if (!tmp) { free(sql); return NULL; }
        sql = tmp;

        snprintf(sql + old_len, frag_len + 1, " AND %s=$%d", fields[i], i + 1);
    }

    printf("DEBUG bddSelect: tableName='%s' (len=%zu)  sql='%s'\n", tableName, strlen(tableName), sql);

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
                table[i][j] = strdup(val);
            }
        }
    }

    PQclear(res);

    *out_nrows = nrows;
    *out_ncols = ncols;
    return table;
}

// Retourne l'id inséré (>= 0) en cas de succès, -1 en cas d'échec.
// NOTE: suppose que la table a une colonne "id" (c'est le cas pour
// "user", "channel" et "message" dans ce projet).
int bddInsert(const char *tableName, char **fields, char **params, int size)
{
    PGconn *conn = bddConnexion();

    if (size <= 0) return -1;

    size_t cap = 32 + strlen(tableName);
    char *sql = malloc(cap);
    if (!sql) return -1;

    int len = snprintf(sql, cap, "INSERT INTO %s (", tableName);

    // Liste des champs : (champ1, champ2, ...)
    for (int i = 0; i < size; i++)
    {
        int frag_len = snprintf(NULL, 0, "%s%s", i > 0 ? ", " : "", fields[i]);
        char *tmp = realloc(sql, len + frag_len + 1);
        if (!tmp) { free(sql); return -1; }
        sql = tmp;
        len += snprintf(sql + len, frag_len + 1, "%s%s", i > 0 ? ", " : "", fields[i]);
    }

    int tail_len = snprintf(NULL, 0, ") VALUES (");
    char *tmp = realloc(sql, len + tail_len + 1);
    if (!tmp) { free(sql); return -1; }
    sql = tmp;
    len += snprintf(sql + len, tail_len + 1, ") VALUES (");

    for (int i = 0; i < size; i++)
    {
        int frag_len = snprintf(NULL, 0, "%s$%d", i > 0 ? ", " : "", i + 1);
        char *t2 = realloc(sql, len + frag_len + 1);
        if (!t2) { free(sql); return -1; }
        sql = t2;
        len += snprintf(sql + len, frag_len + 1, "%s$%d", i > 0 ? ", " : "", i + 1);
    }

    // ")" puis " RETURNING id" pour récupérer l'id généré
    const char *closing = ") RETURNING id";
    int closing_len = (int)strlen(closing);
    char *t3 = realloc(sql, len + closing_len + 1);
    if (!t3) { free(sql); return -1; }
    sql = t3;
    snprintf(sql + len, closing_len + 1, "%s", closing);

    PGresult *res = PQexecParams(conn, sql, size, NULL, (const char * const *)params, NULL, NULL, 0);

    int insertedId = -1;
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0)
    {
        insertedId = atoi(PQgetvalue(res, 0, 0));
    }
    else
    {
        fprintf(stderr, "Insertion échouée: %s\n", PQerrorMessage(conn));
    }

    PQclear(res);
    free(sql);
    return insertedId;
}

int bddUpdate(const char *tableName, char **fields, char **params, int size, int id)
{
    PGconn *conn = bddConnexion();

    if (size <= 0) return 0;

    size_t cap = 32 + strlen(tableName);
    char *sql = malloc(cap);
    if (!sql) return 0;

    int len = snprintf(sql, cap, "UPDATE %s SET ", tableName);

    for (int i = 0; i < size; i++)
    {
        int frag_len = snprintf(NULL, 0, "%s%s = $%d", i > 0 ? ", " : "", fields[i], i + 1);
        char *tmp = realloc(sql, len + frag_len + 1);
        if (!tmp) { free(sql); return 0; }
        sql = tmp;
        len += snprintf(sql + len, frag_len + 1, "%s%s = $%d", i > 0 ? ", " : "", fields[i], i + 1);
    }

    int where_len = snprintf(NULL, 0, " WHERE id = $%d", size + 1);
    char *tmp2 = realloc(sql, len + where_len + 1);
    if (!tmp2) { free(sql); return 0; }
    sql = tmp2;
    len += snprintf(sql + len, where_len + 1, " WHERE id = $%d", size + 1);

    char idStr[32];
    snprintf(idStr, sizeof(idStr), "%d", id);

    char **allParams = malloc((size + 1) * sizeof(char *));
    if (!allParams) { free(sql); return 0; }
    for (int i = 0; i < size; i++)
        allParams[i] = params[i];
    allParams[size] = idStr;

    PGresult *res = PQexecParams(conn, sql, size + 1, NULL, (const char * const *)allParams, NULL, NULL, 0);

    int ok = (PQresultStatus(res) == PGRES_COMMAND_OK);
    if (!ok)
        fprintf(stderr, "Mise à jour échouée: %s\n", PQerrorMessage(conn));

    PQclear(res);
    free(sql);
    free(allParams);
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


//  int main() {
//     PGconn *conn = bddConnexion();

//     const char *password = "test";
//     char *hashed = hash_password(password);
//     if (!hashed) {
//         fprintf(stderr, "Échec du hash\n");
//         return 1;
//     }

//     char *fields[] = {"email", "name", "firstName", "pseudo", "password", "status", "rank"};
//     char *params[] = {"test@discord.fr", "test", "test", "test", hashed, "", "member"};

//     bddInsert("\"users\"", fields, params, 7);
// }
