#pragma once

#define ArrayLength(Array) (sizeof(Array) / sizeof((Array)[0]))
#define OffsetOf(DataType, Member) ((size_t)&(((DataType *)0)->Member))

#define KiloBytes(Value) ((Value)*1024LL)
#define MegaBytes(Value) ((Value)*KiloBytes(1024LL))
#define GigaBytes(Value) ((Value)*MegaBytes(1024LL))
#define TeraBytes(Value) ((Value)*GigaBytes(1024LL))