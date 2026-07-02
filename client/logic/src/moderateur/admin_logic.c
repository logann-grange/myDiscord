#include "../../include/moderateur/admin_logic.h"
#include "../../include/menu_login/network.h"

AdminResult create_channel(const char *nom_canal, UserRole role_executeur) {
    if (role_executeur < ROLE_ADMINISTRATEUR) return ADMIN_ERREUR_PERMISSION;
    if (nom_canal == NULL || strlen(nom_canal) == 0) return ADMIN_ERREUR_CIBLE_INVALIDE;
    network_create_channel(nom_canal);
    return ADMIN_OK;
}

AdminResult delete_channel(const char *nom_canal, UserRole role_executeur) {
    if (role_executeur < ROLE_ADMINISTRATEUR) return ADMIN_ERREUR_PERMISSION;
    if (nom_canal == NULL || strlen(nom_canal) == 0) return ADMIN_ERREUR_CIBLE_INVALIDE;
    network_delete_channel(nom_canal);
    return ADMIN_OK;
}

AdminResult set_user_role(const char *pseudo_cible, UserRole nouveau_role, UserRole role_executeur) {
    if (role_executeur < ROLE_ADMINISTRATEUR) return ADMIN_ERREUR_PERMISSION;
    if (strlen(pseudo_cible) == 0) return ADMIN_ERREUR_CIBLE_INVALIDE;
    network_set_role(pseudo_cible, (int)nouveau_role);
    return ADMIN_OK;
}