#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

typedef void (*MessageReceivedCallback)(int message_id, const char *auteur,
                                          const char *channel, const char *texte, const char *date);

void network_start_listening(MessageReceivedCallback callback);

// Connexion au serveur
bool network_connect(const char *ip, int port);
void network_disconnect(void);

// Authentification
bool network_send_login(const char *pseudo, const char *password_hash);
bool network_send_register(const char *nom, const char *prenom, const char *pseudo,
                            const char *email, const char *password_hash, const char *salt);

// Messages
bool network_send_message(const char *channel, const char *text);
bool network_send_reaction(int message_id, const char *emoji);
bool network_request_channel_history(const char *channel);

// Modération
bool network_send_kick(const char *pseudo_cible);
bool network_send_timeout(const char *pseudo_cible, int duree_secondes);
bool network_send_delete_message(int message_id);

// Administration
bool network_send_create_channel(const char *nom_canal);
bool network_send_delete_channel(const char *nom_canal);
bool network_send_set_role(const char *pseudo_cible, int nouveau_role);

// Profil
bool network_send_update_pseudo(const char *new_pseudo);

#endif // NETWORK_CLIENT_H