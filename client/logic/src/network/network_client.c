// network_client.c disabled for GUI build to avoid duplicate symbols.
#if 0
#include "../../include/network/network_client.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#    include <winsock2.h>
#    include <ws2tcpip.h>
#else
#    include <arpa/inet.h>
#    include <netinet/in.h>
#    include <sys/socket.h>
#    include <unistd.h>
#    define closesocket close
#    define SOCKET int
#    define INVALID_SOCKET (-1)
#    define SOCKET_ERROR (-1)
#endif

// Global socket partagé (menu_login/network.c attend un "extern SOCKET sock;")
SOCKET sock = INVALID_SOCKET;

static ChannelReceivedCallback on_channel_received = NULL;
static MessageReceivedCallback on_message_received = NULL;
static pthread_t listen_thread_handle;
static bool listener_running = false;
static bool channels_announced = false;

static void dispatch_raw_message(const char *buffer) {
    if (on_message_received == NULL) return;

    char auteur[64] = "Serveur";
    char texte[1024];
    const char *sep = strstr(buffer, ": ");
    if (sep) {
        size_t alen = (size_t)(sep - buffer);
        if (alen >= sizeof(auteur)) alen = sizeof(auteur) - 1;
        memcpy(auteur, buffer, alen);
        auteur[alen] = '\0';
        strncpy(texte, sep + 2, sizeof(texte) - 1);
        texte[sizeof(texte)-1] = '\0';
    } else {
        strncpy(texte, buffer, sizeof(texte) - 1);
        texte[sizeof(texte) - 1] = '\0';
    }

    on_message_received(0, auteur, "general", texte, "");
}

static void *listen_thread(void *arg) {
    (void)arg;
    while (listener_running) {
        char buffer[1024];
        int r = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (r <= 0) break;
        buffer[r] = '\0';
        dispatch_raw_message(buffer);
    }
    listener_running = false;
    return NULL;
}

bool network_connect(const char *ip, int port) {
    if (sock != INVALID_SOCKET) return true;

#ifdef _WIN32
    static bool wsa = false;
    if (!wsa) { WSADATA d; if (WSAStartup(MAKEWORD(2,2), &d) != 0) return false; wsa = true; }
#endif

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return false;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
        closesocket(sock); sock = INVALID_SOCKET; return false;
    }

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(sock); sock = INVALID_SOCKET; return false;
    }

    channels_announced = false;
    return true;
}

void network_disconnect(void) {
    listener_running = false;
    if (sock != INVALID_SOCKET) { closesocket(sock); sock = INVALID_SOCKET; }
#ifdef _WIN32
    WSACleanup();
#endif
}

bool network_send_message(const char *channel, const char *text) {
    (void)channel;
    if (sock == INVALID_SOCKET) return false;
    size_t l = strlen(text);
    return send(sock, text, (int)l, 0) != SOCKET_ERROR;
}

bool network_send_reaction(int message_id, const char *emoji) { (void)message_id; (void)emoji; return false; }
bool network_send_kick(const char *pseudo_cible) { (void)pseudo_cible; return false; }
bool network_send_timeout(const char *pseudo_cible, int duree_secondes) { (void)pseudo_cible; (void)duree_secondes; return false; }
bool network_send_delete_message(int message_id) { (void)message_id; return false; }
bool network_send_create_channel(const char *nom_canal) { (void)nom_canal; return false; }
bool network_send_delete_channel(const char *nom_canal) { (void)nom_canal; return false; }
bool network_send_set_role(const char *pseudo_cible, int nouveau_role) { (void)pseudo_cible; (void)nouveau_role; return false; }
bool network_send_update_pseudo(const char *new_pseudo) { (void)new_pseudo; return false; }
bool network_request_channel_history(const char *channel) { (void)channel; return true; }

void network_set_channel_callback(ChannelReceivedCallback callback) { on_channel_received = callback; }

void network_start_listening(MessageReceivedCallback callback) {
    on_message_received = callback;
    if (listener_running || sock == INVALID_SOCKET) return;
    listener_running = true;
    pthread_create(&listen_thread_handle, NULL, listen_thread, NULL);
    pthread_detach(listen_thread_handle);
}

bool network_request_channels(void) {
    if (channels_announced) return true;
    channels_announced = true;
    if (on_channel_received) on_channel_received("Général", "general");
    return true;
}
#endif