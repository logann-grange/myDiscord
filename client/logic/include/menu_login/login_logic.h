#ifndef LOGIN_LOGIC_H
#define LOGIN_LOGIC_H

typedef enum {
    LOGIN_OK,
    LOGIN_CHAMPS_VIDES,
    LOGIN_PASSWORD_TROP_COURT
} LoginResult;

LoginResult validate_login(const char *pseudo, const char *password);

#endif