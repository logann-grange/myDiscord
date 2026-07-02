#ifndef ROLES_H
#define ROLES_H
#include <string.h>

typedef enum {
    ROLE_UTILISATEUR = 0,
    ROLE_MODERATEUR = 1,
    ROLE_ADMINISTRATEUR = 2
} UserRole;

// Convertit la chaine "rank" stockee en base en UserRole.
// Valeurs attendues en base : "member", "moderateur", "administrateur".
UserRole roleFromString(const char *rank);

#endif