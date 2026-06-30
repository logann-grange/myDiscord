#ifndef ADMIN_LOGIC_H
#define ADMIN_LOGIC_H

#include "../../graphic/include/menu_login/app.h"

typedef enum {
    ADMIN_OK,
    ADMIN_ERREUR_PERMISSION,
    ADMIN_ERREUR_CIBLE_INVALIDE,

}AdminResult;

AdminResult create_channel(const char *nom_canal, UserRole role_executeur);
AdminResult delete_channel(const char *nom_canal, UserRole role_executeur);
AdminResult set_user_role(const char *pseudo_cible, UserRole nouveau_role, UserRole role_executeur);

#endif