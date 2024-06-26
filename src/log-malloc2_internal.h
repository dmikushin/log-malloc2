#ifndef LOG_MALLOC2_INTERNAL_H
#define LOG_MALLOC2_INTERNAL_H
/*
 * log-malloc2 API
 *	Malloc logging library with backtrace and byte-exact memory tracking.
 * 
 * Author: Samuel Behan <_samuel_._behan_(at)_dob_._sk> (C) 2013-2015
 *
 * License: GNU LGPLv3 (http://www.gnu.org/licenses/lgpl.html)
 *
 * Web:
 *	http://devel.dob.sk/log-malloc2
 *	http://blog.dob.sk/category/devel/log-malloc2 (howto, tutorials)
 *	https://github.com/samsk/log-malloc2 (git repo)
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include <stdbool.h>

/* pthreads support */
#include <pthread.h>

/* pthread shortcuts */
# define LOCK_INIT()	1
# define LOCK(lock)	(pthread_mutex_trylock(&(lock)) == 0)
# define UNLOCK(lock)	(pthread_mutex_unlock(&(lock)))

/* init constants */
#define LOG_MALLOC_INIT_NULL		0xFAB321
#define LOG_MALLOC_INIT_DONE		0x123FAB
#define LOG_MALLOC_FINI_DONE		0xFAFBFC

/* global context */
typedef struct log_malloc_ctx_s {
	sig_atomic_t init_done;
        sig_atomic_t mem_used;
	sig_atomic_t mem_rused;
	struct {
		sig_atomic_t malloc;
		sig_atomic_t calloc;
		sig_atomic_t realloc;
		sig_atomic_t memalign;
		sig_atomic_t posix_memalign;
		sig_atomic_t valloc;
		sig_atomic_t free;
		sig_atomic_t unrel_sum; /* unrealiable call count sum */
	} stat;
	int memlog_fd;
	int statm_fd;
	bool memlog_disabled;
	clock_t clock_start;
	pthread_mutex_t loglock;
} log_malloc_ctx_t;

#define LOG_MALLOC_CTX_INIT_BASE		\
		LOG_MALLOC_INIT_NULL,		\
		0,				\
		0,				\
		{0, 0, 0, 0, 0, 0, 0, 0},	\
		LOG_MALLOC_TRACE_FD,		\
		-1,				\
		false,				\
		0

#define LOG_MALLOC_CTX_INIT			\
	{					\
		LOG_MALLOC_CTX_INIT_BASE,	\
		PTHREAD_MUTEX_INITIALIZER,	\
	}

/* API function */
log_malloc_ctx_t *log_malloc_ctx_get(void);

#endif
