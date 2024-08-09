#pragma once

b32 CleanExtensionFromDirectory(const char *ExtensionToClean, const char *DirectoryPath);
b32 DoesDirectoryExist(const char *DirectoryPath);
void DeleteDirectoryCompletely(const char *DirectoryPath);
void EmptyDirectory(const char *DirectoryPath);
string_node *GetListOfFilesInFolder(char *FolderPath);
string_node *GetListOfFilesWithNameInFolder(char *DirectoryPath, char *FileName);
void GetListOfFilesWithNameInFolderRecursive(char *DirectoryPath, char *FileName, string_node **List);