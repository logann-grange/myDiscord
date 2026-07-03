#include <stdio.h>
#include <string.h>

#include "../client/logic/include/menu_login/network.h"

int main(void) {
    char err[256] = {0};

    if (!network_connect("127.0.0.1", 8080)) {
        printf("connect: FAIL\n");
        return 1;
    }
    printf("connect: OK\n");

    // Best effort register. If user already exists, we continue with login.
    if (network_send_register("Test", "Copilot", "copilot_test", "copilot_test@example.com", "Passw0rd!", err, sizeof(err))) {
        printf("register: OK\n");
    } else {
        printf("register: %s\n", err);
    }

    memset(err, 0, sizeof(err));
    if (!network_send_login("copilot_test", "Passw0rd!", err, sizeof(err))) {
        printf("login: FAIL (%s)\n", err);
        network_disconnect();
        return 1;
    }
    printf("login: OK\n");

    network_send_message("general", "Hello from app protocol test");
    printf("send: OK\n");

    network_disconnect();
    return 0;
}
