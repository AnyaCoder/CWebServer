#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "../DataStructures/Lists/Queue.h"

//#ifdef _WIN32
//#include <windows.h>
//#include <process.h>  // Windows�߳����
// typedef HANDLE thread_t;        // Windows�߳�����
// typedef CRITICAL_SECTION mutex_t; // Windows������
// typedef CONDITION_VARIABLE cond_t; // Windows��������
//#else
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>             // Linux�߳����
typedef pthread_t thread_t;      // Linux�߳�����
typedef pthread_mutex_t mutex_t; // Linux������
typedef pthread_cond_t cond_t;   // Linux��������

//#endif

struct ThreadPool {
	int num_threads;
	int active;
	struct Queue work;
	thread_t *pool;
	mutex_t lock;
	cond_t signal;

	void (*add_work)(struct ThreadPool *thread_pool, struct ThreadJob job);
};

struct ThreadJob {
	void *(*job)(void *args);
	void *args;
};

struct ThreadPool *thread_pool_constructor(int num_threads);
struct ThreadJob thread_job_constructor(void *(*job_func)(void *args),
                                        void *args);

#endif // !THREAD_POOL_H
