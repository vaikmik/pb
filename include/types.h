#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>

#define ERR         (-1)
#define OK          (0)

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef i32         b32;

typedef float       f32;
typedef double      f64;

typedef size_t      usize;
typedef ssize_t     isize;

#endif
