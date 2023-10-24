#pragma once

void _cdecl X86PrintCharacter(char Character, u8 Page);
void _cdecl X86Divide64BitsBy32Bits
(
    u64 Dividend, u32 Divisor, u64 *QuotientOut, u32 *RemainderOut
);
