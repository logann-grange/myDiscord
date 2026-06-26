#include "../../include/moderateur/mod_logic.h"

ModResult kick_user(const char *pseudo_cible, UserRole role_executeur,UserRole role_cible) {
    if (role_executeur < ROLE_MODERATEUR || role_executeur < role_cible) {
        return MOD_ERREUR_PERMISSION;
    }
    if (strlen(pseudo_cible) == 0) {    
        return MOD_ERREUR_CIBLE_INVALIDE;

    }

    //todo: send kick request to server
    return MOD_OK;
}

ModResult delete_message(const char *message_id, UserRole role_executeur,UserRole role_cible) {
    if (role_executeur < ROLE_MODERATEUR)
        return MOD_ERREUR_PERMISSION;

    //todo: send delete message request to server
    return MOD_OK;
}

ModResult timeout_user(const char *pseudo_cible, int duree, UserRole role_executeur,UserRole role_cible) {
    if (role_executeur < ROLE_MODERATEUR)
        return MOD_ERREUR_PERMISSION;
    if (strlen(pseudo_cible) == 0)
        return MOD_ERREUR_CIBLE_INVALIDE;
    //todo: send timeout request to server
    return MOD_OK;
}
    