FORMAT RAW BIN
OPTION
    QUIET,
    NODEFAULTLIBS,
    START=entry,
    VERBOSE,
    OFFSET=0,
    STACK=0X200,
    MAP=bootloader.map

ORDER
    CLNAME CODE
        SEGMENT _ENTRY.entry
        SEGMENT _TEXT._X86_PrintCharacter
        SEGMENT _TEXT._X86_PrintString
        SEGMENT _TEXT._X86_DivideU64ByU32
        SEGMENT _TEXT._X86_DiskReset
        SEGMENT _TEXT
        SEGMENT _ENTRY.BootloaderEntryMessage
    CLNAME DATA
        SEGMENT CONST
        SEGMENT _DATA