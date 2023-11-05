#pragma once

#define SECTOR_SIZE 512
#define MAX_PATH_SIZE 256
#define MAX_FILE_HANDLES 10
#define ROOT_DIRECTORY_HANDLE -1

#pragma pack(push, 1)

typedef struct
{
    u8 Name[11];
    u8 Attributes;
    u8 _Reserved;
    u8 CreatedTimeTenths;
    u16 CreatedTime;
    u16 CreatedDate;
    u16 AccessedDate;
    u16 FirstClusterHigh;
    u16 ModifiedTime;
    u16 ModifiedDate;
    u16 FirstClusterLow;
    u32 Size;
} directory_entry;

typedef struct
{
    u8 BootJumpInstruction[3];
    u8 OemIdentifier[8];
    u16 BytesPerSector;
    u8 SectorsPerCluster;
    u16 ReservedSectors;
    u8 FatCount;
    u16 DirEntryCount;
    u16 TotalSectors;
    u8 MediaDescriptorType;
    u16 SectorsPerFat;
    u16 SectorsPerTrack;
    u16 Heads;
    u32 HiddenSectors;
    u32 LargeSectorCount;

    // extended boot record
    u8 DriveNumber;
    u8 _Reserved;
    u8 Signature;
    u32 VolumeId; // serial number, value doesn't matter
    u8 VolumeLabel[11]; // 11 bytes, padded with spaces
    u8 SystemId[8];

    // ... we don't care about code ...
} boot_sector;

#pragma pack(pop)

typedef struct
{
    u32 Position;
    u32 Size;
    i16 Handle;
    b8 IsDirectory;
} file;

enum file_attributes
{
    FAT_ATTRIBUTE_READ_ONLY = 0x01,
    FAT_ATTRIBUTE_HIDDEN = 0x02,
    FAT_ATTRIBUTE_SYSTEM = 0x04,
    FAT_ATTRIBUTE_VOLUME_ID = 0x08,
    FAT_ATTRIBUTE_DIRECTORY = 0x10,
    FAT_ATTRIBUTE_ARCHIVE = 0x20,
    FAT_ATTRIBUTE_LFN =
        FAT_ATTRIBUTE_READ_ONLY |
        FAT_ATTRIBUTE_HIDDEN |
        FAT_ATTRIBUTE_SYSTEM |
        FAT_ATTRIBUTE_VOLUME_ID
};

typedef struct
{
    u8 Buffer[SECTOR_SIZE];
    file Public;
    b8 Opened;
    u32 FirstCluster;
    u32 CurrentCluster;
    u32 CurrentSectorInCluster;
} file_data;

typedef struct
{
    union
    {
        boot_sector BootSector;
        u8 BootSectorBytes[SECTOR_SIZE];
    } BootSector;

    file_data RootDirectory;
    file_data OpenedFiles[MAX_FILE_HANDLES];
} fat_data;