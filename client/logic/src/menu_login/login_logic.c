#include "../../include/menu_login/login_logic.h"
#include <string.h>

LoginResult validate_login(const char *pseudo, const char *password) {
    if (strlen(pseudo) == 0 || strlen(password) == 0)
        return LOGIN_CHAMPS_VIDES;

    if (strlen(password) < 6)
        return LOGIN_PASSWORD_TROP_COURT;

    return LOGIN_OK;
}