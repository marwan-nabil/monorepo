void FileIoInitialize
(
    u16 DriveNumber,
    void far *TransientMemoryAddress,
    u32 TransientMemorySize,
    file_io_context far *Context
)
{
    GetDiskDriveParameters(&Context->DiskParameters, DriveNumber);

    InitializeMemoryArena
    (
        &Context->TransientMemoryArena,
        TransientMemorySize,
        TransientMemoryAddress
    );

    Fat12InitializeRamDisk(&Context->DiskParameters, &Context->TransientMemoryArena, &Context->Fat12RamDisk);

    Context->OpenFilesCount = 0;
    for (u16 Index = 0; Index < FILE_IO_MAX_OPEN_FILES; Index++)
    {
        Context->OpenFiles[Index].IsOpen = FALSE;
    }
}

i16 FileOpen(file_io_context far *Context, char far *FilePath)
{
    if (Context->OpenFilesCount >= FILE_IO_MAX_OPEN_FILES)
    {
        return FILE_IO_INVALID_HANDLE;
    }

    i16 FileHandle = FILE_IO_INVALID_HANDLE;

    for (u16 Index = 0; Index < FILE_IO_MAX_OPEN_FILES; Index++)
    {
        if (!Context->OpenFiles[Index].IsOpen)
        {
            FileHandle = Index;
        }
    }

    if (FileHandle == FILE_IO_INVALID_HANDLE)
    {
        return FILE_IO_INVALID_HANDLE;
    }

    file_io_file far *OpenedFile = &Context->OpenFiles[FileHandle];
    directory_entry far *FileDirectoryEntry = Fat12GetDirectoryEntryOfFile
    (
        &Context->Fat12RamDisk,
        &Context->TransientMemoryArena,
        &Context->DiskParameters,
        FilePath
    );

    if (!FileDirectoryEntry)
    {
        return FILE_IO_INVALID_HANDLE;
    }

    Context->OpenFilesCount++;
    OpenedFile->Handle = FileHandle;
    if
    (
        (FileDirectoryEntry->FileAttributes & FAT12_FILE_ATTRIBUTE_DIRECTORY) ==
        FAT12_FILE_ATTRIBUTE_DIRECTORY
    )
    {
        OpenedFile->IsDirectory = TRUE;
    }
    else
    {
        OpenedFile->IsDirectory = FALSE;
    }
    OpenedFile->IsOpen = TRUE;
    OpenedFile->Position = 0;
    OpenedFile->Size = FileDirectoryEntry->FileSize;
    OpenedFile->FirstCluster = FileDirectoryEntry->ClusterNumberLowWord;
    OpenedFile->LoadedCluster = OpenedFile->FirstCluster;

    ReadDiskSectors
    (
        &Context->DiskParameters,
        TranslateClusterNumberToSectorIndex(OpenedFile->FirstCluster),
        1,
        OpenedFile->Buffer
    );

    FreeMemoryArena(&Context->TransientMemoryArena);

    return OpenedFile->Handle;
}

void FileRead(file_io_context far *Context, i16 FileHandle, u32 ByteCount, void far *DataOut)
{
    file_io_file far *File = &Context->OpenFiles[FileHandle];
    u32 PositionSector = File->Position / FAT12_SECTOR_SIZE;
    u32 SectorOffset = File->Position % FAT12_SECTOR_SIZE;

    if ((SectorOffset + ByteCount) < FAT12_SECTOR_SIZE)
    {
        MemoryCopyFarToFar(DataOut, &File->Buffer[SectorOffset], ByteCount);
        File->Position += ByteCount;
    }
    else
    {
        // TODO: implement read from file that spans multiple sectors.
    }
}

b8 FileWrite(file_io_context far *Context, i16 FileHandle, u32 ByteCount, void far *DataIn)
{
    // TODO: implement.
    return TRUE;
}

void FileClose(file_io_context far *Context, i16 FileHandle)
{
    file_io_file far *File = &Context->OpenFiles[FileHandle];
    File->IsOpen = FALSE;
}

void ListDirectory(file_io_context far *Context, char *DirectoryFilePath)
{
    Fat12ListDirectory
    (
        &Context->Fat12RamDisk,
        &Context->TransientMemoryArena,
        &Context->DiskParameters,
        DirectoryFilePath
    );
    FreeMemoryArena(&Context->TransientMemoryArena);
}