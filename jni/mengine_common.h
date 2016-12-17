/* Standard Library Headers */
	/*	TODO(furkan) : 
		- Remove standard library headers and use your own version
	*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/* Type Definitions */
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef size_t memsz;

typedef double r64;
typedef float r32;

typedef u64 b64;
typedef u32 b32;
typedef u16 b16;
typedef u8 b8;

#define true 1
#define false 0

#define Gigabytes(val) (val)*1024*1024*1024
#define Megabytes(val) (val)*1024*1024
#define Kilobytes(val) (val)*1024

/* Utils */
#define ZeroStruct(Struct) \
	memset(&(Struct), 0 , sizeof((Struct)))

#ifdef __unix__
#include <sys/types.h>
#define GetThreadID() gettid()
#else
#error GetThreadID is not defined!
#endif


/* Stack Trace Printer */
#define MENGINE_DEBUG 1
#define StackTraceCapacity 256
#define StackTraceFunctionCapacity 128
char StackTrace[StackTraceCapacity][StackTraceFunctionCapacity];
int StackTraceCount = 0;
int __StackTraceLine = 0;
#define UpdateStackTraceLine __StackTraceLine = __LINE__

#define BeginStackTraceBlock \
	strncpy(StackTrace[StackTraceCount++], __FUNCTION__, StackTraceFunctionCapacity)

#define EndStackTraceBlock \
	StackTraceCount--

#define PrintStackTrace \
	{ \
		int StackIndex; \
		for(StackIndex=0; StackIndex<StackTraceCount; StackIndex++){ \
			Error("%s (Thread : %d)", StackTrace[StackIndex], GetThreadID()); \
		} \
		Error("Line : %d", __StackTraceLine)\
	}
