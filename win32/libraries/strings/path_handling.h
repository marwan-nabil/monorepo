#pragma once

#define PATH_HANDLING_SEGMENT_STRING_LENGTH 128

struct file_path_segment_node
{
    char SegmentName[PATH_HANDLING_SEGMENT_STRING_LENGTH];
    file_path_segment_node *ChildNode;
};

void FreeFilePathSegmentList(file_path_segment_node *RootNode);
file_path_segment_node *CreateFilePathSegmentList(char *FileFullPath);
void RemoveLastSegmentFromPath(char *Path, b8 KeepSlash, char Separator);
char *GetPointerToLastSegmentFromPath(char *Path);
file_path_segment_node *SkipSubPath
(
    file_path_segment_node *Path,
    file_path_segment_node *SubPathToSkip
);
void FlattenPathSegmentList
(
    file_path_segment_node *PathList,
    char *TargetStringBuffer,
    u32 TargetBufferLength,
    char Separator
);