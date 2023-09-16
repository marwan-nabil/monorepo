#pragma once

struct read_file_result;
struct thread_context;
struct game_pixel_buffer;
struct game_input;
struct game_memory;
struct game_sound_request;

#define PLATFORM_READ_FILE(name) read_file_result name(thread_context *Thread, char *FileName)
#define PLATFORM_WRITE_FILE(name) b32 name(thread_context *Thread, char *FileName, void *DataToWrite, u32 DataSize)
#define PLATFORM_FREE_FILE_MEMORY(name) void name(thread_context *Thread, void *FileMemory)
#define GAME_UPDATE_AND_RENDER(name) void name(thread_context *ThreadContext, game_pixel_buffer *PixelBuffer, game_input *GameInput, game_memory *GameMemory)
#define GAME_GET_SOUND_SAMPLES(name) void name(thread_context *ThreadContext, game_sound_request *SoundRequest, game_memory *GameMemory)

typedef PLATFORM_READ_FILE(platform_read_file);
typedef PLATFORM_WRITE_FILE(platform_write_file);
typedef PLATFORM_FREE_FILE_MEMORY(platform_free_file_memory);
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
typedef GAME_GET_SOUND_SAMPLES(game_get_sound_samples);

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
            game_button_state Start;
            // NOTE: put all new buttons above the Start button
        };
    };
};

struct game_input
{
    f32 TimeDeltaForFrame;
    game_button_state MouseButtons[5];
    i32 MouseX;
    i32 MouseY;
    i32 MouseZ;
    game_controller_state ControllerStates[5];
};

struct game_memory
{
    b32 IsInitialized;
    u64 PermanentStorageSize;
    void *PermanentStorage;
    u64 TransientStorageSize;
    void *TransientStorage;

    platform_read_file *PlatformReadFile;
    platform_write_file *PlatformWriteFile;
    platform_free_file_memory *PlatformFreeFileMemory;
};

struct read_file_result
{
    void *FileContents;
    u32 ConstentsSize;
};