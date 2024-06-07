#pragma once

enum artifact_kind
{
    AK_FILE,
    AK_BUILD_INFO
};

struct file_info
{
    char FilePath[1024];
    FILETIME FileLastWriteTime;
    b8 Exists;
};

struct build_info
{
    char BuildToolFlags[1024];
    char CompilerIncludePath[1024];
};

struct artifact
{
    artifact_kind Kind;
    void *Info;
};

struct dependency_link
{
    u32 Artifact;
    artifact_node *Next;
};