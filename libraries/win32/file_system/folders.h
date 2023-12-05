#pragma once

b32 CleanExtensionFromDirectory(const char *ExtensionToClean, const char *DirectoryPath, console_context *ConsoleContext);
b32 DoesDirectoryExist(const char *DirectoryPath);
void DeleteDirectoryCompletely(const char *DirectoryPath);
void EmptyDirectory(const char *DirectoryPath);