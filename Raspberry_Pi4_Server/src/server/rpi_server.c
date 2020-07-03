//
// Created by john on 6/26/20.
//

#include "rpi_server.h"
#include "thread_pool.h"

#define BUFFER_LENGTH 1024


typedef struct st_connection {
    struct sockaddr_in addr;
    int64_t connection_file_descriptor;
    int64_t id;
} Connection;

typedef struct st_server {
    thread_pool_t *thread_pool;
    uint8_t message_buffer[1024];
    struct sockaddr_in server_addr;
    Queue server_queue;
    int64_t fd;
} Server;
Server server;
void client_job(void *argument);
void start_server(void) {
    server.thread_pool = thread_pool_create(0);
    // Creating socket file descriptor
    if ((server.fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    server.server_addr.sin_family = AF_INET;
#ifdef _PI
    printf("Pi4 build detected...\n");
    server.server_addr.sin_addr.s_addr = inet_addr("192.168.1.25");
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
        }
        Connection *client = (Connection *) malloc(sizeof(Connection));
        client->addr = client_addr;
        client->connection_file_descriptor = incoming_socket;
        printf("Client with ip %s connected...\n", inet_ntoa(client->addr.sin_addr));
        thread_pool_add_unit(server.thread_pool,&client_job,(void *)client);
        thread_pool_wait(server.thread_pool);
        sleep(1);
    }
}
void client_job(void *argument) {
    char buffer[BUFFER_LENGTH];
    Connection *client = (Connection *) argument;
    printf("Thread %lu is handling client from %s\n",pthread_self(),inet_ntoa(client->addr.sin_addr));
    int64_t status;
    while(1) {
        status = read(client->connection_file_descriptor,buffer,BUFFER_LENGTH);
        if(status <0) {
            perror("in read: negative return");
            break;
        }
        printf("Device message: %1024s\n",buffer);
        memset(buffer,0,BUFFER_LENGTH);
    }
}


