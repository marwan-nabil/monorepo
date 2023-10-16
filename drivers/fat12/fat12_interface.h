#pragma once

struct file_path_node
{
    char FileName[8];
    char FileExtension[3];
    file_path_node *ChildNode;
};