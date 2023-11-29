#pragma once

#define PATH_HANDLING_MAX_FILENAME 512

struct file_path_node
{
    char FileName[PATH_HANDLING_MAX_FILENAME];
    file_path_node *ChildNode;
};