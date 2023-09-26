#pragma once

struct ram_file
{
    void *Memory;
    char Name[8];
    char Extension[3];
    u32 Size;
};