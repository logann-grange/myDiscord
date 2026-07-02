#ifndef PROTOCOL_H
#define PROTOCOL_H

#define SALT_LEN 16
#define HASH_LEN 32

typedef enum {
    MSG_LOGIN_SALT_REQUEST = 1,
    MSG_LOGIN_SALT_RESPONSE,
    MSG_LOGIN_REQUEST,
    MSG_LOGIN_RESPONSE,
    MSG_REGISTER_REQUEST,
    MSG_REGISTER_RESPONSE,

    MSG_CHAT,
    MSG_CHAT_INCOMING,

    MSG_CHANNEL_LIST_REQUEST,
    MSG_CHANNEL_LIST_RESPONSE,
    MSG_CHANNEL_HISTORY_REQUEST,
    MSG_CHANNEL_HISTORY_RESPONSE,

    // Moderation (verifiees cote serveur, jamais fait confiance au client)
    MSG_KICK_REQUEST,            // pseudo cible
    MSG_TIMEOUT_REQUEST,         // pseudo cible, duree_secondes (string)
    MSG_DELETE_MESSAGE_REQUEST,  // message_id (string)

    // Administration
    MSG_CREATE_CHANNEL_REQUEST,  // nom canal
    MSG_DELETE_CHANNEL_REQUEST,  // nom canal
    MSG_SET_ROLE_REQUEST,        // pseudo cible, nouveau_role (string "0"/"1"/"2")

    MSG_MOD_RESPONSE,            // serveur -> client : "OK" ou message d'erreur (toutes actions ci-dessus)

    MSG_MESSAGE_DELETED_BROADCAST // serveur -> tous les clients du canal : channel, message_id (string)
} MessageType;

#endif