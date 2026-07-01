#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef enum {
    MSG_LOGIN_SALT_REQUEST = 1,   // client -> serveur : "donne-moi le sel de ce pseudo"
    MSG_LOGIN_SALT_RESPONSE,      // serveur -> client : sel (hex) ou vide si pseudo inconnu
    MSG_LOGIN_REQUEST,            // client -> serveur : pseudo + hash
    MSG_LOGIN_RESPONSE,           // serveur -> client : "OK" ou message d'erreur
    MSG_REGISTER_REQUEST,         // client -> serveur : nom, prenom, pseudo, email, salt, hash
    MSG_REGISTER_RESPONSE,        // serveur -> client : "OK" ou message d'erreur
    MSG_CHAT
} MessageType;

#endif