#pragma once

struct artifact_table_entry
{
    char FilePath[1024];
    FILETIME FileLastWriteTime;
    b8 FileExists;
    artifact_table_entry *NextEntry;
};

u32 AddArtifact(char *FileName);
