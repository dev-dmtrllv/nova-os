#pragma once

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;


typedef char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef long int int64_t;

#ifdef x86
typedef uint32_t size_t;
#elif x86_64
typedef uint64_t size_t;
#endif

typedef unsigned long uintptr_t;
