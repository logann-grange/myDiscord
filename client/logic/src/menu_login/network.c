#include "network.h"
#include "net_utils.h"
#include "protocol.h"
#include "../../include/menu_login/login_logic.h"
#include "../../include/menu_login/register_logic.h"
#include <string.h>
#include <stdio.h>
#include <openssl/rand.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define closesocket close
#endif

extern SOCKET sock; 

bool network_send_login(const char *pseudo, const char *password, char *error_msg_out, size_t err_len) 
{
    // Demander le sel pour ce pseudo
    sendType(sock, MSG_LOGIN_SALT_REQUEST);
    sendString(sock, pseudo);

    char salt_hex[SALT_LEN * 2 + 1];
    MessageType type;
    recvType(sock, &type);
    if (recvString(sock, salt_hex, sizeof(salt_hex)) < 0 || strlen(salt_hex) == 0) {
        snprintf(error_msg_out, err_len, "Pseudo inconnu.");
        return false;
    }

    // Reconvertir le sel hex -> bytes, puis hasher le mot de passe localement
    unsigned char salt[SALT_LEN];
    for (int i = 0; i < SALT_LEN; i++)
        sscanf(salt_hex + i * 2, "%2hhx", &salt[i]);

    char hash_hex[HASH_LEN * 2 + 1];
    hash_password_with_salt(password, salt, hash_hex);

    // Envoyer pseudo + hash pour verification
    sendType(sock, MSG_LOGIN_REQUEST);
    sendString(sock, pseudo);
    sendString(sock, hash_hex);

    char response[256];
    recvType(sock, &type);
    recvString(sock, response, sizeof(response));

    if (strcmp(response, "OK") == 0) return true;
    snprintf(error_msg_out, err_len, "%s", response);
    return false;
}

bool network_send_register(const char *nom, const char *prenom, const char *pseudo, const char *email, const char *password, char *error_msg_out, size_t err_len) 
{
    unsigned char salt[SALT_LEN];
    unsigned char hash[HASH_LEN];
    if (!RAND_bytes(salt, SALT_LEN)) {
        snprintf(error_msg_out, err_len, "Erreur generation sel.");
        return false;
    }

    char salt_hex[SALT_LEN * 2 + 1];
    char hash_hex[HASH_LEN * 2 + 1];
    byte_to_hex(salt, SALT_LEN, salt_hex);
    hash_password_with_salt(password, salt, hash_hex);

    sendType(sock, MSG_REGISTER_REQUEST);
    sendString(sock, nom);
    sendString(sock, prenom);
    sendString(sock, pseudo);
    sendString(sock, email);
    sendString(sock, salt_hex);
    sendString(sock, hash_hex);

    char response[256];
    MessageType type;
    recvType(sock, &type);
    recvString(sock, response, sizeof(response));

    if (strcmp(response, "OK") == 0) return true;
    snprintf(error_msg_out, err_len, "%s", response);
    return false;
}