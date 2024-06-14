#pragma once

b32 CleanExtensionFromDirectory(const char *ExtensionToClean, const char *DirectoryPath);
b32 DoesDirectoryExist(const char *DirectoryPath);
void DeleteDirectoryCompletely(const char *DirectoryPath);
void EmptyDirectory(const char *DirectoryPath);
string_node *GetListOfFilesInFolder(char *FolderPath);