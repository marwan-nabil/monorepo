#pragma once

#define FILE_IO_MAX_OPEN_FILES 10
#define FILE_IO_INVALID_HANDLE -1

typedef struct
{
    u32 Position;
    u32 Size;

    i16 Handle;
    u16 FirstCluster;
    u16 LoadedCluster;

    b8 IsDirectory;
    b8 IsOpen;

    u8 Buffer[FAT12_SECTOR_SIZE];
} file_io_file;

typedef struct
{
    fat12_ram_disk Fat12RamDisk;
    disk_parameters DiskParameters;
    memory_arena TransientMemoryArena;

    file_io_file OpenFiles[FILE_IO_MAX_OPEN_FILES];
    u8 OpenFilesCount;
} file_io_context;