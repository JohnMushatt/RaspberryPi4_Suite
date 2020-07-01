//
// Created by john on 6/26/20.
//

#ifndef EK_TM4C1294XL_TEMPERATURE_CLIENT_CLIENT_QUEUE_H
#define EK_TM4C1294XL_TEMPERATURE_CLIENT_CLIENT_QUEUE_H
#include <stdint.h>
#include <stdlib.h>
typedef struct _node {
    void *data;
    struct _node *next;
} Node;
typedef struct _queue {
    Node *head;
    Node *tail;
    int64_t size;
} Queue;
void init_queue(Queue *queue);
int8_t queue(Queue *queue,void *data);
void *dequeue(Queue *queue);
int64_t get_queue_size(Queue *queue);
#endif //EK_TM4C1294XL_TEMPERATURE_CLIENT_CLIENT_QUEUE_H
