/* Standard Library Headers */
	/*	TODO(furkan) : 
		- Remove standard library headers and use your own version
	*/
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

#define Minimum(A,B) ((A) < (B)) ? (A) : (B)
