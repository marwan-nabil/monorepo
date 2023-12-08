// TODO: add support for disk/fat12/file_io drivers in the kernel
#include <stdarg.h>
#include "sources\i686-elf\shared\base_types.h"
#include "sources\i686-elf\shared\basic_defines.h"
#include "sources\i686-elf\shared\vga\vga.h"
// #include "sources\i686-elf\shared\bios\disk.h"
// #include "sources\i686-elf\shared\disk\disk.h"
#include "sources\i686-elf\shared\cpu\io.h"
#include "sources\i686-elf\shared\cpu\timing.h"
#include "sources\i686-elf\shared\math\integers.h"
#include "sources\i686-elf\shared\strings\print.h"
// #include "sources\i686-elf\shared\fat12\fat12.h"
#include "sources\i686-elf\shared\memory\arena_allocator.h"
#include "sources\i686-elf\shared\strings\path_handling.h"
#include "sources\i686-elf\shared\memory\arena_allocator.h"
// #include "sources\i686-elf\shared\file_io\file_io.h"
#include "sources\i686-elf\shared\cpu\gdt.h"
#include "sources\i686-elf\shared\cpu\idt.h"

#include "sources\i686-elf\shared\vga\vga.c"
#include "sources\i686-elf\shared\cpu\timing.c"
#include "sources\i686-elf\shared\memory\memory.c"
#include "sources\i686-elf\shared\strings\strings.c"
#include "sources\i686-elf\shared\strings\print.c"
// #include "sources\i686-elf\shared\disk\disk.c"
#include "sources\i686-elf\shared\memory\arena_allocator.c"
#include "sources\i686-elf\shared\strings\path_handling.c"
#include "sources\i686-elf\shared\cpu\gdt.c"
#include "sources\i686-elf\shared\cpu\idt.c"
// #include "sources\i686-elf\shared\fat12\get.c"
// #include "sources\i686-elf\shared\fat12\set.c"
// #include "sources\i686-elf\shared\file_io\file_io.c"