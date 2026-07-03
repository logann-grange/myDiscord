#ifndef MOD_LOGIC_H
#define MOD_LOGIC_H

#include "../user_role.h"

typedef enum {
    MOD_OK,
    MOD_ERREUR_PERMISSION,
    MOD_ERREUR_CIBLE_INVALIDE,
} ModResult;

ModResult kick_user(const char *pseudo_cible, UserRole role_executeur,UserRole role_cible);
ModResult delete_message(int message_id, UserRole role_executeur,UserRole role_cible);
ModResult timeout_user(const char *pseudo_cible, int duree, UserRole role_executeur,UserRole role_cible);

#endif