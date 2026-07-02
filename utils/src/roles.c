#include "../include/roles.h"
#include <string.h>

UserRole roleFromString(const char *rank) {
    if (!rank) return ROLE_UTILISATEUR;
    if (strcmp(rank, "administrateur") == 0) return ROLE_ADMINISTRATEUR;
    if (strcmp(rank, "moderateur") == 0)     return ROLE_MODERATEUR;
    return ROLE_UTILISATEUR;
}