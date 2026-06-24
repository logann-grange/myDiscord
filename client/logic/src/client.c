#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
    #define THREAD_RET DWORD WINAPI
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <pthread.h>
    #define SOCKET int
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
    #define closesocket close
    #define THREAD_RET void*
#endif

SOCKET sock;
volatile int running = 1;

// Thread dedie a la reception :
THREAD_RET receiveMessages(void *arg) {
    char buffer[1024];
    int bytesReceived;

    while (running) {
        bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            if (running) {
                printf("\nServeur deconnecte.\n");
            }
            running = 0;
            break;
        }
        buffer[bytesReceived] = '\0';
        printf("\nServeur: %s\nVous: ", buffer);
        fflush(stdout);

        if (strcmp(buffer, "/quit") == 0) {
            running = 0;
            break;
        }
    }
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

int main(int argc, char *argv[]) {
    struct sockaddr_in serverAddr;
    char message[1024];
    const char *serverIP = "10.10.6.228";

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

    // Lancement du thread de reception en arriere-plan
#ifdef _WIN32
    HANDLE recvThread = CreateThread(NULL, 0, receiveMessages, NULL, 0, NULL);
    if (recvThread == NULL) {
        printf("Erreur creation thread\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }
#else
    pthread_t recvThread;
    if (pthread_create(&recvThread, NULL, receiveMessages, NULL) != 0) {
        printf("Erreur creation thread\n");
        closesocket(sock);
        return 1;
    }
#endif

    // Le thread principal s'occupe de l'envoi
    printf("Vous: ");
    fflush(stdout);
    while (running) {
        if (fgets(message, sizeof(message), stdin) == NULL) {
            running = 0;
            break;
        }
        message[strcspn(message, "\n")] = '\0';

        if (send(sock, message, (int)strlen(message), 0) == SOCKET_ERROR) {
            running = 0;
            break;
        }

        if (strcmp(message, "/quit") == 0) {
            running = 0;
            break;
        }

        if (running) {
            printf("Vous: ");
            fflush(stdout);
        }
    }

    closesocket(sock); // debloque le thread de reception s'il attend un recv()

#ifdef _WIN32
    WaitForSingleObject(recvThread, 2000);
    CloseHandle(recvThread);
#else
    pthread_join(recvThread, NULL);
#endif

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}