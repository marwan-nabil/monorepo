#pragma once

typedef struct
{
    u64 EntryAddressLow: 16;
    u64 SegmentSelector: 16;
    u64 Reserved: 8;
    u64 Flags: 8;
    u64 EntryAddressHigh: 16;
} __attribute__((packed)) idt_entry;

typedef struct
{
    u16 Limit;
    idt_entry *FirstEntry;
} __attribute__((packed)) idt_descriptor;

typedef enum
{
    IDTFFO_GATE_TYPE_BIT0 = 0,
    IDTFFO_GATE_TYPE_BIT1 = 1,
    IDTFFO_GATE_TYPE_BIT2 = 2,
    IDTFFO_GATE_TYPE_BIT3 = 3,
    IDTFFO_ZEROED_BIT = 4,
    IDTFFO_PRIVILIGE_LEVEL_BIT0 = 5,
    IDTFFO_PRIVILIGE_LEVEL_BIT1 = 6,
    IDTFFO_PRESENT_BIT = 7,
} idt_flags_field_offsets;

void __attribute__((cdecl)) LoadIDT(idt_descriptor *IDTDescriptor);