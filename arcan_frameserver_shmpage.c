#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h> 
#include <string.h>
#include <pthread.h>

#include "arcan_math.h"
#include "arcan_general.h"
#include "arcan_event.h"
#include "frameserver/arcan_frameserver.h"
#include "arcan_frameserver_shmpage.h"

/* This little function tries to get around all the insane problems
 * that occur with the fundamentally broken sem_timedwait with named 
 * semaphores and a parent<->child circular dependency (like we have here).
 * 
 * Sleep a fixed amount of seconds, wake up and check if parent is alive.
 * If that's true, go back to sleep -- otherwise -- wake up, pop open all semaphores
 * set the disengage flag and go back to a longer sleep that it shouldn't wake up from.
 * Show this sleep be engaged anyhow, shut down forcefully. */

struct guard_struct {
	sem_handle semset[3];
	bool* dms; /* dead man's switch */
};
static void* guard_thread(void* gstruct);

void spawn_guardthread(struct guard_struct gs)
{
	struct guard_struct* hgs = (struct guard_struct*) malloc(sizeof(struct guard_struct));
	*hgs = gs;

	pthread_t pth;
	pthread_attr_t pthattr;
	pthread_attr_init(&pthattr);
	pthread_attr_setdetachstate(&pthattr, PTHREAD_CREATE_DETACHED);
//	pthread_attr_setstacksize(&pthattr, PTHREAD_STACK_MIN);
	
	pthread_create(&pth, &pthattr, (void*) guard_thread, (void*) hgs);
}

/* Dislike pulling stunts like this,
 * but it saved a lot of bad codepaths */
#if _WIN32
extern sem_handle async, vsync, esync;
extern HANDLE parent;

static inline bool parent_alive()
{
/* based on the idea that init inherits an orphaned process,
 * wouldn't surprise me if some GNUish environment would break this .. */
	return IsWindow(parent);
}

/* force_unlink isn't used here as the semaphores are passed as inherited handles */
struct frameserver_shmcont frameserver_getshm(const char* shmkey, bool force_unlink){
	struct frameserver_shmcont res = {0};
	HANDLE shmh = (HANDLE) strtoul(shmkey, NULL, 10);
	
	res.addr = (struct frameserver_shmpage*) MapViewOfFile(shmh, FILE_MAP_ALL_ACCESS, 0, 0, MAX_SHMSIZE);

	if ( res.addr == NULL ) {
		arcan_warning("fatal: Couldn't map the allocated shared memory buffer (%i) => error: %i\n", shmkey, GetLastError());
		CloseHandle(shmh);
		return res;
	}

	res.asem = async;
	res.vsem = vsync;
	res.esem = esync;
	res.addr->w = 0;
	res.addr->h = 0;
	res.addr->bpp = 4;
	res.addr->vready = false;
	res.addr->aready = false;
	res.addr->channels = 0;
	res.addr->frequency = 0;
	res.addr->dms = true;
	
	struct guard_struct gs = {
		.dms = &res.addr->dms,
		.semset = { async, vsync, esync }
	};
	spawn_guardthread(gs);
	
	arcan_warning("arcan_frameserver() -- shmpage configured and filled.\n");
	return res;
}
#else 
#include <sys/mman.h>

struct frameserver_shmcont frameserver_getshm(const char* shmkey, bool force_unlink){
/* step 1, use the fd (which in turn is set up by the parent to point to a mmaped "tempfile" */
	struct frameserver_shmcont res = {0};
	
	unsigned bufsize = MAX_SHMSIZE;
	int fd = -1;

/* little hack to get around some implementations not accepting a shm_open on a named shm already
 * mapped in the same process (?!) */
	fd = shm_open(shmkey, O_RDWR, 0700);
	
	if (-1 == fd) {
		arcan_warning("arcan_frameserver(getshm) -- couldn't open keyfile (%s), reason: %s\n", shmkey, strerror(errno));
		return res;
	}

	/* map up the shared key- file */
	res.addr = (struct frameserver_shmpage*) mmap(NULL,
		bufsize,
		PROT_READ | PROT_WRITE,
		MAP_SHARED,
		fd,
	0);
	
	close(fd);
	if (force_unlink) shm_unlink(shmkey);
	
	if (res.addr == MAP_FAILED){
		arcan_warning("arcan_frameserver(getshm) -- couldn't map keyfile (%s), reason: %s\n", shmkey, strerror(errno));
		return res;
	}
	
	arcan_warning("arcan_frameserver(getshm) -- mapped to %" PRIxPTR " \n", (uintptr_t) res.addr); 

/* step 2, semaphore handles */
	char* work = strdup(shmkey);
	work[strlen(work) - 1] = 'v';
	res.vsem = sem_open(work, 0);
	if (force_unlink) sem_unlink(work);
	
	work[strlen(work) - 1] = 'a';
	res.asem = sem_open(work, 0);
	if (force_unlink) sem_unlink(work);
	
	work[strlen(work) - 1] = 'e';
	res.esem = sem_open(work, 0);	
	if (force_unlink) sem_unlink(work);
	free(work);
	
	if (res.asem == 0x0 ||
		res.esem == 0x0 ||
		res.vsem == 0x0 ){
		arcan_warning("arcan_frameserver_shmpage(getshm) -- couldn't map semaphores (basekey: %s), giving up.\n", shmkey);
		return res;  
	}

/* step 2, buffer all set-up, map it to the addr structure */
	res.addr->w = 0;
	res.addr->h = 0;
	res.addr->bpp = 4;
	res.addr->vready = false;
	res.addr->aready = false;
	res.addr->channels = 0;
	res.addr->samplerate = 0;
	res.addr->dms = true;

	struct guard_struct gs = {
		.dms = &res.addr->dms,
		.semset = { res.asem, res.vsem, res.esem }
	};
	spawn_guardthread(gs);
	
	return res;
}

static inline bool parent_alive()
{
/* based on the idea that init inherits an orphaned process,
 * wouldn't surprise me if some GNUish environment would break this .. */
	return getppid() != 1;
}
#endif

static void* guard_thread(void* gs)
{
	struct guard_struct* gstr = (struct guard_struct*) gs;
	*(gstr->dms) = true;
	
	while (true){
		if (!parent_alive()){
			*(gstr->dms) = false;
			
			for (int i = 0; i < sizeof(gstr->semset) / sizeof(gstr->semset[0]); i++)
				if (gstr->semset[i]) 
					sem_post(gstr->semset[i]);
				
			sleep(5);
			arcan_warning("frameserver::guard_thread -- couldn't shut down gracefully, exiting.\n");
			exit(1);
		}

		sleep(5);
	}

	return NULL;
}

#include <assert.h>
int frameserver_semcheck(sem_handle semaphore, int timeout){
		return arcan_sem_timedwait(semaphore, timeout);
}

bool frameserver_shmpage_integrity_check(struct frameserver_shmpage* shmp)
{
	return true;
}

void frameserver_shmpage_setevqs(struct frameserver_shmpage* dst, sem_handle esem, arcan_evctx* inq, arcan_evctx* outq, bool parent)
{
	if (parent){
		arcan_evctx* tmp = inq;
		inq = outq;
		outq = tmp;
	}
	
	inq->synch.external.shared = esem;
	inq->synch.external.killswitch = NULL; 
	inq->local = false;
	inq->eventbuf = dst->childdevq.evqueue;
	inq->front = &dst->childdevq.front;
	inq->back  = &dst->childdevq.back;
	inq->n_eventbuf = sizeof(dst->childdevq.evqueue) / sizeof(arcan_event);
	
	outq->synch.external.shared = esem;
	outq->synch.external.killswitch = NULL;
	outq->local =false;
	outq->eventbuf = dst->parentdevq.evqueue;
	outq->front = &dst->parentdevq.front;
	outq->back  = &dst->parentdevq.back;
	outq->n_eventbuf = sizeof(dst->parentdevq.evqueue) / sizeof(arcan_event);
}

void frameserver_shmpage_calcofs(struct frameserver_shmpage* shmp, void** dstvidptr, void** dstaudptr)
{
/* we want these 16-bit aligned so we'd possibly get something SSE optimized etc. */
	uintptr_t base = (uintptr_t) shmp;
	uintptr_t vidaddr = base + sizeof(struct frameserver_shmpage);
	uintptr_t audaddr;
	
	if (vidaddr % 16 != 0)
		vidaddr += 16 - (vidaddr % 16);
	
	audaddr = vidaddr + abs(shmp->w * shmp->h * shmp->bpp); 
	if (audaddr % 16 != 0)
		audaddr += 16 - (audaddr % 16);
	
	if (audaddr < base || vidaddr < base){
		*dstvidptr = *dstaudptr = NULL;
	}
	else {
		*dstvidptr = (void*) vidaddr;
		*dstaudptr = (void*) audaddr;
	}
}

bool frameserver_shmpage_resize(struct frameserver_shmcont* arg, unsigned width, unsigned height, unsigned bpp, unsigned nchan, float freq)
{
	/* need to rethink synchronization a bit before forcing a resize */
	if (arg->addr){
		arg->addr->w = width;
		arg->addr->h = height;
		arg->addr->bpp = bpp;
		arg->addr->channels = nchan;
		arg->addr->samplerate = freq;
	
		if (frameserver_shmpage_integrity_check(arg->addr)){
			arg->addr->resized = true;
			return true;
		}
	}
	
	return false;
}
