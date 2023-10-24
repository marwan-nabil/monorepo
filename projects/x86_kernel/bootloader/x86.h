#pragma once

void _cdecl X86PrintCharacter(char Character, u8 Page);
void _cdecl X86DivideU64ByU32
(
    u64 Dividend, u32 Divisor, u64 *QuotientOut, u32 *RemainderOut
);