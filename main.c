gcc client/graphic/src/**/*.c client/logic/src/**/*.c \
    $(pkg-config --cflags --libs gtk+-3.0) \
    -lssl -lcrypto -lpthread \
    -o client_app


    gcc server/src/*.c db/src/*.c utils/src/*.c \
    $(pkg-config --cflags --libs libpq) \
    -lssl -lcrypto -lpthread \
    -o server_app