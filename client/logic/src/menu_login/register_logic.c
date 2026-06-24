#include "../../include/menu_login/register_logic.h"


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

void byte_to_hex(const unsigned char *bytes, int len, char *hex_str) {
    for (int i = 0; i < len; i++) {
        sprintf(hex_str + (i * 2), "%02x", bytes[i]);
    }
    hex_str[len * 2] = '\0';
}

void hash_password(const char *password) {
    unsigned char salt[SALT_LEN];
    unsigned char hash[HASH_LEN];

    // Génère un sel aléatoire
    if (!RAND_bytes(salt, SALT_LEN)) {
        fprintf(stderr, "Erreur génération sel\n");
        return;
    }

    // Applique PBKDF2 avec SHA-256
    if (!PKCS5_PBKDF2_HMAC(password, strlen(password),
                            salt, SALT_LEN,
                            ITERATIONS,
                            EVP_sha256(),
                            HASH_LEN, hash)) {
        fprintf(stderr, "Erreur PBKDF2\n");
        return;
    }

    // Convertit en hexadécimal
    char salt_hex[SALT_LEN * 2 + 1];
    char hash_hex[HASH_LEN * 2 + 1];
    byte_to_hex(salt, SALT_LEN, salt_hex);
    byte_to_hex(hash, HASH_LEN, hash_hex);

    printf("Salt: %s\n", salt_hex);
    printf("Hash: %s\n", hash_hex);

    // TODO: envoyer salt_hex + hash_hex au serveur
}