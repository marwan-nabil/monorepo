#pragma once

typedef struct 
{
    u16 LimitLowPart;
    u16 BaseLowPart;
    u8 BaseMiddlePart;
    u8 AccessFlags;
    u8 LimitHighAndFlags;
    u8 BaseHigh;
} __attribute__((packed)) gdt_entry;