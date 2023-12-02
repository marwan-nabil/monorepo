idt_entry CreateIDTEntry
(
    u32 EntryAddress,
    u16 SegmentSelector,
    u8 Flags
)
{
    idt_entry Result;
    Result.EntryAddressLow = (u16)EntryAddress;
    Result.SegmentSelector = SegmentSelector;
    Result.Flags = Flags;
    Result.EntryAddressHigh = (u16)(EntryAddress >> 16);
    return Result;
}