#pragma once

#define UINT32_MAX 2147483647

extern void _cdecl DivideU64ByU32
(
    u64 Dividend, u32 Divisor, u64 *QuotientOut, u32 *RemainderOut
);