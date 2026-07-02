#ifndef REGISTER_LOGIC_H
#define REGISTER_LOGIC_H
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>
#include "../network/network_client.h"
#define SALT_LEN   16
#define HASH_LEN   32
#define ITERATIONS 100000 

typedef enum {
    REGISTER_OK,
    REGISTER_CHAMPS_VIDES,
    REGISTER_PASSWORD_TROP_COURT,
    REGISTER_PASSWORD_FAIBLE,
    REGISTER_EMAIL_INVALIDE
} RegisterResult;

RegisterResult validate_register(const char *nom, const char *prenom, const char *pseudo,
                                 const char *email, const char *password);

void byte_to_hex(const unsigned char *bytes, int len, char *hex_str);

char *hash_password(const char *password);

#endif