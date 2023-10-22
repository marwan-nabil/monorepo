#pragma once

void _cdecl x86_PrintCharacter(char Character, u8 Page);
void _cdecl x86_Divide64By32
(
    u64 Divident, u32 Divisor, u64 *QuotientOut, u32 *RemainderOut
);