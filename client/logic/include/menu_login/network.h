#ifndef NETWORK_H
#define NETWORK_H
#include <stdbool.h>

bool network_connect(const char *ip, int port);
bool network_send_login(const char *pseudo, const char *password, char *error_msg_out, size_t err_len);
bool network_send_register(const char *nom, const char *prenom, const char *pseudo, const char *email, const char *password, char *error_msg_out, size_t err_len);

#endif