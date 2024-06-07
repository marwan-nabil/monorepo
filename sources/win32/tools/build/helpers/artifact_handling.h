#pragma once

struct artifact_dependency_info
{
    char AtrifactPath[1024];
    artifact_node *ArtifactInputs;

    char ObjectFilePath[1024];
    FILETIME SourceFileLastWriteTime;
    FILETIME ObjectFileLastWriteTime;
};