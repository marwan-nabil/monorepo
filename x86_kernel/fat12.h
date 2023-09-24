#pragma once

#define DISK_SECTOR_SIZE 512
#define DIRECTORY_ENTRY_SIZE 32
#define FILE_SYSTEM_TYPE_FAT12 "FAT12  "

#pragma pack(push, 1)

union directory_entry
{
    u8 Bytes[DIRECTORY_ENTRY_SIZE];
    struct
    {
        u8 FileName[8];
        u8 FileExtension[3];
        u8 FileAttributes;
        u8 Reserved[2];
        u16 CreationTime;
        u16 CreationDate;
        u16 LastAccessDate;
        u16 Ignored;
        u16 LastWriteTime;
        u16 LastWriteDate;
        u16 FirstLogicalCluster;
        u32 FileSize;
    };
};

union sector
{
    u8 Bytes[DISK_SECTOR_SIZE];
    directory_entry DirectoryEntries[16];
};

union boot_sector
{
    u8 Bytes[DISK_SECTOR_SIZE];
    struct
    {
        u8 Ignored0[11];
        u16 BytesPerSector;
        u8 SectorsPerCluster;
        u16 NumberOfReserevedSectors;
        u8 NumberOfFATs;
        u16 MaximumRootDirectoryEntries;
        u16 TotalSectorCount;
        u8 Ignored1[1];
        u16 SectorsPerFAT;
        u16 SectorsPerTrack;
        u16 NumberOfHeads;
        u8 Ignored2[4];
        u32 TotalSectorCountForFAT32;
        u8 Ignored3[2];
        u8 BootSignature;
        u32 VolumeId;
        u8 VolumeLabel[11];
        u64 FileSystemType;
        u8 Ignored4[450];
    };
};

union file_allocation_table
{
    sector Sectors[9];
    u8 Bytes[4608];
};

struct root_directory
{
    sector Sectors[14];
};

struct data_area
{
    sector Sectors[2847];
};

struct fat12_disk
{
    boot_sector BootSector;
    file_allocation_table Fat1;
    file_allocation_table Fat2;
    root_directory RootDirectory;
    data_area DataArea;
};

#pragma pack(pop)