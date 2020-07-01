//
// Created by john on 6/26/20.
//

#ifndef EK_TM4C1294XL_TEMPERATURE_CLIENT_RPI_SERVER_H
#define EK_TM4C1294XL_TEMPERATURE_CLIENT_RPI_SERVER_H
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdatomic.h>
#include <stdint.h>

#include <stdbool.h>
#include "client_queue.h"
#define PORT 8080
typedef struct _connection {
    struct sockaddr_in addr;
    int64_t connection_file_descriptor;
    int64_t id;
} Connection;
typedef struct _server {
    uint8_t message_buffer[1024];
    struct sockaddr_in server_addr;
    Queue server_queue;
    int64_t fd;
} Server;
Server server;
bool pi_mode;
void start_server(void);
char *ip_to_string(struct in_addr addr,char *buffer);
void *connect_client(void *arg);
int8_t queue_client(Connection *client);
Connection *dequeue_client(void);
#endif //EK_TM4C1294XL_TEMPERATURE_CLIENT_RPI_SERVER_H
