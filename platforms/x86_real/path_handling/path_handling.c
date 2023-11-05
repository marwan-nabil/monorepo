// NOTE: no need to implement it with the arena allocator
// void FreeFilePathSegmentList(file_path_node *RootNode)
// {
//     file_path_node *CurrentNode = RootNode;
//     file_path_node *ChildNode;

//     while (CurrentNode)
//     {
//         ChildNode = CurrentNode->ChildNode;
//         free(CurrentNode);
//         CurrentNode = ChildNode;
//     }
// }

file_path_node far *
CreateFilePathSegmentList(char far *FileFullPath, memory_arena far *MemoryArena)
{
    char LocalPathBuffer[PATH_HANDLING_MAX_PATH];
    MemoryZeroNear(LocalPathBuffer, ArrayCount(LocalPathBuffer));

    StringConcatenateFarToNear(LocalPathBuffer, ArrayCount(LocalPathBuffer), FileFullPath);

    u32 PathLength = StringLengthNear(LocalPathBuffer);

    file_path_node far *CurrentFilePathNode = PushStruct(MemoryArena, file_path_node);
    MemoryZeroFar(CurrentFilePathNode, sizeof(file_path_node));

    file_path_node far *LastFilePathNode = NULL;

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

void RemoveLastSegmentFromPath(char far *Path)
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