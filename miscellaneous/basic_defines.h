#pragma once

#define ArrayLength(Array) (sizeof(Array) / sizeof((Array)[0]))
#define OffsetOf(DataType, Member) ((size_t)&(((DataType *)0)->Member))