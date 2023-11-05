#pragma once

extern void _cdecl X86_DivideU64ByU32
(
    u64 Dividend, u32 Divisor, u64 *QuotientOut, u32 *RemainderOut
);