#ifndef NETWORK_H
#define NETWORK_H

#include <stdbool.h>
#include <stddef.h>

typedef void (*MessageCallback)(int message_id, const char *auteur, const char *channel,
                                 const char *texte, const char *date);
typedef void (*ChannelCallback)(const char *category, const char *channel_name);

// Connexion
bool network_connect(const char *ip, int port);
void network_disconnect(void);

// Authentification
bool network_send_login(const char *pseudo, const char *password, char *error_msg_out, size_t err_len);
bool network_send_register(const char *nom, const char *prenom, const char *pseudo,
                            const char *email, const char *password,
                            char *error_msg_out, size_t err_len);

// Chat
void network_start_listening(MessageCallback cb);
void network_set_channel_callback(ChannelCallback cb);
void network_send_message(const char *channel, const char *text);

void network_request_channels(void);

void network_request_channel_history(const char *channel);

void network_send_update_pseudo(const char *new_pseudo);

void network_send_reaction(int message_id, const char *emoji);

void network_send_kick(const char *pseudo);

void network_send_timeout(const char *pseudo, int duree);

void network_send_delete_message(int message_id);

typedef void (*ModResponseCallback)(bool success, const char *message);

void network_set_mod_callback(ModResponseCallback cb);

void network_send_kick(const char *pseudo);

void network_send_timeout(const char *pseudo, int duree);

void network_send_delete_message(int message_id);

void network_create_channel(const char *name);

void network_delete_channel(const char *name);

void network_set_role(const char *pseudo, int role);

typedef void (*DeleteMessageCallback)(const char *channel, int message_id);

void network_set_delete_callback(DeleteMessageCallback cb);

#endif