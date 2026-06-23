#include "../../include/menu_login/register_logic.h"
#include <string.h>

RegisterResult validate_register(const char *nom, const char *prenom, const char *pseudo,
                                 const char *email, const char *password) {
    if (strlen(nom) == 0 || strlen(prenom) == 0 || strlen(pseudo) == 0 ||
        strlen(email) == 0 || strlen(password) == 0)
        return REGISTER_CHAMPS_VIDES;

    if (strlen(password) < 6)
        return REGISTER_PASSWORD_TROP_COURT;

    if (!strpbrk(password, "0123456789") || !strpbrk(password, "!-_ù^$,?'"))
        return REGISTER_PASSWORD_FAIBLE;

    if (!strchr(email, '@'))
        return REGISTER_EMAIL_INVALIDE;

    return REGISTER_OK;
}