#ifndef REGISTER_LOGIC_H
#define REGISTER_LOGIC_H

typedef enum {
    REGISTER_OK,
    REGISTER_CHAMPS_VIDES,
    REGISTER_PASSWORD_TROP_COURT,
    REGISTER_PASSWORD_FAIBLE,
    REGISTER_EMAIL_INVALIDE
} RegisterResult;

RegisterResult validate_register(const char *nom, const char *prenom, const char *pseudo,
                                 const char *email, const char *password);

#endif