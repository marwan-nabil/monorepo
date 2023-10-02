disk_address TranslateToDiskAddress(u32 LogicalBlockAddress)
{
    disk_address Result = {};
    Result.Cylinder = (LogicalBlockAddress / DISK_SECTORS_PER_TRACK) / DISK_HEADS;
    Result.Head = (LogicalBlockAddress / DISK_SECTORS_PER_TRACK) % DISK_HEADS;
    Result.Sector = (LogicalBlockAddress % DISK_SECTORS_PER_TRACK) + 1;
    return Result;
}