#pragma once

#include <stdint.h>
#include <math.h>

#if HANDMADE_SLOW
#define Assert(Expression) {if(!(Expression)){ *(int *)0 = 0; }}
#else
#define Assert(Expression) 
#endif

#define InvalidCodepath Assert(!"InvalidCodepath")

#define ArrayCount(Array) (sizeof(Array)/sizeof((Array)[0]))

#define KiloBytes(Value) ((Value)*1024LL)
#define MegaBytes(Value) ((Value)*KiloBytes(1024LL))
#define GigaBytes(Value) ((Value)*MegaBytes(1024LL))
#define TeraBytes(Value) ((Value)*GigaBytes(1024LL))

#define Minimum(A, B) (((A) < (B))? (A) : (B))
#define Maximum(A, B) (((A) > (B))? (A) : (B))

#define internal static
#define local_persist static
#define global_variable static

#ifndef COMPILER_MSVC
#define COMPILER_MSVC 0
#endif

#ifndef COMPILER_LLVM
#define COMPILER_LLVM 0
#endif

#if !(COMPILER_MSVC) && !(COMPILER_LLVM)
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#else 
#undef COMPILER_LLVM
#define COMPILER_LLVM 1
#endif
// TODO: support more compiler intrinsics
#endif

#if COMPILER_MSVC
#include <intrin.h>
#endif

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
typedef int32_t b32;
typedef float f32;
typedef double f64;
typedef size_t memory_index;

struct thread_context
{
    i32 PlaceHolder;
};

struct game_pixel_buffer
{
    void *PixelsMemory;
    i32 WidthInPixels;
    i32 HeightInPixels;
    i32 BytesPerPixel;
    i32 BytesPerRow;
};

struct game_sound_request
{
    u32 OutputSamplesCount;
    u32 SamplesPerSecond;
    i16 *SamplesMemory;
};

struct game_button_state
{
    i32 TransitionsCount;
    b32 IsDown;
};

struct game_controller_state
{
    b32 IsConnected;
    b32 MovementIsAnalog;
    f32 StickAverageX;
    f32 StickAverageY;

    union
    {
        game_button_state Buttons[12];
        struct
        {
            game_button_state MoveUp;
            game_button_state MoveDown;
            game_button_state MoveLeft;
            game_button_state MoveRight;
            game_button_state ActionUp;
            game_button_state ActionDown;
            game_button_state ActionLeft;
            game_button_state ActionRight;
            game_button_state LeftShoulder;
            game_button_state RightShoulder;
            game_button_state Back;
            game_button_state Start; // NOTE: put all new buttons above this line 
        };
    };
};

#if HANDMADE_INTERNAL

struct debug_read_file_result
{
    void *FileContents;
    u32 ConstentsSize;
};

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(thread_context *Thread, char *FileName)
#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) b32 name(thread_context *Thread, char *FileName, void *DataToWrite, u32 DataSize)
#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(thread_context *Thread, void *FileMemory)

typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#endif // HANDMADE_INTERNAL

struct game_input
{
    f32 TimeDeltaForFrame;
    game_button_state MouseButtons[5];
    i32 MouseX;
    i32 MouseY;
    i32 MouseZ;
    game_controller_state ControllerStates[5];
};

inline game_controller_state *
GetController(game_input *GameInput, int ControllerIndex)
{
    Assert(ControllerIndex < ArrayCount(GameInput->ControllerStates));
    return &GameInput->ControllerStates[ControllerIndex];
}

struct game_memory
{
    b32 IsInitialized;
    u64 PermanentStorageSize;
    void *PermanentStorage;
    u64 TransientStorageSize;
    void *TransientStorage;

    debug_platform_read_entire_file *DebugPlatformReadEntireFile;
    debug_platform_write_entire_file *DebugPlatformWriteEntireFile;
    debug_platform_free_file_memory *DebugPlatformFreeFileMemory;
};

#define GAME_UPDATE_AND_RENDER(name) void name(thread_context *ThreadContext, game_pixel_buffer *PixelBuffer, game_input *GameInput, game_memory *GameMemory)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render_function);

#define GAME_GET_SOUND_SAMPLES(name) void name(thread_context *ThreadContext, game_sound_request *SoundRequest, game_memory *GameMemory)
typedef GAME_GET_SOUND_SAMPLES(game_get_sound_samples_function);
