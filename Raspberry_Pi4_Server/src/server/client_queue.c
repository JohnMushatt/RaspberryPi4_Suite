//
// Created by john on 6/26/20.
//

#include "client_queue.h"

void init_queue(Queue *queue) {
    queue->head=malloc(sizeof(Node ));
    queue->tail=malloc(sizeof(Node ));
    queue->head->next=queue->tail;

}
int8_t queue(Queue *queue,void *data) {
    int8_t index= 0;
    Node *ptr = queue->head;
    if(queue->size==0) {
        queue->head->data=(void *) malloc(sizeof(void *));
        queue->head->data=data;
        queue->head->next=queue->tail;
        queue->size++;
        return queue->size;
    }
    while(ptr->next!=NULL) {
        ptr=ptr->next;
        index++;
    }
    //Node *new_node = (Node *) malloc(sizeof(Node));
    ptr->data=data;
    ptr->next=NULL;
    queue->tail=ptr;
    queue->size++;
    return   queue->size;
}
void *dequeue(Queue *queue) {
    void *data = queue->head->data;
    if(queue->size==1) {
        queue->head=queue->tail;
        queue->size=0;
    }
    else {
        free(queue->head);
        queue->head = queue->head->next;
        queue->size--;
    }
    return data;
}