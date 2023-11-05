#pragma once

#define PATH_HANDLING_MAX_FILENAME 512
#define PATH_HANDLING_MAX_PATH 1024

typedef struct _file_path_node
{
    char FileName[PATH_HANDLING_MAX_FILENAME];
    struct _file_path_node far *ChildNode;
} file_path_node;