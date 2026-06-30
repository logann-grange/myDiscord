#include "../../include/moderateur/admin_logic.h"




AdminResult create_channel(const char *nom_canal, UserRole role_executeur) {
    if (role_executeur < ROLE_ADMINISTRATEUR) {
        return ADMIN_ERREUR_PERMISSION;
    }
    if (nom_canal == NULL || strlen(nom_canal) == 0) {
        return ADMIN_ERREUR_CIBLE_INVALIDE;
    }
    return ADMIN_OK;
}

AdminResult delete_channel(const char *nom_canal, UserRole role_executeur) {
    if (role_executeur < ROLE_ADMINISTRATEUR) {
        return ADMIN_ERREUR_PERMISSION;
    }
    if (nom_canal == NULL || strlen(nom_canal) == 0) {
        return ADMIN_ERREUR_CIBLE_INVALIDE;
    }
    return ADMIN_OK;
}

AdminResult set_user_role(const char *pseudo_cible, UserRole nouveau_role, UserRole role_executeur) {
     if (role_executeur < ROLE_ADMINISTRATEUR)
        return ADMIN_ERREUR_PERMISSION;
    if (strlen(pseudo_cible) == 0)
        return ADMIN_ERREUR_CIBLE_INVALIDE;

    return ADMIN_OK;
}