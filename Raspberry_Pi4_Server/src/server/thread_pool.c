//
// Created by john on 7/1/20.
//
#include "thread_pool.h"
#include <stdlib.h>
#include <stdio.h>


/**
 * @brief Struct containing a function pointer, argument pointer, and the next node in the thread pool work list
 * @struct st_thread_unit Represents a node in a linked list of workable objects within a thread pool
 * @field thread_func_t func - Function pointer to run on when a thread grabs this unit
 * @field void  *arg - Argument pointer to data for thread to execute with function
 */
typedef struct st_thread_unit {
    thread_func_t func;
    void *arg;
    struct st_thread_unit *next;
} thread_unit_t;
/**
 * @brief Struct containing head and tail pointers to thread_unit_t objects, mutex, 2 conditionals, working and thread counts,
 * and running status
 * @struct st_thread_pool Representsa collection of workable objects in a linked-list FIFO queue that allows a pool of threasds
 * to retrieve jobs/units of work to be completed
 * @param thread_unit_t *work_head Head of job list
 * @param thread_unit_t *work_tail Tail of job list
 * @param pthread_mutex_t working_mutex Mutex for threads to retrieve units
 * @param pthread_cond_t work_conditional Conditional variable for signaling if there is still work remaining in the list
 * @param pthread_con_t working_conditional Conditional variable for signaling whether there are still threads actively working
 * @param size_t working_cnt Number of actively working threads
 * @param size_t thread_cnt Number of active threads remaining in the pool
 * @param bool running Boolean representing whether the thread pool is actively running
 */
typedef struct st_thread_pool {
    thread_unit_t *work_head;
    thread_unit_t *work_tail;
    pthread_mutex_t working_mutex;
    pthread_cond_t work_conditional;
    pthread_cond_t working_conditional;
    size_t working_cnt;
    size_t thread_cnt;
    bool running;
} thread_pool_t;

uint64_t get_pc_thread_count(void) {
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#else

#include <unistd.h>

    return (uint64_t) sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

/***
 * @brief Create a unit of work to be added to the queue
 * @param func Function for work to be done
 * @param arguments Arguments for function
 * @return Unit of work
 */
static thread_unit_t *thread_unit_create(thread_func_t func, void *arguments) {
    thread_unit_t *work;

    if (func == NULL) {
        return NULL;
    }
    work = malloc(sizeof(*work));
    work->func = func;
    work->arg = arguments;
    work->next = NULL;

    return work;
}

/***
 * Free memory associated with a work unit
 * @param unit Unit object to free resources from
 */
static void thread_unit_destroy(thread_unit_t *unit) {
    if (unit == NULL) {
        return;
    }
    free(unit);
}
/***
 * Gets the next available piece of work from the queue and updates the queue
 * @param pool Work pool to get work from
 * @return thread_unit_t object containing work
 */
static thread_unit_t *thread_pool_get_unit(thread_pool_t *pool) {
    thread_unit_t *unit;

    if (pool == NULL) {
        return NULL;
    }
    unit = pool->work_head;
    if (unit == NULL) {
        return NULL;
    }
    if (unit->next == NULL) {
        pool->work_head = NULL;
        pool->work_tail = NULL;
    } else {
        pool->work_head = unit->next;
    }

    return unit;
}

/***
 * Work function for thread_unit_t to use when receiving work
 * @param arg Arguments to pass into function
 */
static void *thread_unit_work(void *arg) {

    thread_pool_t *thread_pool = (thread_pool_t *) arg;

    thread_unit_t *unit;
    while (1) {
        pthread_mutex_lock(&thread_pool->working_mutex);

        while (thread_pool->work_head == NULL && (thread_pool->running)) {
            pthread_cond_wait(&thread_pool->work_conditional, &thread_pool->working_mutex);
        }
        if (!thread_pool->running) {
            break;
        }
        unit = thread_pool_get_unit(thread_pool);
        thread_pool->working_cnt++;
        pthread_mutex_unlock(&thread_pool->working_mutex);
        if (unit != NULL) {
            unit->func(unit->arg);
            thread_unit_destroy(unit);
        }
        pthread_mutex_lock(&thread_pool->working_mutex);
        thread_pool->working_cnt--;
        if (thread_pool->running && thread_pool->working_cnt == 0 && thread_pool->work_head == NULL) {
            pthread_cond_signal(&thread_pool->working_conditional);
        }
        pthread_mutex_unlock(&thread_pool->working_mutex);
    }
    thread_pool->thread_cnt--;
    pthread_cond_signal(&thread_pool->working_conditional);
    pthread_mutex_unlock(&thread_pool->working_mutex);
    return NULL;
}

thread_pool_t *thread_pool_create(size_t num_threads) {
    thread_pool_t *thread_pool = (thread_pool_t *) malloc(sizeof(thread_pool_t ));
    if (num_threads != 0) {
        thread_pool->thread_cnt = num_threads;
    }
    thread_pool->thread_cnt = get_pc_thread_count();
    printf("Thread pool created with %zu threads\n",thread_pool->thread_cnt);
    pthread_mutex_init(&thread_pool->working_mutex, NULL);
    pthread_cond_init(&thread_pool->working_conditional, NULL);
    pthread_cond_init(&thread_pool->work_conditional, NULL);


    thread_pool->work_head = NULL;
    thread_pool->work_tail = NULL;
    thread_pool->running=true;
    pthread_t id;
    for (size_t i = 0; i < thread_pool->thread_cnt; i++) {
        pthread_create(&id, NULL, thread_unit_work, (void *) thread_pool);
        pthread_detach(id);
    }
    return thread_pool;


}

void thread_pool_wait(thread_pool_t *thread_pool) {
    if(thread_pool==NULL) {
        return;
    }
    pthread_mutex_lock(&thread_pool->working_mutex);
    while(1) {
        if((thread_pool->running && thread_pool->working_cnt!=0) || (!thread_pool->running && thread_pool->thread_cnt!=0)) {
            pthread_cond_wait(&thread_pool->working_conditional,&thread_pool->working_mutex);
        }
        else {
            break;
        }
    }
    pthread_mutex_unlock(&thread_pool->working_mutex);
}

void thread_pool_destroy(thread_pool_t *thread_pool) {
    thread_unit_t *head, *tail;

    if (thread_pool == NULL) {
        return;
    }
    pthread_mutex_lock(&thread_pool->working_mutex);

    head = thread_pool->work_head;
    while (head != NULL) {
        tail = head->next;
        thread_unit_destroy(head);
        head = tail;
    }
    thread_pool->running = false;
    pthread_cond_broadcast(&thread_pool->work_conditional);
    pthread_mutex_unlock(&thread_pool->working_mutex);
    thread_pool_wait(thread_pool);
    pthread_mutex_destroy(&thread_pool->working_mutex);
    pthread_cond_destroy(&thread_pool->work_conditional);
    pthread_cond_destroy(&thread_pool->working_conditional);

    free(thread_pool);
}

bool thread_pool_add_unit(thread_pool_t *thread_pool, thread_func_t func, void *arg) {
    thread_unit_t *unit;

    if(thread_pool==NULL) {
        return false;
    }

    unit = thread_unit_create(func,arg);
    if(unit==NULL){
        return false;
    }
    pthread_mutex_lock(&thread_pool->working_mutex);
    if(thread_pool->work_head==NULL) {
        thread_pool->work_head = unit;
        thread_pool->work_tail = thread_pool->work_head;
    }
    else {
        thread_pool->work_tail->next= unit;
        thread_pool->work_tail = unit;
    }
    pthread_cond_broadcast(&thread_pool->work_conditional);
    pthread_mutex_unlock(&thread_pool->working_mutex);

    return true;
}