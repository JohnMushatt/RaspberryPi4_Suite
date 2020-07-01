//
// Created by john on 7/1/20.
//
#include <stdlib.h>
#include "thread_pool.h"
static thread_unit_t *thread_unit_create(thread_func_t func, void *arguments) {
    thread_unit_t *work;

    if(func==NULL) {
        return NULL;
    }
    work = malloc(sizeof(*work));
    work->func = func;
    work->arg = arguments;
    work->next = NULL;

    return work;
}
static void thread_unit_destroy(thread_unit_t *unit) {
    if(unit==NULL) {
        return;
    }
    free(unit);
}

static thread_unit_t *thread_pool_get_unit(thread_pool_t *pool) {
    thread_unit_t *unit;

    if(pool==NULL) {
        return NULL;
    }
    unit = pool->work_head;
    if(unit==NULL) {
        return NULL;
    }
    if(unit->next==NULL) {
        pool->work_head=NULL;
        pool->work_tail=NULL;
    }
    else {
        pool->work_head=unit->next;
    }

    return unit;
}

