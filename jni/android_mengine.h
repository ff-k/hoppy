#ifndef INCLUDED_ANDROID_MENGINE
#define INCLUDED_ANDROID_MENGINE

#include <android_native_app_glue.h>

/*	TODO(furkan) : 
		- Remove standard library headers and use your own version
*/

/* Standard Library Headers */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Type Definitions */
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef double r64;
typedef float r32;

typedef u64 b64;
typedef u32 b32;
typedef u16 b16;
typedef u8 b8;

#define true 1
#define false 0

typedef struct android_app android_app;
typedef struct android_poll_source android_poll_source;


/* Logger */
#include <android/log.h>

#define __LogTag "Mengine"
#define __LogBufferSize 256
#define __LogTempSize __LogBufferSize

static char __logBuffer[__LogBufferSize];
static char __logTemp[__LogTempSize];

#define __log(prio, ...) \
	if(snprintf(__logTemp, __LogTempSize, __VA_ARGS__) > 0){ \
		if(snprintf(__logBuffer, __LogBufferSize, "%s [%s:%d (%s)]", __logTemp, __FUNCTION__, __LINE__, __FILE__)){ \
			__android_log_print(prio, __LogTag, "%s", __logBuffer); \
		} \
	}

#define Debug(...) __log(ANDROID_LOG_DEBUG, __VA_ARGS__)
#define Error(...) __log(ANDROID_LOG_ERROR, __VA_ARGS__)
#define Warning(...) __log(ANDROID_LOG_WARN, __VA_ARGS__)


/* Other Util */
#define Assert(expr) \
	if(!(expr)){ \
		Error("Assertion failed!"); \
		*(int *) 0 = 0; \
	}

#define ZeroStruct(Struct) \
	memset(&(Struct), 0 , sizeof((Struct)))

#endif
