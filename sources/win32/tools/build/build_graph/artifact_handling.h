#pragma once

struct artifact_table_entry;

struct artifact_list_node
{
    artifact_table_entry *Artifact;
    artifact_list_node *NextNode;
};

struct artifact_table_entry
{
    char FilePath[1024];
    FILETIME FileLastWriteTime;
    b8 FileExists;
    artifact_list_node *Dependencies;
    artifact_table_entry *NextEntry;
};

artifact_table_entry *AddArtifact(char *FileName);
void PushArtifactList(artifact_list_node **List, char *FilePath);
void FreeArtifactList(artifact_list_node *RootNode);
