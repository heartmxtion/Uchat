#include "../inc/server.h"

int mx_create_socket() {
    int opt = 1;
    int server_fd;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
        &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

void mx_socket_bind(int server_fd, char *ip, int port) {
    struct sockaddr_in address;
    if (port) {}

    mx_memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // struct sockaddr_in address;

    // mx_memset(&address, 0, sizeof(address));

    // address.sin_family = AF_INET;
    // address.sin_addr.s_addr = inet_addr(ip);
    // address.sin_port = htons(port);

    // if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    //     perror("bind failed");
    //     exit(EXIT_FAILURE);
    // }
}

int mx_socket_accept(int server_fd) {
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
        (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    return new_socket;
}

void mx_socket_listen(int server_fd, KeyPair server_key_pair) {
    pthread_t threads[MAX_THREAD];
    Thread_data *threads_d = (Thread_data *)malloc(sizeof(Thread_data) * MAX_THREAD);
    // Thread_data threads_d[MAX_THREAD];
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_THREAD; i++) {
        threads_d[i].client_public_key = 0;
        threads_d[i].socket = 0;
        threads_d[i].thread_work = false;
        threads_d[i].thread_data = &threads_d;
        threads_d[i].server_key_pair = &server_key_pair;
    }

    while (1) {
        int new_conn = mx_socket_accept(server_fd);

        if (new_conn) {
            int slot = -1;
            for (int i = 0; i < MAX_THREAD; i++) {
                if (!threads_d[i].thread_work) {
                    slot = i;
                    break;
                }
            }
            if (slot < 0) send(new_conn, "bf", 2, 0);
            else {
                threads_d[slot].socket = new_conn;
                pthread_create(&threads[slot], NULL, foo, &(threads_d[slot]));
            }
        }
    }
    free(threads_d);
}

