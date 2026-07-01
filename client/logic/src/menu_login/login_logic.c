#include "../../include/menu_login/login_logic.h"

LoginResult validate_login(const char *pseudo, const char *password) {
    if (strlen(pseudo) == 0 || strlen(password) == 0)
        return LOGIN_CHAMPS_VIDES;

    if (strlen(password) < 6)
        return LOGIN_PASSWORD_TROP_COURT;

    return LOGIN_OK;
}

void hash_password_with_salt(const char *password, const unsigned char *salt, char *hash_hex_out) {
    unsigned char hash[HASH_LEN];

    if (!PKCS5_PBKDF2_HMAC(password, strlen(password),
                            salt, SALT_LEN,
                            ITERATIONS,
                            EVP_sha256(),
                            HASH_LEN, hash)) {
        fprintf(stderr, "Erreur PBKDF2\n");
        return;
    }

    for (int i = 0; i < HASH_LEN; i++) {
        sprintf(hash_hex_out + (i * 2), "%02x", hash[i]);
    }
    hash_hex_out[HASH_LEN * 2] = '\0';
}