
#include "ThreadPool.h"

void* generic_thread_function(void* args);
void add_work(struct ThreadPool* thread_pool, struct ThreadJob job);

struct ThreadJob thread_job_constructor(void* (*job_func)(void* args), void* args)
{
    struct ThreadJob thread_job;
    thread_job.job = job_func;
    thread_job.args = args;
    return thread_job;
}

void thread_pool_destructor(struct ThreadPool* thread_pool)
{

    thread_pool->active = 0;

    for (int i = 0; i < thread_pool->num_threads; i++)
    {
        pthread_cond_signal(&thread_pool->signal);
    }

    if (thread_pool->pool)
    {
        for (int i = 0; i < thread_pool->num_threads; i++)
        {
            pthread_join(thread_pool->pool[i], NULL);
        }
        free(thread_pool->pool);
    }

    pthread_mutex_destroy(&thread_pool->lock);
    pthread_cond_destroy(&thread_pool->signal);
    queue_destructor(&thread_pool->work);
}

void* generic_thread_function(void* args)
{
    struct ThreadPool* thread_pool = (struct ThreadPool*)args;
    while (thread_pool->active == 1)
    {
        pthread_mutex_lock(&thread_pool->lock);
        pthread_cond_wait(&thread_pool->signal, &thread_pool->lock);
        struct ThreadJob thread_job = *(struct ThreadJob*)thread_pool->work.front(&thread_pool->work);
        thread_pool->work.pop(&thread_pool->work);
        pthread_mutex_unlock(&thread_pool->lock);

        if (thread_job.job)
        {
            thread_job.job(thread_job.args);
        }
    }
    return NULL;
}

struct ThreadPool* thread_pool_constructor(int num_threads)
{
    struct ThreadPool* thread_pool = (struct ThreadPool*)malloc(sizeof(struct ThreadPool));
    thread_pool->num_threads = num_threads;
    thread_pool->active = 1;

    pthread_mutex_init(&thread_pool->lock, NULL);
    pthread_cond_init(&thread_pool->signal, NULL);

    thread_pool->pool = (thread_t*)malloc(sizeof(thread_t) * num_threads);
    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(
            &thread_pool->pool[i],
            NULL,
            generic_thread_function,
            thread_pool
        );
    }

    thread_pool->work = queue_constructor();
    thread_pool->add_work = add_work;

    return thread_pool;
}

void add_work(struct ThreadPool* thread_pool, struct ThreadJob thread_job)
{
    pthread_mutex_lock(&thread_pool->lock);
    thread_pool->work.push(&thread_pool->work, &thread_job, sizeof(thread_job));
    pthread_cond_signal(&thread_pool->signal);
    pthread_mutex_unlock(&thread_pool->lock);
}