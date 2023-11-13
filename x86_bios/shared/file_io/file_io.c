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

    InitializeFat12RamDisk(&Context->DiskParameters, &Context->TransientMemoryArena, &Context->Fat12RamDisk);

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
    directory_entry far *FileDirectoryEntry = GetDirectoryEntryOfFileByPath
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

    GetDiskSectorFromFatClusterNumber
    (
        &Context->DiskParameters,
        (sector far *)OpenedFile->Buffer,
        OpenedFile->FirstCluster
    );

    FreeMemoryArena(&Context->TransientMemoryArena);

    return FileHandle;
}

void FileRead(file_io_context far *Context, i16 FileHandle, u32 ByteCount, u8 far *DataOut)
{
    file_io_file far *File = &Context->OpenFiles[FileHandle];

    for (u32 LoopIndex = 0; LoopIndex < FAT12_SECTORS_IN_DATA_AREA; LoopIndex++)
    {
        u32 OffsetInSector = File->Position % FAT12_SECTOR_SIZE;
        if ((OffsetInSector + ByteCount) < FAT12_SECTOR_SIZE)
        {
            MemoryCopyFarToFar(DataOut, &File->Buffer[OffsetInSector], ByteCount);
            File->Position += ByteCount;
            return;
        }
        else
        {
            u32 BytesToRead = FAT12_SECTOR_SIZE - OffsetInSector;
            MemoryCopyFarToFar(DataOut, &File->Buffer[OffsetInSector], BytesToRead);

            File->Position += BytesToRead;
            ByteCount -= BytesToRead;
            DataOut += BytesToRead;

            u16 NextCluster = GetFatEntryFromClusterNumber(&Context->Fat12RamDisk, File->LoadedCluster);
            if (IsFatEntryEndOfFile(NextCluster))
            {
                return;
            }
            else
            {
                GetDiskSectorFromFatClusterNumber
                (
                    &Context->DiskParameters,
                    (sector far *)File->Buffer,
                    NextCluster
                );
                File->LoadedCluster = NextCluster;
            }
        }
    }
}

b8 FileWrite(file_io_context far *Context, i16 FileHandle, u32 ByteCount, void far *DataIn)
{
    // TODO: implement.
    return TRUE;
}

void FileSeek(file_io_context far *Context, i16 FileHandle, u32 NewSeekPosition)
{
    file_io_file far *File = &Context->OpenFiles[FileHandle];

    if (NewSeekPosition < File->Size)
    {
        u32 SectorIndex = NewSeekPosition / FAT12_SECTOR_SIZE;

        // loop until you get to this sector and load it
        u16 CurrentCluster = File->FirstCluster;

        while (SectorIndex)
        {
            CurrentCluster = GetFatEntryFromClusterNumber(&Context->Fat12RamDisk, CurrentCluster);
            SectorIndex--;
        }

        GetDiskSectorFromFatClusterNumber
        (
            &Context->DiskParameters,
            (sector far *)File->Buffer,
            CurrentCluster
        );

        File->LoadedCluster = CurrentCluster;
        File->Position = NewSeekPosition;
    }
}

u32 FileGetPosition(file_io_context far *Context, i16 FileHandle)
{
    file_io_file far *File = &Context->OpenFiles[FileHandle];
    return File->Position;
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