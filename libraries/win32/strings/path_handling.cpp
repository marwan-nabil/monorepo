static void
FreeFilePathSegmentList(file_path_node *RootNode)
{
    file_path_node *CurrentNode = RootNode;
    file_path_node *ChildNode;

    while (CurrentNode)
    {
        ChildNode = CurrentNode->ChildNode;
        free(CurrentNode);
        CurrentNode = ChildNode;
    }
}

static file_path_node *
CreateFilePathSegmentList(char *FileFullPath)
{
    char LocalPathBuffer[MAX_PATH] = {};
    StringCchCat(LocalPathBuffer, ArrayCount(LocalPathBuffer), FileFullPath);

    u32 PathLength = StringLength(LocalPathBuffer);

    file_path_node *CurrentFilePathNode = (file_path_node *)malloc(sizeof(file_path_node));
    *CurrentFilePathNode = {};
    file_path_node *LastFilePathNode = 0;

    for (i32 CharIndex = PathLength - 1; CharIndex >= 0; CharIndex--)
    {
        if (LocalPathBuffer[CharIndex] == '\\')
        {
            char PathSegment[MAX_PATH] = {};
            StringCchCat(PathSegment, MAX_PATH, &LocalPathBuffer[CharIndex + 1]);
            ZeroMemory(&LocalPathBuffer[CharIndex], StringLength(&LocalPathBuffer[CharIndex]));

            if (!CurrentFilePathNode)
            {
                CurrentFilePathNode = (file_path_node *)malloc(sizeof(file_path_node));
                *CurrentFilePathNode = {};
            }

            memcpy(CurrentFilePathNode->FileName, PathSegment, ArrayCount(CurrentFilePathNode->FileName));
            CurrentFilePathNode->ChildNode = LastFilePathNode;

            LastFilePathNode = CurrentFilePathNode;
            CurrentFilePathNode = 0;
        }
    }

    return LastFilePathNode;
}

static void RemoveLastSegmentFromPath(char *Path)
{
    u32 PathLength = StringLength(Path);
    for (i32 CharIndex = PathLength - 1; CharIndex >= 0; CharIndex--)
    {
        if (Path[CharIndex] == '\\')
        {
            ZeroMemory(&Path[CharIndex], StringLength(&Path[CharIndex]));
            return;
        }
    }
}