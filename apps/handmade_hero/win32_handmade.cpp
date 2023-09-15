#include <windows.h>
#include <Xinput.h>
#include <dsound.h>

#include <malloc.h>
#include <stdio.h>

#include "handmade_platform.h"
#include "win32_handmade.h"

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_
#define DirectSoundCreate DirectSoundCreate_

typedef X_INPUT_GET_STATE(x_input_get_state);
typedef X_INPUT_SET_STATE(x_input_set_state);
typedef DIRECT_SOUND_CREATE(direct_sound_create);

X_INPUT_GET_STATE(XInputGetStateStub);
X_INPUT_SET_STATE(XInputSetStateStub);

global_variable b32 GlobalRunning;
global_variable b32 GlobalPause;
global_variable win32_pixel_buffer GlobalBackBuffer;
global_variable LPDIRECTSOUNDBUFFER GlobalSecondarySoundBuffer;
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
global_variable b32 GlobalDebugShowCursor;
global_variable WINDOWPLACEMENT GlobalPreviousWindowPlacement = {sizeof(GlobalPreviousWindowPlacement)};

X_INPUT_GET_STATE(XInputGetStateStub)
//DWORD WINAPI XInputGetStateStub(DWORD dwUserIndex, XINPUT_STATE *pState)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}

X_INPUT_SET_STATE(XInputSetStateStub)
//DWORD WINAPI XInputSetStateStub(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}

inline u32
SafeTruncateUint64ToUint32(u64 Value)
{
    Assert(Value <= 0xFFFFFFFF);
    return (u32)Value;
}

internal void
CatStrings(size_t SourceASize, size_t SourceBSize, size_t DestinationSize,
           char *SourceA, char *SourceB, char *Destination)
{
    Assert((SourceASize + SourceBSize) < DestinationSize);

    for (u32 Index = 0; Index < SourceASize; Index++)
    {
        *Destination++ = *SourceA++;
    }

    for (u32 Index = 0; Index < SourceBSize; Index++)
    {
        *Destination++ = *SourceB++;
    }

    *Destination++ = '\0';
}

internal void
Win32GetEXEFilePath(win32_state *Win32LayerState)
{
    u32 ExeFilePathSize = GetModuleFileNameA(NULL, Win32LayerState->ExeFilePath, sizeof(Win32LayerState->ExeFilePath));
    Win32LayerState->ExeFilePathOnePastLastSlash = Win32LayerState->ExeFilePath;
    
    for (char *ScanedCharacter = Win32LayerState->ExeFilePath; *ScanedCharacter; ScanedCharacter++)
    {
        if (*ScanedCharacter == '\\')
        {
            Win32LayerState->ExeFilePathOnePastLastSlash = ScanedCharacter + 1;
        }
    }
}

internal u32
StringLength(char *String)
{
    u32 Count = 0;
    while (*String++)
    {
        ++Count;
    }
    return Count;
}

internal void
Win32BuildFilePath(win32_state *Win32State, char *FileName, u32 DestinationBufferSize, char *DestinationBuffer)
{
    size_t FirstStringLength = Win32State->ExeFilePathOnePastLastSlash - Win32State->ExeFilePath;
    CatStrings
    (
        FirstStringLength, StringLength(FileName), DestinationBufferSize,
        Win32State->ExeFilePath, FileName, DestinationBuffer
    );
}

DEBUG_PLATFORM_FREE_FILE_MEMORY(DebugPlatformFreeFileMemory)
//void name(thread_context *Thread, void *FileMemory)
{
    if (FileMemory)
    {
        VirtualFree(FileMemory, 0, MEM_RELEASE);
    }
}

DEBUG_PLATFORM_READ_ENTIRE_FILE(DebugPlatformReadEntireFile)
//debug_read_file_result DebugPlatformReadEntireFile(thread_context *Thread, char *FileName)
{
    debug_read_file_result Result = {};

    HANDLE FileHandle = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (FileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if (GetFileSizeEx(FileHandle, &FileSize))
        {
            void *FileMemory = VirtualAlloc(0, FileSize.QuadPart, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (FileMemory)
            {
                DWORD BytesRead;
                u32 FileSizeU32 = SafeTruncateUint64ToUint32(FileSize.QuadPart);
                if (ReadFile(FileHandle, FileMemory, FileSizeU32, &BytesRead, 0) && (FileSizeU32 == BytesRead))
                {
                    Result.FileContents = FileMemory;
                    Result.ConstentsSize = FileSizeU32;
                }
                else
                {
                    DebugPlatformFreeFileMemory(Thread, FileMemory);
                }
            }
        }

        CloseHandle(FileHandle);
    }

    return Result;
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DebugPlatformWriteEntireFile)
//b32 DebugPlatformWriteEntireFile(thread_context *Thread, char *FileName, void *DataToWrite, u32 DataSize)
{
    b32 Result = false;

    HANDLE FileHandle = CreateFileA(FileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD BytesWritten;
        if (WriteFile(FileHandle, DataToWrite, DataSize, &BytesWritten, 0))
        {
            Result = (DataSize == BytesWritten);
        }

        CloseHandle(FileHandle);
    }

    return Result;
}

internal FILETIME
Win32GetFileLastWriteTime(char *FileName)
{
    FILETIME Result = {};

    WIN32_FILE_ATTRIBUTE_DATA FileAttributes;
    if (GetFileAttributesExA(FileName, GetFileExInfoStandard, &FileAttributes))
    {
        Result = FileAttributes.ftLastWriteTime;
    }

    return Result;
}

internal win32_game_code
Win32LoadGameCode(char *GameCodeSourceDLLPath, char *GameCodeTempDLLPath)
{
    win32_game_code Result = {};

    CopyFileA(GameCodeSourceDLLPath, GameCodeTempDLLPath, FALSE);

    Result.LastWriteTimeForLoadedDLL = Win32GetFileLastWriteTime(GameCodeTempDLLPath);
    Result.GameCodeDll = LoadLibraryA(GameCodeTempDLLPath);

    if (Result.GameCodeDll)
    {
        Result.UpdateAndRender = (game_update_and_render_function *)GetProcAddress(Result.GameCodeDll, "GameUpdateAndRender");
        Result.GetSoundSamples = (game_get_sound_samples_function *)GetProcAddress(Result.GameCodeDll, "GameGetSoundSamples");
        Result.IsValid = Result.UpdateAndRender && Result.GetSoundSamples;
    }

    if (!Result.IsValid)
    {
        Result.UpdateAndRender = 0;
        Result.GetSoundSamples = 0;
    }

    return Result;
}

internal void
Win32UnloadGameCode(win32_game_code *GameCode)
{
    if (GameCode->GameCodeDll)
    {
        FreeLibrary(GameCode->GameCodeDll);
    }

    GameCode->IsValid = false;
    GameCode->GameCodeDll = NULL;
    GameCode->UpdateAndRender = 0;
    GameCode->GetSoundSamples = 0;
    GameCode->LastWriteTimeForLoadedDLL = {};
}

internal void
Win32LoadXInput()
{
    HMODULE XInputDLL = LoadLibraryA("xinput1_4.dll");
    if (!XInputDLL)
    {
        XInputDLL = LoadLibraryA("xinput1_3.dll");
    }
    if (!XInputDLL)
    {
        XInputDLL = LoadLibraryA("xinput9_1_0.dll");
    }

    if (XInputDLL)
    {
        XInputGetState_ = (x_input_get_state *)GetProcAddress(XInputDLL, "XInputGetState");
        if (!XInputGetState_)
        {
            XInputGetState_ = XInputGetStateStub;
        }

        XInputSetState_ = (x_input_set_state *)GetProcAddress(XInputDLL, "XInputSetState");
        if (!XInputSetState_)
        {
            XInputSetState_ = XInputSetStateStub;
        }
    }
}

internal void
Win32InitDSound(HWND Window, win32_sound_configuration *SoundBufferConfiguratoin, LPDIRECTSOUNDBUFFER *SecondarySoundBuffer)
{
    HMODULE DSoundDLL = LoadLibraryA("dsound.dll");
    if (DSoundDLL)
    {
        direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(DSoundDLL, "DirectSoundCreate");

        LPDIRECTSOUND DirectSound;
        if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
        {
            WAVEFORMATEX WaveFormat = {};
            WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
            WaveFormat.nChannels = 2;
            WaveFormat.nSamplesPerSec = SoundBufferConfiguratoin->SamplesPerSecond;
            WaveFormat.wBitsPerSample = 16;
            WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
            WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
            WaveFormat.cbSize = 0;

            if (SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
            {
                LPDIRECTSOUNDBUFFER PrimarySoundBuffer;
                DSBUFFERDESC PrimaryBufferDescription = {};
                PrimaryBufferDescription.dwSize = sizeof(PrimaryBufferDescription);
                PrimaryBufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
                if (SUCCEEDED(DirectSound->CreateSoundBuffer(&PrimaryBufferDescription, &PrimarySoundBuffer, 0)))
                {
                    if (SUCCEEDED(PrimarySoundBuffer->SetFormat(&WaveFormat)))
                    {
                        OutputDebugStringA("Primary Sound Buffer Created And Fromat Set!\n");
                    }
                }
            }

            DSBUFFERDESC SecondaryBufferDescription = {};
            SecondaryBufferDescription.dwSize = sizeof(SecondaryBufferDescription);
            SecondaryBufferDescription.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
            SecondaryBufferDescription.dwBufferBytes = SoundBufferConfiguratoin->SoundBufferSize;
            SecondaryBufferDescription.lpwfxFormat = &WaveFormat;
            if (SUCCEEDED(DirectSound->CreateSoundBuffer(&SecondaryBufferDescription, SecondarySoundBuffer, 0)))
            {
                OutputDebugStringA("Seocndary Sound Buffer Created!\n");
            }
        }
    }
}

internal void
Win32ResizePixelBuffer(win32_pixel_buffer *PixelBuffer, i32 NewWidthInPixels, i32 NewHeightInPixels)
{
    PixelBuffer->WidthInPixels = NewWidthInPixels;
    PixelBuffer->HeightInPixels = NewHeightInPixels;
    PixelBuffer->BytesPerPixel = 4;
    PixelBuffer->BytesPerRow = PixelBuffer->WidthInPixels * PixelBuffer->BytesPerPixel;

    PixelBuffer->BitmapInfo.bmiHeader.biSize = sizeof(PixelBuffer->BitmapInfo.bmiHeader);
    PixelBuffer->BitmapInfo.bmiHeader.biWidth = PixelBuffer->WidthInPixels;
    PixelBuffer->BitmapInfo.bmiHeader.biHeight = -PixelBuffer->HeightInPixels;
    PixelBuffer->BitmapInfo.bmiHeader.biPlanes = 1;
    PixelBuffer->BitmapInfo.bmiHeader.biBitCount = 32;
    PixelBuffer->BitmapInfo.bmiHeader.biCompression = BI_RGB;

    if (PixelBuffer->PixelsMemory)
    {
        VirtualFree(PixelBuffer->PixelsMemory, 0, MEM_RELEASE);
    }

    i32 BitmapMemorySize = PixelBuffer->WidthInPixels * PixelBuffer->HeightInPixels * PixelBuffer->BytesPerPixel;
    PixelBuffer->PixelsMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    // TODO: clear bitmap memory to black
}

internal void
Win32DisplayBufferInWindow(HDC DeviceContext, win32_window_dimensions WindowDimensions, win32_pixel_buffer *PixelBuffer)
{
    if ((WindowDimensions.WidthInPixels >= 1280) && (WindowDimensions.HeightInPixels >= 800))
    {
        // TODO: center when in full screen mode
        i32 DestinationWidth = (i32)(PixelBuffer->WidthInPixels * 1.3);
        i32 DestinationHeight = (i32)(PixelBuffer->HeightInPixels * 1.3);
        StretchDIBits(DeviceContext,
                      0, 0, DestinationWidth, DestinationHeight,
                      0, 0, PixelBuffer->WidthInPixels, PixelBuffer->HeightInPixels,
                      PixelBuffer->PixelsMemory, &PixelBuffer->BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
    }
    else
    {
        i32 OffsetX = 10;
        i32 OffsetY = 10;
        PatBlt(DeviceContext, 0, 0, WindowDimensions.WidthInPixels, OffsetY, BLACKNESS);
        PatBlt(DeviceContext, 0, OffsetY + PixelBuffer->HeightInPixels, WindowDimensions.WidthInPixels, WindowDimensions.HeightInPixels, BLACKNESS);
        PatBlt(DeviceContext, 0, 0, OffsetX, WindowDimensions.HeightInPixels, BLACKNESS);
        PatBlt(DeviceContext, OffsetX + PixelBuffer->WidthInPixels, 0, WindowDimensions.WidthInPixels, WindowDimensions.HeightInPixels, BLACKNESS);

        StretchDIBits(DeviceContext,
                      OffsetX, OffsetY, PixelBuffer->WidthInPixels, PixelBuffer->HeightInPixels,
                      0, 0, PixelBuffer->WidthInPixels, PixelBuffer->HeightInPixels,
                      PixelBuffer->PixelsMemory, &PixelBuffer->BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
    }
}

internal win32_window_dimensions
Win32GetWindowDimensions(HWND Window)
{
    win32_window_dimensions Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.WidthInPixels = ClientRect.right - ClientRect.left;
    Result.HeightInPixels = ClientRect.bottom - ClientRect.top;
    return Result;
}

internal void
Win32FillGlobalSoundBuffer
(
    win32_sound_configuration *SoundBufferConfiguration,
    win32_sound_state *SoundBufferState,
    DWORD ByteToLock,
    DWORD BytesToWrite,
    LPDIRECTSOUNDBUFFER SecondarySoundBuffer,
    game_sound_request *GameSoundBuffer
)
{
    void *BufferRegion1;
    DWORD BufferRegion1Size;
    void *BufferRegion2;
    DWORD BufferRegion2Size;
    
    if 
    (
        SUCCEEDED
        (
            SecondarySoundBuffer->Lock
            (
                ByteToLock, BytesToWrite, 
                &BufferRegion1, &BufferRegion1Size, 
                &BufferRegion2, &BufferRegion2Size, 
                0
            )
        )
    )
    {
        // TODO: assert that the returned region sizes are valid (multiples of 4 bytes)

        i16 *SourceHalfSample = GameSoundBuffer->SamplesMemory;
        i16 *DestinationHalfSample = (i16 *)BufferRegion1;
        u32 Region1SampleCount = BufferRegion1Size / SoundBufferConfiguration->BytesPerSample;

        for(DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; SampleIndex++)
        {
            *DestinationHalfSample++ = *SourceHalfSample++;
            *DestinationHalfSample++ = *SourceHalfSample++;
            SoundBufferState->RunningSampleIndex++;
        }

        DestinationHalfSample = (i16 *)BufferRegion2;
        u32 Region2SampleCount = BufferRegion2Size / SoundBufferConfiguration->BytesPerSample;

        for (DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; SampleIndex++)
        {
            *DestinationHalfSample++ = *SourceHalfSample++;
            *DestinationHalfSample++ = *SourceHalfSample++;
            SoundBufferState->RunningSampleIndex++;
        }

        SecondarySoundBuffer->Unlock(BufferRegion1, BufferRegion1Size, BufferRegion2, BufferRegion2Size);
    }
}

internal void
Win32ClearSoundBuffer(win32_sound_configuration *SoundBufferConfiguration, LPDIRECTSOUNDBUFFER SecondarySoundBuffer)
{
    void *BufferRegion1;
    DWORD BufferRegion1Size;
    void *BufferRegion2;
    DWORD BufferRegion2Size;

    if
    (
        SUCCEEDED
        (
            SecondarySoundBuffer->Lock
            (
                0, SoundBufferConfiguration->SoundBufferSize, 
                &BufferRegion1, &BufferRegion1Size, 
                &BufferRegion2, &BufferRegion2Size, 
                0
            )
        )
    )
    {
        u8 *DestinationByte = (u8 *)BufferRegion1;
        for (DWORD ByteIndex = 0; ByteIndex < BufferRegion1Size; ByteIndex++)
        {
            *DestinationByte++ = 0;
        }

        DestinationByte = (u8 *)BufferRegion2;

        for (DWORD ByteIndex = 0; ByteIndex < BufferRegion2Size; ByteIndex++)
        {
            *DestinationByte++ = 0;
        }

        SecondarySoundBuffer->Unlock(BufferRegion1, BufferRegion1Size, BufferRegion2, BufferRegion2Size);
    }
}

internal void
Win32ProcessButton(game_button_state *NewButtonState, b32 IsDown)
{
    if (NewButtonState->IsDown != IsDown)
    {
        NewButtonState->IsDown = IsDown;
        ++NewButtonState->TransitionsCount;
    }
}

LRESULT CALLBACK
Win32MainWindowCallback
(
    HWND Window, UINT Message,
    WPARAM WParam, LPARAM LParam
)
{
    LRESULT Result = 0;

    switch (Message)
    {
        case WM_ACTIVATEAPP:
        {
            //u8 Alpha = 255;
            //if (WParam == false)
            //{
            //    Alpha = 128;
            //}
            //SetLayeredWindowAttributes(MainWindow, RGB(0, 0, 0), Alpha, LWA_ALPHA);
        } break;

        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
        } break;

        case WM_SETCURSOR:
        {
            if (GlobalDebugShowCursor)
            {
                DefWindowProcA(Window, Message, WParam, LParam);
            }
            else
            {
                SetCursor(0);
            }
        } break;

        case WM_PAINT:
        {
            OutputDebugStringA("WM_PAINT\n");
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            Win32DisplayBufferInWindow(DeviceContext, Win32GetWindowDimensions(Window), &GlobalBackBuffer);
            EndPaint(Window, &Paint);
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            Assert(false);
        } break;

        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
            GlobalRunning = false;
        } break;

        case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");
            GlobalRunning = false;
        } break;

        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }

    return Result;
}

internal f32
Win32ProcessStickDeadzone(SHORT StickValueIn, SHORT DeadzoneThreshold)
{
    f32 StickValueOut;

    if (StickValueIn < -DeadzoneThreshold)
    {
        StickValueOut = (f32)(StickValueIn + DeadzoneThreshold) / (32768.0f - (f32)DeadzoneThreshold);
    }
    else if (StickValueIn > DeadzoneThreshold)
    {
        StickValueOut = (f32)(StickValueIn - DeadzoneThreshold) / (32767.0f - (f32)DeadzoneThreshold);
    }
    else
    {
        StickValueOut = 0.0f;
    }

    return StickValueOut;
}

internal void
Win32CaptureGamepadControllerSample
(
    game_controller_state *NewControllerState, 
    game_controller_state *OldControllerState, 
    XINPUT_STATE *SampledControllerState
)
{
    NewControllerState->IsConnected = true;
    NewControllerState->MovementIsAnalog = OldControllerState->MovementIsAnalog;

    XINPUT_GAMEPAD *GamePadSample = &SampledControllerState->Gamepad;

    NewControllerState->StickAverageX = Win32ProcessStickDeadzone(GamePadSample->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    NewControllerState->StickAverageY = Win32ProcessStickDeadzone(GamePadSample->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

    if
    (
        (NewControllerState->StickAverageX != 0.0f) ||
        (NewControllerState->StickAverageY != 0.0f)
    )
    {
        NewControllerState->MovementIsAnalog = true;
    }

    if (GamePadSample->wButtons & XINPUT_GAMEPAD_DPAD_UP)
    {
        NewControllerState->StickAverageY = 1.0f;
        NewControllerState->MovementIsAnalog = false;
    }
    if (GamePadSample->wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
    {
        NewControllerState->StickAverageY = -1.0f;
        NewControllerState->MovementIsAnalog = false;
    }
    if (GamePadSample->wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
    {
        NewControllerState->StickAverageX = 1.0f;
        NewControllerState->MovementIsAnalog = false;
    }
    if (GamePadSample->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
    {
        NewControllerState->StickAverageX = 1.0f;
        NewControllerState->MovementIsAnalog = false;
    }

    f32 AnalogStickDigitalThreshold = 0.5f;

    Win32ProcessButton
    (
        &NewControllerState->MoveLeft, 
        ((NewControllerState->StickAverageX < -AnalogStickDigitalThreshold) ? 1 : 0)
    );
    Win32ProcessButton
    (
        &NewControllerState->MoveRight, 
        ((NewControllerState->StickAverageX > AnalogStickDigitalThreshold) ? 1 : 0)
    );
    Win32ProcessButton
    (
        &NewControllerState->MoveUp, 
        ((NewControllerState->StickAverageY > AnalogStickDigitalThreshold) ? 1 : 0)
    );
    Win32ProcessButton
    (
        &NewControllerState->MoveDown, 
        ((NewControllerState->StickAverageY < -AnalogStickDigitalThreshold) ? 1 : 0)
    );

    Win32ProcessButton
    (
        &NewControllerState->ActionUp, 
        ((GamePadSample->wButtons & XINPUT_GAMEPAD_Y) == XINPUT_GAMEPAD_Y)
    );
    Win32ProcessButton
    (
        &NewControllerState->ActionDown, 
        ((GamePadSample->wButtons & XINPUT_GAMEPAD_A) == XINPUT_GAMEPAD_A)
    );
    Win32ProcessButton
    (
        &NewControllerState->ActionLeft, 
        ((GamePadSample->wButtons & XINPUT_GAMEPAD_X) == XINPUT_GAMEPAD_X) 
    );
    Win32ProcessButton
    (
        &NewControllerState->ActionRight, 
        ((GamePadSample->wButtons & XINPUT_GAMEPAD_B) == XINPUT_GAMEPAD_B) 
    );
    Win32ProcessButton
    (
        &NewControllerState->LeftShoulder, 
        ((GamePadSample->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) == XINPUT_GAMEPAD_LEFT_SHOULDER)
    );
    Win32ProcessButton
    (
        &NewControllerState->RightShoulder, 
        ((GamePadSample->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) == XINPUT_GAMEPAD_RIGHT_SHOULDER)
    );
    Win32ProcessButton
    (
        &NewControllerState->Start, 
        ((GamePadSample->wButtons & XINPUT_GAMEPAD_START) == XINPUT_GAMEPAD_START)
    );
    Win32ProcessButton
    (
        &NewControllerState->Back, 
        ((GamePadSample->wButtons & XINPUT_GAMEPAD_BACK) == XINPUT_GAMEPAD_BACK)
    );
}

internal void
Win32GetInputOrStateFileLocation(i32 SlotIndex, b32 IsInputStream, win32_state *Win32State, u32 DestSize, char *Dest)
{
    char Temp[64];
    sprintf(Temp, "loop_edit_%d_%s.hmi", SlotIndex, IsInputStream ? "input" : "state");
    Win32BuildFilePath(Win32State, Temp, DestSize, Dest);
}

internal win32_game_input_buffer *
Win32GetGameInputBuffer(win32_state *Win32State, u32 Index)
{
    Assert(Index > 0);
    Assert(Index < ArrayCount(Win32State->GameInputBuffers));

    win32_game_input_buffer *GameInputBuffer = &Win32State->GameInputBuffers[Index];
    return GameInputBuffer;
}

internal void
Win32BeginRecordingInput(win32_state *Win32State, u32 CurrentGameInputRecordingBufferIndex)
{
    win32_game_input_buffer *GameInputBuffer = Win32GetGameInputBuffer(Win32State, CurrentGameInputRecordingBufferIndex);
    if (GameInputBuffer->MemoryBlock)
    {
        Win32State->CurrentGameInputRecordingBufferIndex = CurrentGameInputRecordingBufferIndex;

        char GameInputRecordingFileName[WIN32_FILENAME_LENGTH];
        Win32GetInputOrStateFileLocation
        (
            CurrentGameInputRecordingBufferIndex, true, Win32State, 
            sizeof(GameInputRecordingFileName), GameInputRecordingFileName
        );

        Win32State->CurrentGameInputRecordingFileHandle = CreateFileA(GameInputRecordingFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

        CopyMemory(GameInputBuffer->MemoryBlock, Win32State->GameMemoryBlock, Win32State->GameMemoryBlockSize);
    }
}

internal void
Win32EndRecordingInput(win32_state *Win32State)
{
    Win32State->CurrentGameInputRecordingBufferIndex = 0;
    CloseHandle(Win32State->CurrentGameInputRecordingFileHandle);
    Win32State->CurrentGameInputRecordingFileHandle = NULL;
}

internal void
Win32BeginInputPlayback(win32_state *Win32State, u32 CurrentGameInputPlaybackBufferIndex)
{
    win32_game_input_buffer *GameInputBuffer = Win32GetGameInputBuffer(Win32State, CurrentGameInputPlaybackBufferIndex);
    if (GameInputBuffer->MemoryBlock)
    {
        Win32State->CurrentGameInputPlaybackBufferIndex = CurrentGameInputPlaybackBufferIndex;

        char CurrentGameInputPlaybackFileName[WIN32_FILENAME_LENGTH];
        Win32GetInputOrStateFileLocation
        (
            CurrentGameInputPlaybackBufferIndex, true, Win32State, 
            sizeof(CurrentGameInputPlaybackFileName), CurrentGameInputPlaybackFileName
        );

        Win32State->CurrentInputPlaybackFileHandle = CreateFileA(CurrentGameInputPlaybackFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);

        CopyMemory(Win32State->GameMemoryBlock, GameInputBuffer->MemoryBlock, Win32State->GameMemoryBlockSize);
    }
}

internal void
Win32EndInputPlayback(win32_state *Win32State)
{
    Win32State->CurrentGameInputPlaybackBufferIndex = 0;
    CloseHandle(Win32State->CurrentInputPlaybackFileHandle);
}

internal void
Win32RecordInput(win32_state *Win32State, game_input *GameInput)
{
    DWORD BytesWritten;
    WriteFile(Win32State->CurrentGameInputRecordingFileHandle, GameInput, sizeof(game_input), &BytesWritten, 0);
}

internal void
Win32PlaybackInput(win32_state *Win32State, game_input *GameInput)
{
    DWORD BytesRead;
    if (ReadFile(Win32State->CurrentInputPlaybackFileHandle, GameInput, sizeof(game_input), &BytesRead, 0))
    {
        if (BytesRead == 0)
        {
            Win32EndInputPlayback(Win32State);
            Win32BeginInputPlayback(Win32State, Win32State->CurrentGameInputPlaybackBufferIndex);
            BOOL ReadResult = ReadFile(Win32State->CurrentInputPlaybackFileHandle, GameInput, sizeof(game_input), &BytesRead, 0);
        }
    }
}

internal void
Win32ToggleFullScreen(HWND Window)
{
    u32 CurrentWindowStyle = GetWindowLong(Window, GWL_STYLE);

    if (CurrentWindowStyle & WS_OVERLAPPEDWINDOW)
    {
        HMONITOR Monitor = MonitorFromWindow(Window, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO MonitorInfo = {sizeof(MonitorInfo)};
        b32 GetMonitorInfoResult = GetMonitorInfo(Monitor, &MonitorInfo);

        b32 GetWindowPlacementResult = GetWindowPlacement(Window, &GlobalPreviousWindowPlacement);

        if (GetWindowPlacementResult && GetMonitorInfoResult)
        {
            SetWindowLong(Window, GWL_STYLE, CurrentWindowStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos
            (
                Window, HWND_TOP,
                MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top,
                MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
                MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED
            );
        }
    }
    else
    {
        SetWindowLong(Window, GWL_STYLE, CurrentWindowStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(Window, &GlobalPreviousWindowPlacement);
        SetWindowPos
        (
            Window, 0, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED
        );
    }
}

internal void
Win32ProcessPendingMessages(win32_state *Win32State, game_controller_state *KeyboardController)
{
    MSG WindowsMessage;
    while (PeekMessage(&WindowsMessage, 0, 0, 0, PM_REMOVE))
    {
        switch (WindowsMessage.message)
        {
            case WM_QUIT:
            {
                GlobalRunning = false;
            } break;

            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                u32 VirtualKeyCode = (u32)WindowsMessage.wParam;
                b32 KeyWasDown = (WindowsMessage.lParam & (1 << 30)) != 0;
                b32 KeyIsDown = (WindowsMessage.lParam & (1ll << 31)) == 0;
                b32 AltKeyIsDown = (WindowsMessage.lParam & (1 << 29)) != 0;

                if (KeyIsDown != KeyWasDown)
                {
                    if (VirtualKeyCode == 'W')
                    {
                        Win32ProcessButton(&KeyboardController->MoveUp, KeyIsDown);
                    }
                    else if (VirtualKeyCode == 'A')
                    {
                        Win32ProcessButton(&KeyboardController->MoveLeft, KeyIsDown);
                    }
                    else if (VirtualKeyCode == 'S')
                    {
                        Win32ProcessButton(&KeyboardController->MoveDown, KeyIsDown);
                    }
                    else if (VirtualKeyCode == 'D')
                    {
                        Win32ProcessButton(&KeyboardController->MoveRight, KeyIsDown);
                    }
                    else if (VirtualKeyCode == 'Q')
                    {
                        Win32ProcessButton(&KeyboardController->LeftShoulder, KeyIsDown);
                    }
                    else if (VirtualKeyCode == 'E')
                    {
                        Win32ProcessButton(&KeyboardController->RightShoulder, KeyIsDown);
                    }
                    else if (VirtualKeyCode == VK_UP)
                    {
                        Win32ProcessButton(&KeyboardController->ActionUp, KeyIsDown);
                    }
                    else if (VirtualKeyCode == VK_DOWN)
                    {
                        Win32ProcessButton(&KeyboardController->ActionDown, KeyIsDown);
                    }
                    else if (VirtualKeyCode == VK_LEFT)
                    {
                        Win32ProcessButton(&KeyboardController->ActionLeft, KeyIsDown);
                    }
                    else if (VirtualKeyCode == VK_RIGHT)
                    {
                        Win32ProcessButton(&KeyboardController->ActionRight, KeyIsDown);
                    }
                    else if (VirtualKeyCode == VK_ESCAPE)
                    {
                        Win32ProcessButton(&KeyboardController->Back, KeyIsDown);
                        //GlobalRunning = false;
                    }
                    else if (VirtualKeyCode == VK_SPACE)
                    {
                        Win32ProcessButton(&KeyboardController->Start, KeyIsDown);
                    }
#if HANDMADE_INTERNAL
                    else if (VirtualKeyCode == 'P' && KeyIsDown)
                    {
                        GlobalPause = !GlobalPause;
                    }
                    else if (VirtualKeyCode == 'L' && KeyIsDown)
                    {
                        if (Win32State->CurrentGameInputPlaybackBufferIndex == 0)
                        {
                            if (Win32State->CurrentGameInputRecordingBufferIndex == 0)
                            {
                                Win32BeginRecordingInput(Win32State, 1);
                            }
                            else
                            {
                                Win32EndRecordingInput(Win32State);
                                Win32BeginInputPlayback(Win32State, 1);
                            }
                        }
                        else
                        {
                            Win32EndInputPlayback(Win32State);
                        }
                    }
#endif
                    if ((VirtualKeyCode == VK_F4) && KeyIsDown && AltKeyIsDown)
                    {
                        GlobalRunning = false;
                    }
                    else if ((VirtualKeyCode == VK_RETURN) && KeyIsDown && AltKeyIsDown)
                    {
                        if (WindowsMessage.hwnd)
                        {
                            Win32ToggleFullScreen(WindowsMessage.hwnd);
                        }
                    }
                }
            } break;

            default:
            {
                TranslateMessage(&WindowsMessage);
                DispatchMessage(&WindowsMessage);
            } break;
        }
    }
}

#if 0
internal void
Win32DebugDrawVertical(win32_pixel_buffer *BackBuffer, i32 X, i32 Top, i32 Bottom, u32 Color)
{
    if (Top <= 0)
    {
        Top = 0;
    }

    if (Bottom >= BackBuffer->HeightInPixels)
    {
        Bottom = BackBuffer->HeightInPixels - 1;
    }

    if ((X >= 0) && (X < BackBuffer->WidthInPixels))
    {
        u8 *Pixel = ((u8 *)BackBuffer->PixelsMemory +
                     X * BackBuffer->BytesPerPixel +
                     Top * BackBuffer->BytesPerRow);

        for (i32 Y = Top;
             Y < Bottom;
             Y++)
        {
            *(u32 *)Pixel = Color;
            Pixel += BackBuffer->BytesPerRow;
        }
    }
}

internal void
Win32DrawSoundBufferMarker(win32_pixel_buffer *BackBuffer, win32_sound_configuration *SoundBufferState,
                           f32 SoundBufferCoefficient, i32 PadX, i32 Top, i32  Bottom, u32 SoundBufferIndex, u32 Color)
{
    i32 X = PadX + (i32)(SoundBufferCoefficient * (f32)SoundBufferIndex);
    Win32DebugDrawVertical(BackBuffer, X, Top, Bottom, Color);
}

internal void
Win32DebugSyncDisplay(win32_pixel_buffer *BackBuffer, win32_debug_sound_time_marker *TimeMarkers, i32 MarkersCount,
                      win32_sound_configuration *SoundBufferConfiguration, f32 TargetSecondsPerFrame, i32 CurrentMarkerIndex)
{
    i32 PadX = 16;
    i32 PadY = 16;
    i32 LineHeight = 64;

    // AA BB GG RR
    u32 FlipPlayColor = 0xffffffff; // white
    u32 FlipWriteColor = 0xff000000; // black
    u32 OutputPlayColor = 0xffFA90FF; // magenta
    u32 OutputWriteColor = 0xff653468; // dark magenta
    u32 ByteToLockColor = 0xff2AFF00; // green
    u32 TargetCursorColor = 0xff116600; // dark green
    u32 ExpectedFlipColor = 0xffffff00; // yellow

    f32 SoundBufferCoefficient = (f32)(BackBuffer->WidthInPixels - 2 * PadX) / (f32)SoundBufferConfiguration->SoundBufferSize;

    for (int MarkerIndex = 0;
         MarkerIndex < MarkersCount;
         MarkerIndex++)
    {
        win32_debug_sound_time_marker *CurrentMarker = &TimeMarkers[MarkerIndex];
        Assert(CurrentMarker->OutputPlayCursor < SoundBufferConfiguration->SoundBufferSize);
        Assert(CurrentMarker->OutputWriteCursor < SoundBufferConfiguration->SoundBufferSize);
        Assert(CurrentMarker->OutputLocation < SoundBufferConfiguration->SoundBufferSize);
        Assert(CurrentMarker->OutputByteCount < SoundBufferConfiguration->SoundBufferSize);
        Assert(CurrentMarker->FlipPlayCursor < SoundBufferConfiguration->SoundBufferSize);
        Assert(CurrentMarker->FlipWriteCursor < SoundBufferConfiguration->SoundBufferSize);

        i32 Top = PadY;
        i32 Bottom = PadY + LineHeight;

        Win32DrawSoundBufferMarker(BackBuffer, SoundBufferConfiguration, SoundBufferCoefficient, PadX, Top, Bottom,
                                   CurrentMarker->FlipPlayCursor, FlipPlayColor);
                                   //CurrentMarker->FlipPlayCursor + 480 * SoundBufferConfiguration->BytesPerSample, FlipPlayColor);
        Win32DrawSoundBufferMarker(BackBuffer, SoundBufferConfiguration, SoundBufferCoefficient, PadX, Top, Bottom,
                                   CurrentMarker->FlipWriteCursor, FlipWriteColor);

        if (MarkerIndex == CurrentMarkerIndex)
        {
            Top += LineHeight + PadY;
            Bottom += LineHeight + PadY;
            i32 FirstTop = Top;

            Win32DrawSoundBufferMarker(BackBuffer, SoundBufferConfiguration, SoundBufferCoefficient, PadX, Top, Bottom,
                                       CurrentMarker->OutputPlayCursor, OutputPlayColor);
            Win32DrawSoundBufferMarker(BackBuffer, SoundBufferConfiguration, SoundBufferCoefficient, PadX, Top, Bottom,
                                       CurrentMarker->OutputWriteCursor, OutputWriteColor);

            Top += LineHeight + PadY;
            Bottom += LineHeight + PadY;

            Win32DrawSoundBufferMarker(BackBuffer, SoundBufferConfiguration, SoundBufferCoefficient, PadX, Top, Bottom,
                                       CurrentMarker->OutputLocation, ByteToLockColor);
            Win32DrawSoundBufferMarker(BackBuffer, SoundBufferConfiguration, SoundBufferCoefficient, PadX, Top, Bottom,
                                       CurrentMarker->OutputLocation + CurrentMarker->OutputByteCount, TargetCursorColor);

            Top += LineHeight + PadY;
            Bottom += LineHeight + PadY;

            Win32DrawSoundBufferMarker(BackBuffer, SoundBufferConfiguration, SoundBufferCoefficient, PadX, FirstTop, Bottom,
                                       CurrentMarker->ExpectedFlipCursor, ExpectedFlipColor);
        }
    }
}
#endif

inline LARGE_INTEGER
Win32GetWallClock()
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return Result;
}

inline f32
Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End, i64 CounterFrequency)
{
    f32 SecondsElapsed = (f32)(End.QuadPart - Start.QuadPart)
        / (f32)CounterFrequency;
    return SecondsElapsed;
}

int
WinMain
(
    HINSTANCE CurrentApplicationInstance,
    HINSTANCE PreviousApplicationInstance,
    LPSTR CommandLine,
    i32 ShowCmdOptions
)
{
    win32_state Win32LayerState = {};

    Win32LoadXInput();

    LARGE_INTEGER PerformanceCounterFrequency;
    QueryPerformanceFrequency(&PerformanceCounterFrequency);

    Win32GetEXEFilePath(&Win32LayerState);

    char GameCodeDLLPath[WIN32_FILENAME_LENGTH];
    Win32BuildFilePath(&Win32LayerState, (char *)"handmade.dll", sizeof(GameCodeDLLPath), GameCodeDLLPath);

    char TempDLLPath[WIN32_FILENAME_LENGTH];
    Win32BuildFilePath(&Win32LayerState, (char *)"handmade_temp.dll", sizeof(TempDLLPath), TempDLLPath);

    char CompilationLockFilePath[WIN32_FILENAME_LENGTH];
    Win32BuildFilePath(&Win32LayerState, (char *)"compilation.lock", sizeof(CompilationLockFilePath), CompilationLockFilePath);

    b32 SleepIsGranular = (timeBeginPeriod(1) == TIMERR_NOERROR);

#if HANDMADE_INTERNAL
    GlobalDebugShowCursor = true;
#endif

    WNDCLASSA MainWindowClass = {};
    MainWindowClass.style = CS_VREDRAW | CS_HREDRAW;
    MainWindowClass.lpfnWndProc = Win32MainWindowCallback;
    MainWindowClass.hInstance = CurrentApplicationInstance;
    MainWindowClass.lpszClassName = "MainWindowClass";
    MainWindowClass.hCursor = LoadCursorA(0, IDC_ARROW);

    if (RegisterClassA(&MainWindowClass))
    {
        HWND MainWindow = CreateWindowExA
        (
            0, //WS_EX_TOPMOST | WS_EX_LAYERED,
            MainWindowClass.lpszClassName,
            "HandmadeHero",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            0, 0, CurrentApplicationInstance, 0
        );

        if (MainWindow)
        {
            i32 MonitorRefreshHz = 60;

            HDC MainWindowDeviceContextHandle = GetDC(MainWindow);
            i32 Win32RefreshRate = GetDeviceCaps(MainWindowDeviceContextHandle, VREFRESH);
            ReleaseDC(MainWindow, MainWindowDeviceContextHandle);
            
            if (Win32RefreshRate > 1)
            {
                MonitorRefreshHz = Win32RefreshRate; 
            }
            f32 GameRefreshHz = (MonitorRefreshHz / 2.0f);
            f32 TargetSecondsPerFrame = 1.0f / (f32)GameRefreshHz;

            Win32ResizePixelBuffer(&GlobalBackBuffer, 960, 540);

            win32_sound_state SoundBufferState = {};

            win32_sound_configuration SoundBufferConfiguration = {};
            SoundBufferConfiguration.SamplesPerSecond = 48000;
            SoundBufferConfiguration.BytesPerSample = 2 * sizeof(i16);
            SoundBufferConfiguration.SoundBufferSize = 1 * SoundBufferConfiguration.SamplesPerSecond * SoundBufferConfiguration.BytesPerSample;
            u32 SoundBytesProducedPerFrame = (u32)((f32)(SoundBufferConfiguration.SamplesPerSecond * SoundBufferConfiguration.BytesPerSample) / GameRefreshHz);
            SoundBufferConfiguration.SafetyMarginInBytes = (u32)((f32)SoundBytesProducedPerFrame / 3.0f);

            Win32InitDSound(MainWindow, &SoundBufferConfiguration, &GlobalSecondarySoundBuffer);
            Win32ClearSoundBuffer(&SoundBufferConfiguration, GlobalSecondarySoundBuffer);

            GlobalSecondarySoundBuffer->Play(0, 0, DSBPLAY_LOOPING);

            i16 *GameSoundSamples = (i16 *)VirtualAlloc(0, SoundBufferConfiguration.SoundBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

#if HANDMADE_INTERNAL
            LPVOID AllocationBaseAddress = 0;
#else
            LPVOID AllocationBaseAddress = TeraBytes((u64)2);
#endif
            game_memory GameMemory = {};
            GameMemory.IsInitialized = false;
            GameMemory.DebugPlatformFreeFileMemory = DebugPlatformFreeFileMemory;
            GameMemory.DebugPlatformWriteEntireFile = DebugPlatformWriteEntireFile;
            GameMemory.DebugPlatformReadEntireFile = DebugPlatformReadEntireFile;
            GameMemory.PermanentStorageSize = MegaBytes(256);
            GameMemory.TransientStorageSize = MegaBytes((u64)256); // casey has this at 1GB
            
            Win32LayerState.GameMemoryBlockSize = GameMemory.TransientStorageSize + GameMemory.PermanentStorageSize;
            Win32LayerState.GameMemoryBlock = 
                VirtualAlloc(AllocationBaseAddress, (size_t)Win32LayerState.GameMemoryBlockSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            GameMemory.PermanentStorage = Win32LayerState.GameMemoryBlock;
            GameMemory.TransientStorage = (u8 *)GameMemory.PermanentStorage + GameMemory.PermanentStorageSize;

            for
            (
                i32 BufferIndex = 1;
                BufferIndex < ArrayCount(Win32LayerState.GameInputBuffers);
                BufferIndex++
            )
            {
                win32_game_input_buffer *GameInputBuffer = &Win32LayerState.GameInputBuffers[BufferIndex];

                Win32GetInputOrStateFileLocation(BufferIndex, false, &Win32LayerState, sizeof(GameInputBuffer->InputBufferFileName), GameInputBuffer->InputBufferFileName);

                GameInputBuffer->FileHandle =
                    CreateFileA(GameInputBuffer->InputBufferFileName, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

                LARGE_INTEGER MaxSize;
                MaxSize.QuadPart = Win32LayerState.GameMemoryBlockSize;

                GameInputBuffer->MemoryMapHandle = 
                    CreateFileMappingA(GameInputBuffer->FileHandle, 0, PAGE_READWRITE, MaxSize.HighPart, MaxSize.LowPart, 0);

                GameInputBuffer->MemoryBlock = 
                    MapViewOfFile(GameInputBuffer->MemoryMapHandle, FILE_MAP_ALL_ACCESS, 0, 0, Win32LayerState.GameMemoryBlockSize);
            }

            if (GameSoundSamples && Win32LayerState.GameMemoryBlock)
            {
                u32 DebugTimeMarkerIndex = 0;
                win32_debug_sound_time_marker DebugTimeMarkers[30] = {0};
                b32 SoundIsValid = false;

                game_input GameInputDoubleBuffer[2] = {};
                game_input *NewTotalGameInput = &GameInputDoubleBuffer[0];
                game_input *OldTotalGameInput = &GameInputDoubleBuffer[1];

                NewTotalGameInput->TimeDeltaForFrame = TargetSecondsPerFrame;
                OldTotalGameInput->TimeDeltaForFrame = TargetSecondsPerFrame;

                LARGE_INTEGER LastPerfCount = Win32GetWallClock();
                u64 LastCycleCount = __rdtsc();

                win32_game_code GameCode = Win32LoadGameCode(GameCodeDLLPath, TempDLLPath);

                thread_context ThreadContext = {};

                GlobalRunning = true;
                while (GlobalRunning)
                {
                    FILETIME LastWriteTimeForDLL = Win32GetFileLastWriteTime(GameCodeDLLPath);
                    if (CompareFileTime(&LastWriteTimeForDLL, &GameCode.LastWriteTimeForLoadedDLL) != 0)
                    {
                        WIN32_FILE_ATTRIBUTE_DATA IgnoredParameter = {};
                        if (!GetFileAttributesExA(CompilationLockFilePath, GetFileExInfoStandard, &IgnoredParameter))
                        {
                            Win32UnloadGameCode(&GameCode);
                            GameCode = Win32LoadGameCode(GameCodeDLLPath, TempDLLPath);
                        }
                    }
                    
                    game_controller_state *OldKeyboardController = GetController(OldTotalGameInput, 0);
                    game_controller_state *NewKeyboardController = GetController(NewTotalGameInput, 0);
                    *NewKeyboardController = {};

                    DWORD MaxControllerCount = XUSER_MAX_COUNT;
                    if (MaxControllerCount > (ArrayCount(NewTotalGameInput->ControllerStates) - 1))
                    {
                        MaxControllerCount = (ArrayCount(NewTotalGameInput->ControllerStates) - 1);
                    }

                    for
                    (
                        int ButtonIndex = 0;
                        ButtonIndex < ArrayCount(NewKeyboardController->Buttons);
                        ButtonIndex++
                    )
                    {
                        NewKeyboardController->Buttons[ButtonIndex].IsDown = OldKeyboardController->Buttons[ButtonIndex].IsDown;
                    }
                    NewKeyboardController->IsConnected = true;

                    Win32ProcessPendingMessages(&Win32LayerState, NewKeyboardController);

                    if (!GlobalPause)
                    {
                        POINT MousePosition;
                        GetCursorPos(&MousePosition);
                        ScreenToClient(MainWindow, &MousePosition);

                        NewTotalGameInput->MouseX = MousePosition.x;
                        NewTotalGameInput->MouseY = MousePosition.y;
                        NewTotalGameInput->MouseZ = 0;

                        Win32ProcessButton(&NewTotalGameInput->MouseButtons[0], GetKeyState(VK_LBUTTON) & (1 << 15));
                        Win32ProcessButton(&NewTotalGameInput->MouseButtons[1], GetKeyState(VK_MBUTTON) & (1 << 15));
                        Win32ProcessButton(&NewTotalGameInput->MouseButtons[2], GetKeyState(VK_RBUTTON) & (1 << 15));
                        Win32ProcessButton(&NewTotalGameInput->MouseButtons[3], GetKeyState(VK_XBUTTON1) & (1 << 15));
                        Win32ProcessButton(&NewTotalGameInput->MouseButtons[4], GetKeyState(VK_XBUTTON2) & (1 << 15));
                        
                        for (DWORD ControllerIndex = 0; ControllerIndex < MaxControllerCount; ControllerIndex++)
                        {
                            game_controller_state *OldControllerState = GetController(OldTotalGameInput, ControllerIndex + 1);
                            game_controller_state *NewControllerState = GetController(NewTotalGameInput, ControllerIndex + 1);

                            XINPUT_STATE SampledControllerState;
                            if (XInputGetState(ControllerIndex, &SampledControllerState) == ERROR_SUCCESS)
                            {
                                Win32CaptureGamepadControllerSample(NewControllerState, OldControllerState, &SampledControllerState);
                            }
                            else
                            {
                                NewControllerState->IsConnected = false;
                            }
                        }

                        game_pixel_buffer LocalPixelsBuffer = {};
                        LocalPixelsBuffer.PixelsMemory = GlobalBackBuffer.PixelsMemory;
                        LocalPixelsBuffer.WidthInPixels = GlobalBackBuffer.WidthInPixels;
                        LocalPixelsBuffer.HeightInPixels = GlobalBackBuffer.HeightInPixels;
                        LocalPixelsBuffer.BytesPerRow = GlobalBackBuffer.BytesPerRow;
                        LocalPixelsBuffer.BytesPerPixel = GlobalBackBuffer.BytesPerPixel;

                        if (Win32LayerState.CurrentGameInputRecordingBufferIndex)
                        {
                            Win32RecordInput(&Win32LayerState, NewTotalGameInput);
                        }

                        if (Win32LayerState.CurrentGameInputPlaybackBufferIndex)
                        {
                            Win32PlaybackInput(&Win32LayerState, NewTotalGameInput);
                        }

                        if (GameCode.UpdateAndRender)
                        {
                            GameCode.UpdateAndRender(&ThreadContext, &LocalPixelsBuffer, NewTotalGameInput, &GameMemory);
                        }

                        LARGE_INTEGER AudioOutputWallClock = Win32GetWallClock();
                        f32 FromFrameBeginningToAudioSeconds = Win32GetSecondsElapsed(LastPerfCount, AudioOutputWallClock, PerformanceCounterFrequency.QuadPart);

                        DWORD PlayCursor, WriteCursor;
                        if (SUCCEEDED(GlobalSecondarySoundBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor)))
                        {
                            if (!SoundIsValid)
                            {
                                SoundBufferState.RunningSampleIndex = WriteCursor / SoundBufferConfiguration.BytesPerSample;
                                SoundIsValid = true;
                            }

                            DWORD ByteToLock = 
                                (SoundBufferState.RunningSampleIndex * SoundBufferConfiguration.BytesPerSample)
                                % SoundBufferConfiguration.SoundBufferSize;

                            f32 SecondsLeftUntilFlip = TargetSecondsPerFrame - FromFrameBeginningToAudioSeconds;
                            u32 ExpectedBytesUntilFlip = (u32)((f32)SoundBytesProducedPerFrame * SecondsLeftUntilFlip / TargetSecondsPerFrame);
                            u32 ExpectedFrameBoundaryByte = PlayCursor + ExpectedBytesUntilFlip;

                            u32 SafeWriteCursor = WriteCursor;
                            if (SafeWriteCursor < PlayCursor)
                            {
                                SafeWriteCursor += SoundBufferConfiguration.SoundBufferSize;
                            }
                            Assert(SafeWriteCursor > PlayCursor);
                            SafeWriteCursor += SoundBufferConfiguration.SafetyMarginInBytes;

                            b32 AudioCardIsLowLatency = (SafeWriteCursor < ExpectedFrameBoundaryByte);

                            DWORD TargetCursor = 0;
                            if (AudioCardIsLowLatency)
                            {
                                TargetCursor = ExpectedFrameBoundaryByte + SoundBytesProducedPerFrame;
                            }
                            else
                            {
                                TargetCursor = WriteCursor + SoundBufferConfiguration.SafetyMarginInBytes + SoundBytesProducedPerFrame;
                            }
                            TargetCursor = TargetCursor % SoundBufferConfiguration.SoundBufferSize;

                            DWORD BytesToWrite = 0;
                            if (ByteToLock > TargetCursor)
                            {
                                BytesToWrite = (SoundBufferConfiguration.SoundBufferSize - ByteToLock) + TargetCursor;
                            }
                            else
                            {
                                BytesToWrite = TargetCursor - ByteToLock;
                            }

                            game_sound_request GameSoundRequest = {};
                            GameSoundRequest.SamplesPerSecond = SoundBufferConfiguration.SamplesPerSecond;
                            GameSoundRequest.OutputSamplesCount = BytesToWrite / SoundBufferConfiguration.BytesPerSample;
                            GameSoundRequest.SamplesMemory = GameSoundSamples;

                            if (GameCode.GetSoundSamples)
                            {
                                GameCode.GetSoundSamples(&ThreadContext, &GameSoundRequest, &GameMemory);
                            }
                            Win32FillGlobalSoundBuffer(&SoundBufferConfiguration, &SoundBufferState, ByteToLock, BytesToWrite, GlobalSecondarySoundBuffer, &GameSoundRequest);
#if HANDMADE_INTERNAL
                            win32_debug_sound_time_marker *TimeMarker = &DebugTimeMarkers[DebugTimeMarkerIndex];
                            TimeMarker->OutputPlayCursor = PlayCursor;
                            TimeMarker->OutputWriteCursor = WriteCursor;
                            TimeMarker->OutputLocation = ByteToLock;
                            TimeMarker->OutputByteCount = BytesToWrite;
                            TimeMarker->ExpectedFlipCursor = ExpectedFrameBoundaryByte;

                            u32 UnwrappedWriteCursor = WriteCursor;
                            if (UnwrappedWriteCursor < PlayCursor)
                            {
                                UnwrappedWriteCursor += SoundBufferConfiguration.SoundBufferSize;
                            }
                            u32 AudioLatencyInBytes = UnwrappedWriteCursor - PlayCursor;
                            f32 AudioLatencyInSeconds = (f32)AudioLatencyInBytes / (f32)SoundBufferConfiguration.BytesPerSample / (f32)SoundBufferConfiguration.SamplesPerSecond;
#if 0
                            char StringBuffer[512];
                            _snprintf_s(StringBuffer, sizeof(StringBuffer),
                                        "=================================================================\n"
                                        "PlayCursor:     %d   WriteCursor:  %d                            \n"
                                        "ByteToLock:     %d   BytesToWrite: %d   TargetCursor: %d         \n"
                                        "BytesBetweenCursors: %d                 AudioLatencyInSeconds: %f\n"
                                        "=================================================================\n",
                                        PlayCursor, WriteCursor, ByteToLock, BytesToWrite, TargetCursor,
                                        AudioLatencyInBytes, AudioLatencyInSeconds);
                            OutputDebugStringA(StringBuffer);
#endif
#endif // HANDMADE_INTERNAL
                        }
                        else
                        {
                            SoundIsValid = false;
                        }

                        f32 SecondsElapsedForWork = Win32GetSecondsElapsed(LastPerfCount, Win32GetWallClock(), PerformanceCounterFrequency.QuadPart);
                        f32 SecondsElapsedForFrame = SecondsElapsedForWork;
                        if (SecondsElapsedForFrame < TargetSecondsPerFrame)
                        {
                            if (SleepIsGranular)
                            {
                                DWORD SleepMilliSeconds = (DWORD)(1000.0f * (TargetSecondsPerFrame - SecondsElapsedForFrame));
                                if (SleepMilliSeconds > 0)
                                {
                                    Sleep(SleepMilliSeconds);
                                }
                            }

                            SecondsElapsedForFrame = Win32GetSecondsElapsed(LastPerfCount, Win32GetWallClock(), PerformanceCounterFrequency.QuadPart);
                            if (SecondsElapsedForFrame < TargetSecondsPerFrame)
                            {
                                do
                                {
                                    SecondsElapsedForFrame = Win32GetSecondsElapsed(LastPerfCount, Win32GetWallClock(), PerformanceCounterFrequency.QuadPart);
                                } while (SecondsElapsedForFrame < TargetSecondsPerFrame);
                            }
                            else
                            {
                                // TODO: log that a sleep() sleeped too much
                            }
                        }
#if 0
                        f32 MilliSecondsPerFrame = 1000.0f * Win32GetSecondsElapsed(LastPerfCount, Win32GetWallClock(), PerformanceCounterFrequency.QuadPart);
                        f32 FramesPerSeconds = 1000.0f / MilliSecondsPerFrame;
                        u64 CPUCyclesElapsedSinceLastFrame = __rdtsc() - LastCycleCount;
                        char StringBuffer[256];
                        _snprintf_s(StringBuffer, sizeof(StringBuffer), "MilliSeconds Per Frame = %f | FPS = %f | MegaCycles Per Frame = %f\n",
                                    MilliSecondsPerFrame, FramesPerSeconds, (f32)CPUCyclesElapsedSinceLastFrame / 1000000.0f);
                        OutputDebugStringA(StringBuffer);
#endif
                        HDC DeviceContext = GetDC(MainWindow);
                        Win32DisplayBufferInWindow(DeviceContext, Win32GetWindowDimensions(MainWindow), &GlobalBackBuffer);
                        ReleaseDC(MainWindow, DeviceContext);

                        LastPerfCount = Win32GetWallClock();
                        LastCycleCount = __rdtsc();
#if HANDMADE_INTERNAL
                        DWORD FlipPlayCursor, FlipWriteCursor;
                        if (SUCCEEDED(GlobalSecondarySoundBuffer->GetCurrentPosition(&FlipPlayCursor, &FlipWriteCursor)))
                        {
                            Assert(DebugTimeMarkerIndex < ArrayCount(DebugTimeMarkers));
                            win32_debug_sound_time_marker *TimeMarker = &DebugTimeMarkers[DebugTimeMarkerIndex];
                            TimeMarker->FlipPlayCursor = FlipPlayCursor;
                            TimeMarker->FlipWriteCursor = FlipWriteCursor;
                        }
                        DebugTimeMarkerIndex++;
                        if (DebugTimeMarkerIndex == ArrayCount(DebugTimeMarkers))
                        {
                            DebugTimeMarkerIndex = 0;
                        }
#endif
                        game_input *TempGameInput = NewTotalGameInput;
                        NewTotalGameInput = OldTotalGameInput;
                        OldTotalGameInput = TempGameInput;
                    }
                }
            }
        }
    }

    return (0);
}