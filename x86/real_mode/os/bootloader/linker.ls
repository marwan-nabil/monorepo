FORMAT RAW BIN
OPTION
    QUIET,
    NODEFAULTLIBS,
    START=entry,
    VERBOSE,
    OFFSET=0,
    STACK=0x200,
    MAP=bootloader.map

ORDER
    CLNAME CODE
        SEGMENT _ENTRY.entry
        SEGMENT _ENTRY.BootloaderEntryMessage
        SEGMENT _TEXT._BIOS_PrintCharacter
        SEGMENT _TEXT._BIOS_PrintString
        SEGMENT _TEXT._DivideU64ByU32
        SEGMENT _TEXT.__U4D
        SEGMENT _TEXT.__U4M
        SEGMENT _TEXT._BIOS_DiskReset
        SEGMENT _TEXT._BIOS_DiskRead
        SEGMENT _TEXT._BIOS_DiskWrite
        SEGMENT _TEXT._BIOS_GetDiskDriveParameters
        SEGMENT _TEXT
    CLNAME DATA
        SEGMENT _DATA.DataSegmentAlignmentPadding
        SEGMENT CONST
        SEGMENT _DATA