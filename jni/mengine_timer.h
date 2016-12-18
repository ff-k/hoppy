#include <time.h>

typedef struct timer{
#if defined(__aarch64__)
	u64 LastCounter;
	u32 Frequency;
#else
//	NOTE(furkan) : This block can be used temporarily to compile.
//	Those lines will probably work but, using processor-specific
//	instructions are preferable.

	struct timespec LastCounter;
#endif
} timer;

#if defined(__aarch64__)
static u64
#else
static struct timespec
#endif
/*	TODO(furkan) : Find a better name for this since
	it does not return current time at all 
*/
GetCurrentTime(){
#if defined(__aarch64__)
	u64 Result;
	asm volatile("mrs %0, cntvct_el0" : "=r"(Result));
#else
	/* TODO(furkan) : Should return u64 from here, too */
	struct timespec Result;
	if(clock_gettime(CLOCK_REALTIME, &Result)){
		Error("clock_gettime() failed!");
		Result.tv_sec = -1;
		Result.tv_nsec = -1;
	}
#endif
#if 0
	/* TODO(furkan) :  on ARMv7, instructions below will probably work.
		Try that later
	*/
	u32 ccntr;
	u32 useren;
	u32 cntenset;
	asm volatile("mrc p15, 0, %0, c9, c14, 0" : "=r"(useren));
	if(useren & 1){
		asm volatile("mrc p15, 0, %0, c9, c12, 1" : "=r"(cntenset));
		if(cntenset & 0x80000000ul){
			asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(ccntr));
			Error("mrc %d", ccntr);
		}
	}
#endif
	return Result;
}

static u32
GetProcessorFrequency(){
#if defined(__aarch64__)
	u32 Result;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(Result));
	return Result;
#else
#error Not implemented GetProcessorFrequency for this target yet
#endif
	return -1;
}

struct timespec StartupTime;
static u32
ElapsedTimeSinceStartup(){
	/*	NOTE(furkan) : Returns elapse time since the beginning
		of the game in seconds	
	*/
	u32 Result = 0;

	struct timespec Now;
	if(clock_gettime(CLOCK_REALTIME, &Now)){
		Error("clock_gettime() failed!");
	} else {
		Result = (Now.tv_sec - StartupTime.tv_sec);
	}

	return Result;
}

static void InitialiseTimer(timer * Timer){
	Timer->LastCounter = GetCurrentTime();
	Timer->Frequency = GetProcessorFrequency();

	if(StartupTime.tv_sec == 0){
		if(clock_gettime(CLOCK_REALTIME, &StartupTime)){
			Error("clock_gettime() failed!");
			StartupTime.tv_nsec = StartupTime.tv_sec = 0;
		}
	}
}
