#!/usr/bin/env python3
import socket
HOST = '0.0.0.0'
PORT = 8080

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))
    s.listen(5)
    print(f'Echo server listening on {HOST}:{PORT}')
    try:
        while True:
            conn, addr = s.accept()
            print('Connection from', addr)
            with conn:
                while True:
                    data = conn.recv(1024)
                    if not data:
                        break
                    conn.sendall(data)
            print('Closed', addr)
    except KeyboardInterrupt:
        print('Server stopped')
