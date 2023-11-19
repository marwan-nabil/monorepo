file_path_node *
CreateFilePathSegmentList(char *FileFullPath, memory_arena *MemoryArena)
{
    char LocalPathBuffer[PATH_HANDLING_MAX_PATH];
    MemoryZeroNear(LocalPathBuffer, ArrayCount(LocalPathBuffer));

    StringConcatenateFarToNear(LocalPathBuffer, ArrayCount(LocalPathBuffer), FileFullPath);

    u32 PathLength = StringLengthNear(LocalPathBuffer);

    file_path_node *CurrentFilePathNode = PushStruct(MemoryArena, file_path_node);
    MemoryZeroFar(CurrentFilePathNode, sizeof(file_path_node));

    file_path_node *LastFilePathNode = NULL;

    for (i32 CharIndex = PathLength - 1; CharIndex >= 0; CharIndex--)
    {
        if (LocalPathBuffer[CharIndex] == '\\')
        {
            char PathSegment[PATH_HANDLING_MAX_PATH];
            MemoryZeroNear(PathSegment, ArrayCount(PathSegment));

            StringConcatenateNearToNear(PathSegment, PATH_HANDLING_MAX_PATH, &LocalPathBuffer[CharIndex + 1]);
            MemoryZeroNear(&LocalPathBuffer[CharIndex], StringLengthNear(&LocalPathBuffer[CharIndex]));

            if (!CurrentFilePathNode)
            {
                CurrentFilePathNode = PushStruct(MemoryArena, file_path_node);
                MemoryZeroFar(CurrentFilePathNode, sizeof(file_path_node));
            }

            MemoryCopyNearToFar(CurrentFilePathNode->FileName, PathSegment, ArrayCount(CurrentFilePathNode->FileName));
            CurrentFilePathNode->ChildNode = LastFilePathNode;

            LastFilePathNode = CurrentFilePathNode;
            CurrentFilePathNode = NULL;
        }
    }

    return LastFilePathNode;
}

void RemoveLastSegmentFromPath(char *Path)
{
    u32 PathLength = StringLengthFar(Path);
    for (i32 CharIndex = PathLength - 1; CharIndex >= 0; CharIndex--)
    {
        if (Path[CharIndex] == '\\')
        {
            MemoryZeroFar(&Path[CharIndex], StringLengthFar(&Path[CharIndex]));
            return;
        }
    }
}

u32 GetLastCharacterIndex(char *String, u32 StringLength, char Character)
{
    for (i32 CharacterIndex = StringLength - 1; CharacterIndex >= 0; CharacterIndex--)
    {
        if (String[CharacterIndex] == Character)
        {
            return CharacterIndex;
        }
    }

    return UINT32_MAX;
}

void GetFileNameAndExtensionFromString
(
    char *SourceString,
    char *FileName, u32 FileNameSize,
    char *FileExtension, u32 FileExtensionSize
)
{
    u32 SourceStringLength = StringLengthFar(SourceString);
    u32 DotIndex = GetLastCharacterIndex(SourceString, SourceStringLength, '.');

    u32 ReadIndex = 0;
    u32 WriteIndex = 0;

    while
    (
        (ReadIndex < DotIndex) &&
        (ReadIndex < SourceStringLength) &&
        (WriteIndex < FileNameSize)
    )
    {
        FileName[WriteIndex++] = SourceString[ReadIndex++];
    }

    if (ReadIndex == DotIndex)
    {
        WriteIndex = 0;
        ReadIndex++;

        while
        (
            (ReadIndex < SourceStringLength) &&
            (WriteIndex < FileExtensionSize)
        )
        {
            FileExtension[WriteIndex++] = SourceString[ReadIndex++];
        }
    }
    else if (ReadIndex == SourceStringLength)
    {
        return;
    }
    else if (WriteIndex == FileNameSize)
    {
        if (DotIndex == UINT32_MAX)
        {
            MemoryZeroFar(FileExtension, FileExtensionSize);
        }
        else
        {
            WriteIndex = 0;
            ReadIndex = DotIndex + 1;

            while
            (
                (ReadIndex < SourceStringLength) &&
                (WriteIndex < FileExtensionSize)
            )
            {
                FileExtension[WriteIndex++] = SourceString[ReadIndex++];
            }
        }
    }
}