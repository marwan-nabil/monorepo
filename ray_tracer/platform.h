#pragma once

typedef uint8_t b8;
typedef uint32_t b32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

#ifndef SIMD_NUMBEROF_LANES
#define SIMD_NUMBEROF_LANES 4
#endif

#define ENABLE_ASSERTIONS 0