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

#define Assert(Expression) {if(!(Expression)){ *(int *)0 = 0; }}
#define AbsoluteValue(X) (((X) < 0)?-(X):(X))
#define Assert(Expression) {if(!(Expression)){ *(int *)0 = 0; }}
#define AssertIsBit(Value) Assert(!((Value) & (~1ull)))
#define AssertFits(Value, FittingMask) Assert(!((Value) & (~(FittingMask))))
#define ArrayLength(Array) (sizeof(Array) / sizeof((Array)[0]))
