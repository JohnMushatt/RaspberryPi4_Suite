//
// Created by john on 7/1/20.
//

#ifndef EK_TM4C1294XL_TEMPERATURE_CLIENT_THREAD_POOL_H
#define EK_TM4C1294XL_TEMPERATURE_CLIENT_THREAD_POOL_H

#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>
#include <stdint.h>

typedef void (*thread_func_t)(void *arg);



/***
 *
 */
typedef struct st_thread_pool thread_pool_t ;

/***
 * Gets the number of cores available on the target system
 * @return 64-bit integer representing the number of available cores/processors on the target system
 */
uint64_t get_pc_thread_count(void);


/***
 * Creates a thread_pool_t object
 * @param num_threads Sets thread pool size to num_threads, if 0 it uses the system number of cores
 * @return thread_pool_t object
 */
thread_pool_t *thread_pool_create(size_t num_threads);

/***
 * Waits for the remaining threads to finish their work before returning
 * @param thread_pool thread_pool_t object to wait on
 */
void thread_pool_wait(thread_pool_t *thread_pool);

/***
 * Frees memory allocated to designated thread_pool_t object
 * @param thread_pool thread_pool_t object to release allocated memory from
 */
void thread_pool_destroy(thread_pool_t *thread_pool);

bool thread_pool_add_unit(thread_pool_t *thread_pool, thread_func_t func, void *arg);

#endif //EK_TM4C1294XL_TEMPERATURE_CLIENT_THREAD_POOL_H
