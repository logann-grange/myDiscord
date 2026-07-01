#include "../../include/menu_login/settings_logic.h"
#include <string.h>

SettingsResult validate_pseudo(const char *new_pseudo) {
    if (strlen(new_pseudo) == 0)
        return SETTINGS_PSEUDO_VIDE;

    if (strlen(new_pseudo) < 3)
        return SETTINGS_PSEUDO_TROP_COURT;

    // TODO: vérifier que le pseudo n'est pas déjà pris (via serveur)
    return SETTINGS_OK;
}