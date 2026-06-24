#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
    #define closesocket close
#endif

int main(int argc, char *argv[]) {
    SOCKET sock;
    struct sockaddr_in serverAddr;
    char buffer[1024];
    char message[1024];
    const char *serverIP = "10.10.6.228";

    //Permet de passer l'IP en argument :
    if (argc > 1) {
        serverIP = argv[1];
    }

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Erreur WSAStartup\n");
        return 1;
    }
#endif

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Erreur socket\n");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    if (inet_pton(AF_INET, serverIP, &serverAddr.sin_addr) != 1) {
        printf("Adresse IP invalide: %s\n", serverIP);
        closesocket(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Erreur connexion\n");
        closesocket(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    printf("Connecte au serveur %s:8080. Tapez /quit pour fermer la conversation.\n", serverIP);

    while (1) {
        printf("Vous: ");
        fflush(stdout);
        if (fgets(message, sizeof(message), stdin) == NULL) {
            break;
        }
        message[strcspn(message, "\n")] = '\0';

        send(sock, message, (int)strlen(message), 0);

        if (strcmp(message, "/quit") == 0) {
            break;
        }

        int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            printf("Serveur deconnecte.\n");
            break;
        }
        buffer[bytesReceived] = '\0';
        printf("Serveur: %s\n", buffer);

        if (strcmp(buffer, "/quit") == 0) {
            break;
        }
    }

    closesocket(sock);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}