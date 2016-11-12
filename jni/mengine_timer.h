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
GetCurrentTime(){
#if defined(__aarch64__)
	u64 Result;
	asm volatile("mrs %0, cntvct_el0" : "=r"(Result));
#else
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
