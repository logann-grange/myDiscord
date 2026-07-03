#ifndef LOGIN_LOGIC_H
#define LOGIN_LOGIC_H

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>

#define SALT_LEN   16
#define HASH_LEN   32
#define ITERATIONS 100000


typedef enum {
    LOGIN_OK,
    LOGIN_CHAMPS_VIDES,
    LOGIN_PASSWORD_TROP_COURT
} LoginResult;

LoginResult validate_login(const char *pseudo, const char *password);
void hash_password_with_salt(const char *password, const unsigned char *salt, char *hash_hex_out);
#endif