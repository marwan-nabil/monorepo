#pragma once

#define DISK_OPERATION_MAXIMUM_RETRIES 3

typedef struct
{
    u8 Id;
    u8 Type;
    u16 Cylinders;
    u16 Heads;
    u16 Sectors;
} disk_parameters;