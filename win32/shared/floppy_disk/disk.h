#pragma once

#define DISK_SECTORS_PER_TRACK 50
#define DISK_HEADS 1

struct disk_address
{
    u32 Cylinder;
    u32 Head;
    u32 Sector;
};