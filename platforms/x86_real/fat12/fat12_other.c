static fat_data far *g_Data;
static u8 far *g_Fat = NULL;
static u32 g_DataSectionLba;

b8 ReadBootSector(disk_parameters *disk)
{
    return ReadDiskSectors(disk, 0, 1, g_Data->BootSector.BootSectorBytes);
}

b8 Fat12ReadFat(disk_parameters *disk)
{
    return ReadDiskSectors(disk, g_Data->BootSector.BootSector.ReservedSectors, g_Data->BootSector.BootSector.SectorsPerFat, g_Fat);
}

b8 InitializeFat12FileSystem(disk_parameters *disk)
{
    g_Data = (fat_data far *)MEMORY_LAYOUT_FAT_DRIVER_DATA_FAR_ADDRESS;

    // read boot sector
    if (!ReadBootSector(disk))
    {
        PrintFormatted("FAT: read boot sector failed\r\n");
        return FALSE;
    }

    // read FAT
    g_Fat = (u8 far *)g_Data + sizeof(fat_data);
    u32 fatSize = g_Data->BootSector.BootSector.BytesPerSector * g_Data->BootSector.BootSector.SectorsPerFat;
    if (sizeof(fat_data) + fatSize >= MEMORY_LAYOUT_FAT_DRIVER_DATA_SIZE)
    {
        PrintFormatted
        (
            "FAT: not enough memory to read FAT! Required %lu, only have %u\r\n",
            sizeof(fat_data) + fatSize, MEMORY_LAYOUT_FAT_DRIVER_DATA_SIZE
        );
        return FALSE;
    }

    if (!Fat12ReadFat(disk))
    {
        PrintFormatted("FAT: read FAT failed\r\n");
        return FALSE;
    }

    // open root directory file
    u32 rootDirLba =
        g_Data->BootSector.BootSector.ReservedSectors +
        g_Data->BootSector.BootSector.SectorsPerFat * g_Data->BootSector.BootSector.FatCount;
    u32 rootDirSize = sizeof(directory_entry) * g_Data->BootSector.BootSector.DirEntryCount;

    g_Data->RootDirectory.Public.Handle = ROOT_DIRECTORY_HANDLE;
    g_Data->RootDirectory.Public.IsDirectory = TRUE;
    g_Data->RootDirectory.Public.Position = 0;
    g_Data->RootDirectory.Public.Size = sizeof(directory_entry) * g_Data->BootSector.BootSector.DirEntryCount;
    g_Data->RootDirectory.Opened = TRUE;
    g_Data->RootDirectory.FirstCluster = rootDirLba;
    g_Data->RootDirectory.CurrentCluster = rootDirLba;
    g_Data->RootDirectory.CurrentSectorInCluster = 0;

    if (!ReadDiskSectors(disk, rootDirLba, 1, g_Data->RootDirectory.Buffer))
    {
        PrintFormatted("FAT: read root directory failed\r\n");
        return FALSE;
    }

    // calculate data section
    u32 rootDirSectors = (rootDirSize + g_Data->BootSector.BootSector.BytesPerSector - 1) / g_Data->BootSector.BootSector.BytesPerSector;
    g_DataSectionLba = rootDirLba + rootDirSectors;

    // reset opened files
    for (i16 i = 0; i < MAX_FILE_HANDLES; i++)
        g_Data->OpenedFiles[i].Opened = FALSE;

    return TRUE;
}

u32 ClusterToLba(u32 cluster)
{
    return g_DataSectionLba + (cluster - 2) * g_Data->BootSector.BootSector.SectorsPerCluster;
}

file far *OpenFatEntry(disk_parameters *disk, directory_entry *entry)
{
    // find empty handle
    i16 handle = -1;
    for (i16 i = 0; i < MAX_FILE_HANDLES && handle < 0; i++)
    {
        if (!g_Data->OpenedFiles[i].Opened)
            handle = i;
    }

    // out of handles
    if (handle < 0)
    {
        PrintFormatted("FAT: out of file handles\r\n");
        return FALSE;
    }

    // setup vars
    file_data far *fd = &g_Data->OpenedFiles[handle];
    fd->Public.Handle = handle;
    fd->Public.IsDirectory = (entry->Attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
    fd->Public.Position = 0;
    fd->Public.Size = entry->Size;
    fd->FirstCluster = entry->FirstClusterLow + ((u32)entry->FirstClusterHigh << 16);
    fd->CurrentCluster = fd->FirstCluster;
    fd->CurrentSectorInCluster = 0;

    if (!ReadDiskSectors(disk, ClusterToLba(fd->CurrentCluster), 1, fd->Buffer))
    {
        PrintFormatted("FAT: read error\r\n");
        return FALSE;
    }

    fd->Opened = TRUE;
    return &fd->Public;
}

u32 GetNextCluster(u32 currentCluster)
{
    u32 fatIndex = currentCluster * 3 / 2;

    if (currentCluster % 2 == 0)
        return (*(u16 far *)(g_Fat + fatIndex)) & 0x0FFF;
    else
        return (*(u16 far *)(g_Fat + fatIndex)) >> 4;
}

u32 Fat12ReadFile(disk_parameters *disk, file far *file, u32 byteCount, void *dataOut)
{
    // get file data
    file_data far *fd = (file->Handle == ROOT_DIRECTORY_HANDLE)
        ? &g_Data->RootDirectory
        : &g_Data->OpenedFiles[file->Handle];

    u8 *u8DataOut = (u8*)dataOut;

    // don't read past the end of the file
    if (!fd->Public.IsDirectory)
        byteCount = Min(byteCount, fd->Public.Size - fd->Public.Position);

    while (byteCount > 0)
    {
        u32 leftInBuffer = SECTOR_SIZE - (fd->Public.Position % SECTOR_SIZE);
        u32 take = Min(byteCount, leftInBuffer);

        MemoryCopyNearToNear(u8DataOut, fd->Buffer + fd->Public.Position % SECTOR_SIZE, take);
        u8DataOut += take;
        fd->Public.Position += take;
        byteCount -= take;

        // PrintFormatted("leftInBuffer=%lu take=%lu\r\n", leftInBuffer, take);
        // See if we need to read more data
        if (leftInBuffer == take)
        {
            // Special handling for root directory
            if (fd->Public.Handle == ROOT_DIRECTORY_HANDLE)
            {
                ++fd->CurrentCluster;

                // read next sector
                if (!ReadDiskSectors(disk, fd->CurrentCluster, 1, fd->Buffer))
                {
                    PrintFormatted("FAT: read error!\r\n");
                    break;
                }
            }
            else
            {
                // calculate next cluster & sector to read
                if (++fd->CurrentSectorInCluster >= g_Data->BootSector.BootSector.SectorsPerCluster)
                {
                    fd->CurrentSectorInCluster = 0;
                    fd->CurrentCluster = GetNextCluster(fd->CurrentCluster);
                }

                if (fd->CurrentCluster >= 0xFF8)
                {
                    // Mark end of file
                    fd->Public.Size = fd->Public.Position;
                    break;
                }

                // read next sector
                if (!ReadDiskSectors(disk, ClusterToLba(fd->CurrentCluster) + fd->CurrentSectorInCluster, 1, fd->Buffer))
                {
                    PrintFormatted("FAT: read error!\r\n");
                    break;
                }
            }
        }
    }

    return u8DataOut - (u8*)dataOut;
}

b8 Fat12ReadDirectoryEntry(disk_parameters *disk, file far *file, directory_entry *dirEntry)
{
    return Fat12ReadFile(disk, file, sizeof(directory_entry), dirEntry) == sizeof(directory_entry);
}

void Fat12CloseFile(file far *file)
{
    if (file->Handle == ROOT_DIRECTORY_HANDLE)
    {
        file->Position = 0;
        g_Data->RootDirectory.CurrentCluster = g_Data->RootDirectory.FirstCluster;
    }
    else
    {
        g_Data->OpenedFiles[file->Handle].Opened = FALSE;
    }
}

b8 Fat12FindFile(disk_parameters *disk, file far *file, const char *name, directory_entry *entryOut)
{
    char fatName[12];
    directory_entry entry;

    // convert from name to fat name
    MemorySetNear(fatName, ' ', sizeof(fatName));
    fatName[11] = '\0';

    const char *ext = GetCharacterPointer(name, '.');
    if (ext == NULL)
        ext = name + 11;

    for (i16 i = 0; i < 8 && name[i] && name + i < ext; i++)
        fatName[i] = ConvertCharacterToUpperCase(name[i]);

    if (ext != NULL)
    {
        for (i16 i = 0; i < 3 && ext[i + 1]; i++)
            fatName[i + 8] = ConvertCharacterToUpperCase(ext[i + 1]);
    }

    while (Fat12ReadDirectoryEntry(disk, file, &entry))
    {
        if (MemoryCompareNearToNear(fatName, entry.Name, 11) == 0)
        {
            *entryOut = entry;
            return TRUE;
        }
    }

    return FALSE;
}

file far *Fat12OpenFile(disk_parameters *disk, const char *path)
{
    char name[MAX_PATH_SIZE];

    // ignore leading slash
    if (path[0] == '/')
        path++;

    file far *current = &g_Data->RootDirectory.Public;

    while (*path) {
        // extract next file name from path
        b8 isLast = FALSE;
        const char *delim = GetCharacterPointer(path, '/');
        if (delim != NULL)
        {
            MemoryCopyNearToNear(name, (void *)path, delim - path);
            name[delim - path + 1] = '\0';
            path = delim + 1;
        }
        else
        {
            unsigned len = StringLengthNear((char *)path);
            MemoryCopyNearToNear(name, (void *)path, len);
            name[len + 1] = '\0';
            path += len;
            isLast = TRUE;
        }

        // find directory entry in current directory
        directory_entry entry;
        if (Fat12FindFile(disk, current, name, &entry))
        {
            Fat12CloseFile(current);

            // check if directory
            if (!isLast && entry.Attributes & FAT_ATTRIBUTE_DIRECTORY == 0)
            {
                PrintFormatted("FAT: %s not a directory\r\n", name);
                return NULL;
            }

            // open new directory entry
            current = OpenFatEntry(disk, &entry);
        }
        else
        {
            Fat12CloseFile(current);
            PrintFormatted("FAT: %s not found\r\n", name);
            return NULL;
        }
    }

    return current;
}