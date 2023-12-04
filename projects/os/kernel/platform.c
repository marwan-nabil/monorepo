// TODO: add support for disk/fat12/file_io drivers in the kernel
#include <stdarg.h>
#include "platforms\i686-elf\base_types.h"
#include "platforms\i686-elf\basic_defines.h"
#include "platforms\i686-elf\vga\vga.h"
// #include "platforms\i686-elf\bios\disk.h"
// #include "platforms\i686-elf\disk\disk.h"
#include "platforms\i686-elf\cpu\io.h"
#include "platforms\i686-elf\cpu\timing.h"
#include "platforms\i686-elf\math\integers.h"
#include "platforms\i686-elf\strings\print.h"
// #include "platforms\i686-elf\fat12\fat12.h"
#include "platforms\i686-elf\memory\arena_allocator.h"
#include "platforms\i686-elf\strings\path_handling.h"
#include "platforms\i686-elf\memory\arena_allocator.h"
// #include "platforms\i686-elf\file_io\file_io.h"
#include "platforms\i686-elf\cpu\gdt.h"
#include "platforms\i686-elf\cpu\idt.h"

#include "platforms\i686-elf\vga\vga.c"
#include "platforms\i686-elf\cpu\timing.c"
#include "platforms\i686-elf\memory\memory.c"
#include "platforms\i686-elf\strings\strings.c"
#include "platforms\i686-elf\strings\print.c"
// #include "platforms\i686-elf\disk\disk.c"
#include "platforms\i686-elf\memory\arena_allocator.c"
#include "platforms\i686-elf\strings\path_handling.c"
#include "platforms\i686-elf\cpu\gdt.c"
#include "platforms\i686-elf\cpu\idt.c"
// #include "platforms\i686-elf\fat12\get.c"
// #include "platforms\i686-elf\fat12\set.c"
// #include "platforms\i686-elf\file_io\file_io.c"