#pragma once

#define FAT12_DISK_SECTOR_SIZE 512
#define FAT12_DISK_CLUSTER_SIZE 512
#define FAT12_BOOT_SIGNATURE 0xAA55
#define FAT12_ENTRIES_PER_FAT 3072
#define FAT12_MAX_PATH 1024

#define FAT12_FILENAME_EMPTY_SLOT 0x00
#define FAT12_FILENAME_DELETED_SLOT 0xE5

#define FAT12_FILE_ATTRIBUTE_NORMAL 0x00
#define FAT12_FILE_ATTRIBUTE_READONLY 0x01
#define FAT12_FILE_ATTRIBUTE_HIDDEN 0x02
#define FAT12_FILE_ATTRIBUTE_SYSTEM 0x04
#define FAT12_FILE_ATTRIBUTE_VOLUME 0x08
#define FAT12_FILE_ATTRIBUTE_DIRECTORY 0x10
#define FAT12_FILE_ATTRIBUTE_ARCHIVE 0x20

#define FAT12_FAT_ENTRY_FREE_CLUSTER 0x0000
#define FAT12_FAT_ENTRY_RESERVED_CLUSTER 0x0FF6
#define FAT12_FAT_ENTRY_BAD_CLUSTER 0x0FF7
#define FAT12_FAT_ENTRY_END_OF_FILE_CLUSTER_RANGE_START 0x0FF8
#define FAT12_FAT_ENTRY_END_OF_FILE_CLUSTER_RANGE_END 0x0FFF

#pragma pack(push, 1)

struct boot_sector
{
    u8 JumpInstructionSpace[3];
    u8 OEMName[8];

    u16 BytesPerSector;
    u8 SectorsPerCluster;
    u16 NumberOfReserevedSectors;
    u8 NumberOfFATs;
    u16 RootDirectoryEntries;
    u16 TotalSectors;
    u8 MediaDescriptor;
    u16 SectorsPerFAT;
    u16 SectorsPerTrack;
    u16 NumberOfHeads;
    u32 HiddenSectors;
    u32 LargeSectors;

    u8 DriveNumber;
    u8 Reserved;
    u8 Signature;
    u32 VolumeId;
    u8 VolumeLabel[11];
    u8 SystemId[8];

    u8 BootSectorCode[448];
    u16 BootSectorSignature;
};

struct directory_entry
{
    u8 FileName[8];
    u8 FileExtension[3];
    u8 FileAttributes;
    u8 Reserved[2];
    u16 CreationTime;
    u16 CreationDate;
    u16 LastAccessDate;
    u16 HighWordOfClusterNumber;
    u16 LastWriteTime;
    u16 LastWriteDate;
    u16 FirstLogicalCluster;
    u32 FileSize;
};

union sector
{
    u8 Bytes[FAT12_DISK_SECTOR_SIZE];
    directory_entry DirectoryEntries[16];
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

union fat12_disk
{
    struct
    {
        boot_sector BootSector;
        file_allocation_table Fat1;
        file_allocation_table Fat2;
        root_directory RootDirectory;
        data_area DataArea;
    };
    sector Sectors[2880];
};

#pragma pack(pop)

struct file_path_node
{
    char FileName[8];
    char FileExtension[3];
    file_path_node *ChildNode;
};