//
// Created by john on 7/1/20.
//

#ifndef EK_TM4C1294XL_TEMPERATURE_CLIENT_THREAD_POOL_H
#define EK_TM4C1294XL_TEMPERATURE_CLIENT_THREAD_POOL_H
#include <stdbool.h>
#include <stddef.h>

typedef void (*thread_func_t)(void *arg);
/**
 * func - function to run on thread worker
 * arg - argument for passed function
 */
typedef struct st_thread_unit {
    thread_func_t func;
    void *arg;
    struct st_thread_unit *next;
} thread_unit_t;
/***
 *
 */
typedef struct st_thread_pool {
    thread_unit_t *work_head;
    thread_unit_t *work_tail;
    pthread_mutex_t working_mutex;
    pthread_cond_t work_conditional;
    pthread_cond_t working_conditional;
    size_t working_cnt;
    size_t thread_cnt;
    bool  running;
} thread_pool_t;
/***
 * Create a unit of work to be added to the queue
 * @param func Function for work to be done
 * @param arguments Arguments for function
 * @return Unit of work
 */
static thread_unit_t *thread_unit_create(thread_func_t func, void *arguments);
/***
 * Free memory associated with a work unit
 * @param unit Unit object to free resources from
 */
static void thread_unit_destroy(thread_unit_t *unit);
/***
 * Gets the next available piece of work from the queue and updates the queue
 * @param pool Work pool to get work from
 * @return thread_unit_t object containing work
 */
static thread_unit_t *thread_pool_get_unit(thread_pool_t *pool);
static void
#endif //EK_TM4C1294XL_TEMPERATURE_CLIENT_THREAD_POOL_H
