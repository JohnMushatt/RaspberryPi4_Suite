//
// Created by john on 6/26/20.
//

#include "rpi_server.h"

void start_server(void) {
    init_queue(&server.server_queue);
    // Creating socket file descriptor
    if ((server.fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    server.server_addr.sin_family = AF_INET;
#ifdef _PI
    printf("Pi4 build detected...\n");
    server.server_addr.sin_addr.s_addr = inet_addr("192.168.1.20");
#else
    printf("x86_64 build detected...\n");
    server.server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif
    server.server_addr.sin_port = htons(PORT);

    memset(server.server_addr.sin_zero, '\0', sizeof(server.server_addr.sin_zero));
    signal(SIGPIPE, SIG_IGN);
    int64_t reuse = 1;

    if (setsockopt(server.fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
    }
#ifdef SO_REUSEADDR
    if (setsockopt(server.fd, SOL_SOCKET, SO_REUSEPORT, (const char *) &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(SO_REUSEPORT) failed");
    }
#endif

    if (bind(server.fd, (struct sockaddr *) &server.server_addr,
             sizeof(server.server_addr)) < 0) {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server.fd, 20) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    printf("Succesfully binded server to %s:%ld\nListening for TM4C1294XL tcp connection...\n",
           inet_ntoa(server.server_addr.sin_addr), (int64_t) ntohs(server.server_addr.sin_port));
    struct sockaddr_in client_addr;
    socklen_t client_size = sizeof(client_addr);
    int64_t incoming_socket;
    while (1) {
        if ((incoming_socket = accept(server.fd, (struct sockaddr *) &(client_addr), (socklen_t *) &client_size)) < 0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        Connection *client = (Connection *) malloc(sizeof(Connection));
        client->addr=client_addr;
        client->connection_file_descriptor = incoming_socket;
        printf("Client with ip %s connected...\n",inet_ntoa(client->addr.sin_addr));
        queue_client(client);
        close(client->connection_file_descriptor);
        sleep(1);
    }
}

int8_t queue_client(Connection *client) {
    int8_t ret = queue(&server.server_queue, (void *) client);
    return ret;
}

Connection *dequeue_client(void) {
    Connection *client = (Connection *) dequeue(&server.server_queue);
    return client;
}

char *ip_to_string(struct in_addr addr, char *buffer) {
    //inet_ntop(AF_INET,addr,buffer,INET_ADDRSTRLEN);
    return "";
}
