#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "..\miscellaneous\base_types.h"
#include "..\miscellaneous\assertions.h"
#include "..\miscellaneous\basic_defines.h"

#include "fat12.h"

inline u32 TranslateSectorNumber(u32 LogicalSectorNumber)
{
    Assert(LogicalSectorNumber >= 2);
    u32 PhysicalSectorNumber = 33 + LogicalSectorNumber - 2;
    return PhysicalSectorNumber;
}

inline u16 GetFatEntry(file_allocation_table *Fat, u32 EntryIndex)
{
    u16 Result = 0;
    u32 StartingByteIndex = EntryIndex * 3 / 2;

    // ## #@ @@ %% %& && $$ $# ## @@ @  ...
    // DD DD DD DD DD DD DD DD DD DD DD ...

    if ((EntryIndex % 2) == 0)
    {
        Result = Fat->Bytes[StartingByteIndex];
        Result |= (Fat->Bytes[StartingByteIndex + 1] & 0x0F) << 8;
    }
    else
    {
        Result = (Fat->Bytes[StartingByteIndex] & 0xF0) >> 4;
        Result |= (Fat->Bytes[StartingByteIndex + 1]) << 4;
    }

    return Result;
}

void CreateFat12File(fat12_disk *Disk, char *FileName, void *Data, u32 Size)
{

}

i32
main(i32 argc, u8 **argv)
{
    Assert(sizeof(fat12_disk) == (2880 * DISK_SECTOR_SIZE));

    printf("Finished.\n");

    return 0;
}