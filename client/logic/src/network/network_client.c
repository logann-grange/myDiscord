#include "../../include/network/network_client.h"


static MessageReceivedCallback on_message_received = NULL;

static void *listen_thread(void *arg) {
    while (1) {
        // TODO: recv() sur le socket, bloquant
        // Quand un message arrive, parser le paquet et appeler :
        // on_message_received(auteur, channel, texte);
        
        // Pour l'instant, rien ne se passe
        printf("[NETWORK] Thread d'écoute en attente (non implémenté)\n");
        break; // évite la boucle infinie tant que rien n'est implémenté
    }
    return NULL;
}

void network_start_listening(MessageReceivedCallback callback) {
    on_message_received = callback;
    pthread_t thread;
    pthread_create(&thread, NULL, listen_thread, NULL);
    pthread_detach(thread);
}

bool network_connect(const char *ip, int port) {
    printf("[NETWORK] Connexion à %s:%d (non implémenté)\n", ip, port);
    // TODO: socket(), connect()
    return false;
}

void network_disconnect(void) {
    printf("[NETWORK] Déconnexion (non implémenté)\n");
    // TODO: close socket
}

bool network_send_login(const char *pseudo, const char *password_hash) {
    printf("[NETWORK] Login: %s (non implémenté)\n", pseudo);
    // TODO: envoyer paquet LOGIN
    return false;
}

bool network_send_register(const char *nom, const char *prenom, const char *pseudo,
                            const char *email, const char *password_hash, const char *salt) {
    printf("[NETWORK] Register: %s (non implémenté)\n", pseudo);
    // TODO: envoyer paquet REGISTER
    return false;
}

bool network_send_message(const char *channel, const char *text) {
    printf("[NETWORK] Message dans #%s: %s (non implémenté)\n", channel, text);
    // TODO: envoyer paquet MESSAGE
    return false;
}

bool network_send_reaction(int message_id, const char *emoji) {
    printf("[NETWORK] Réaction sur message %d: %s (non implémenté)\n", message_id, emoji);
    // TODO: envoyer paquet REACTION
    return false;
}

bool network_send_kick(const char *pseudo_cible) {
    printf("[NETWORK] Kick: %s (non implémenté)\n", pseudo_cible);
    // TODO: envoyer paquet KICK
    return false;
}

bool network_send_timeout(const char *pseudo_cible, int duree_secondes) {
    printf("[NETWORK] Timeout: %s pour %ds (non implémenté)\n", pseudo_cible, duree_secondes);
    // TODO: envoyer paquet TIMEOUT
    return false;
}

bool network_send_delete_message(int message_id) {
    printf("[NETWORK] Suppression message %d (non implémenté)\n", message_id);
    // TODO: envoyer paquet DELETE_MESSAGE
    return false;
}

bool network_send_create_channel(const char *nom_canal) {
    printf("[NETWORK] Création canal: %s (non implémenté)\n", nom_canal);
    // TODO: envoyer paquet CREATE_CHANNEL
    return false;
}

bool network_send_delete_channel(const char *nom_canal) {
    printf("[NETWORK] Suppression canal: %s (non implémenté)\n", nom_canal);
    // TODO: envoyer paquet DELETE_CHANNEL
    return false;
}

bool network_send_set_role(const char *pseudo_cible, int nouveau_role) {
    printf("[NETWORK] Changement rôle %s -> %d (non implémenté)\n", pseudo_cible, nouveau_role);
    // TODO: envoyer paquet SET_ROLE
    return false;
}

bool network_send_update_pseudo(const char *new_pseudo) {
    printf("[NETWORK] Changement pseudo -> %s (non implémenté)\n", new_pseudo);
    // TODO: envoyer paquet UPDATE_PSEUDO
    return false;
}

bool network_request_channel_history(const char *channel) {
    printf("[NETWORK] Demande historique du canal #%s (non implémenté)\n", channel);
    // TODO: envoyer paquet REQUEST_HISTORY
    return false;
}