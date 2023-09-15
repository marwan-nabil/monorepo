#pragma once

#define WIN32_FILENAME_LENGTH MAX_PATH

struct win32_pixel_buffer
{
    BITMAPINFO BitmapInfo;
    void *PixelsMemory;
    i32 WidthInPixels;
    i32 HeightInPixels;
    i32 BytesPerPixel;
    i32 BytesPerRow;
};

struct win32_window_dimensions
{
    i32 WidthInPixels;
    i32 HeightInPixels;
};

struct win32_sound_configuration
{
    u32 SamplesPerSecond;
    u32 BytesPerSample;
    u32 SoundBufferSize;
    u32 SafetyMarginInBytes;
};

struct win32_sound_state
{
    u32 RunningSampleIndex;
};

struct win32_debug_sound_time_marker
{
    u32 OutputPlayCursor;
    u32 OutputWriteCursor;
    u32 OutputLocation;
    u32 OutputByteCount;
    u32 FlipPlayCursor;
    u32 FlipWriteCursor;
    u32 ExpectedFlipCursor;
};

struct win32_game_code
{
    HMODULE GameCodeDll;
    FILETIME LastWriteTimeForLoadedDLL;
    game_update_and_render_function *UpdateAndRender;
    game_get_sound_samples_function *GetSoundSamples;
    b32 IsValid;
};

struct win32_game_input_buffer
{
    char InputBufferFileName[WIN32_FILENAME_LENGTH];
    HANDLE FileHandle;
    HANDLE MemoryMapHandle;
    void *MemoryBlock;
};

struct win32_state
{
    u64 GameMemoryBlockSize;
    void *GameMemoryBlock;

    win32_game_input_buffer GameInputBuffers[4];

    u32 CurrentGameInputRecordingBufferIndex;
    HANDLE CurrentGameInputRecordingFileHandle;
    
    u32 CurrentGameInputPlaybackBufferIndex;
    HANDLE CurrentInputPlaybackFileHandle;

    char ExeFilePath[WIN32_FILENAME_LENGTH];
    char *ExeFilePathOnePastLastSlash;
};
