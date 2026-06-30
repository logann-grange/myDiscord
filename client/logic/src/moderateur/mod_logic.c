#include "../../include/moderateur/mod_logic.h"

ModResult kick_user(const char *pseudo_cible, UserRole role_executeur,UserRole role_cible) {
    if (role_executeur < ROLE_MODERATEUR || role_executeur < role_cible) {
        return MOD_ERREUR_PERMISSION;
    }
    if (strlen(pseudo_cible) == 0) {    
        return MOD_ERREUR_CIBLE_INVALIDE;

    }

    network_send_kick(pseudo_cible);
    return MOD_OK;
}

ModResult delete_message(int message_id, UserRole role_executeur,UserRole role_cible) {
    if (role_executeur < ROLE_MODERATEUR)
        return MOD_ERREUR_PERMISSION;

    network_send_delete_message(message_id);
    return MOD_OK;
}

ModResult timeout_user(const char *pseudo_cible, int duree, UserRole role_executeur,UserRole role_cible) {
    if (role_executeur < ROLE_MODERATEUR)
        return MOD_ERREUR_PERMISSION;
    if (strlen(pseudo_cible) == 0)
        return MOD_ERREUR_CIBLE_INVALIDE;
    network_send_timeout(pseudo_cible, duree);
    return MOD_OK;
}
    