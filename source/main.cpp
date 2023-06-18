#include <Windows.h>
#include <stdint.h>
#include <math.h>

typedef uint8_t bool8;
typedef uint32_t bool32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

#define Assert(Expression) {if(!(Expression)){ *(int *)0 = 0; }}
#define AbsoluteValue(X) ((u32)(((X) < 0)?-(X):(X)))
#define Assert(Expression) {if(!(Expression)){ *(int *)0 = 0; }}
#define AssertIsBit(Value) Assert(!((Value) & (~1ull)))
#define AssertFits(Value, FittingMask) Assert(!((Value) & (~(FittingMask))))
#define ArrayLength(Array) (sizeof(Array) / sizeof((Array)[0]))

#define PI32 3.1415926535f
#define SQRT2 1.4142135623f

#include "math.cpp"
#include "physics.cpp"
#include "simulation.cpp"
#include "renderer.cpp"

struct window_private_data
{
    BITMAPINFO *LocalBufferBitmapinfo;
    rendering_buffer *LocalRenderingBuffer;
    i32 *RunningState;
};

void
DisplayRenderBufferInWindow(HWND Window, HDC DeviceContext, rendering_buffer *Buffer, BITMAPINFO *LocalBufferBitmapinfo)
{
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);

    i32 WindowWidth = ClientRect.right - ClientRect.left;
    i32 WindowHeight = ClientRect.bottom - ClientRect.top;

    if((WindowWidth == 1280) && (WindowHeight == 800))
    {
        i32 DestinationWidth = (i32)(Buffer->Width * 1.3);
        i32 DestinationHeight = (i32)(Buffer->Height * 1.3);
        StretchDIBits
        (
            DeviceContext,
            0, 0, DestinationWidth, DestinationHeight,
            0, 0, Buffer->Width, Buffer->Height,
            Buffer->Memory, LocalBufferBitmapinfo, DIB_RGB_COLORS, SRCCOPY
        );
    }
    else
    {
        i32 OffsetX = 10;
        i32 OffsetY = 10;
        PatBlt(DeviceContext, 0, 0, WindowWidth, OffsetY, BLACKNESS);
        PatBlt(DeviceContext, 0, OffsetY + Buffer->Height, WindowWidth, WindowHeight, BLACKNESS);
        PatBlt(DeviceContext, 0, 0, OffsetX, WindowHeight, BLACKNESS);
        PatBlt(DeviceContext, OffsetX + Buffer->Width, 0, WindowWidth, WindowHeight, BLACKNESS);

        StretchDIBits
        (
            DeviceContext,
            OffsetX, OffsetY, Buffer->Width, Buffer->Height,
            0, 0, Buffer->Width, Buffer->Height,
            Buffer->Memory, LocalBufferBitmapinfo, DIB_RGB_COLORS, SRCCOPY
        );
    }
}

LRESULT CALLBACK
MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *LocalCreateStruct = (CREATESTRUCT *)LParam;
            SetWindowLongPtr(Window, GWLP_USERDATA, (LONG_PTR)LocalCreateStruct->lpCreateParams);
            SetWindowPos(Window, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
        } break;

        case WM_PAINT:
        {
            window_private_data *WindowPrivateData = (window_private_data *)GetWindowLongPtr(Window, GWLP_USERDATA);
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            DisplayRenderBufferInWindow
            (
                Window, DeviceContext,
                WindowPrivateData->LocalRenderingBuffer,
                WindowPrivateData->LocalBufferBitmapinfo
            );
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
        case WM_QUIT:
        case WM_CLOSE:
        {
            window_private_data *WindowPrivateData = (window_private_data *)GetWindowLongPtr(Window, GWLP_USERDATA);
            *WindowPrivateData->RunningState = false;
        } break;

        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }

    return Result;
}

void
ResizeRenderBuffer(rendering_buffer *Buffer, BITMAPINFO *LocalBufferBitmapinfo, u32 Width, u32 Height)
{
    
}

void
ProcessPendingMessages()
{
    MSG Message;
    while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch (Message.message)
        {
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                // TODO: handle key press
            } break;

            default:
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            } break;
        }
    }
}

inline LARGE_INTEGER
GetWindowsTimerValue()
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return Result;
}

inline f32
GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End, i64 ProfileCounterFrequency)
{
    f32 SecondsElapsed = (f32)(End.QuadPart - Start.QuadPart)
        / (f32)ProfileCounterFrequency;
    return SecondsElapsed;
}

void
SleepRestOfFrame(LARGE_INTEGER FrameStartTime, i64 WindowsTimerFrequency, f32 TargetSecondsPerFrame)
{
    f32 SecondsElapsedForFrame = GetSecondsElapsed(FrameStartTime, GetWindowsTimerValue(), WindowsTimerFrequency);
    if (SecondsElapsedForFrame < TargetSecondsPerFrame)
    {
        DWORD SleepMilliSeconds = (DWORD)(1000.0f * (TargetSecondsPerFrame - SecondsElapsedForFrame));
        if (SleepMilliSeconds > 0)
        {
            Sleep(SleepMilliSeconds);
        }

        do
        {
            SecondsElapsedForFrame = GetSecondsElapsed(FrameStartTime, GetWindowsTimerValue(), WindowsTimerFrequency);
        } while (SecondsElapsedForFrame < TargetSecondsPerFrame);
    }
    else
    {
        // NOTE: frame missed
    }
}

i32
WinMain
(
    HINSTANCE Instance, 
    HINSTANCE PrevInstance,
    LPSTR CmdLine, 
    i32 ShowCmd
)
{
    i32 RunningState = false;
    rendering_buffer LocalRenderingBuffer = {};
    BITMAPINFO LocalBufferBitmapinfo = {};

    WNDCLASSA MainWindowClass = {};
    MainWindowClass.style = CS_VREDRAW | CS_HREDRAW;
    MainWindowClass.lpfnWndProc = MainWindowCallback;
    MainWindowClass.hInstance = Instance;
    MainWindowClass.lpszClassName = "MainWindowClass";
    MainWindowClass.hCursor = LoadCursor(0, IDC_ARROW);

    LARGE_INTEGER ProfileCounterFrequency;
    QueryPerformanceFrequency(&ProfileCounterFrequency);
    i64 WindowsTimerFrequency = ProfileCounterFrequency.QuadPart;

    if (RegisterClassA(&MainWindowClass))
    {
        window_private_data WindowPrivateData = {};
        WindowPrivateData.LocalRenderingBuffer = &LocalRenderingBuffer;
        WindowPrivateData.LocalBufferBitmapinfo = &LocalBufferBitmapinfo;
        WindowPrivateData.RunningState = &RunningState;

        HWND Window = CreateWindowExA
        (
            0, 
            MainWindowClass.lpszClassName,
            "thing",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            0, 
            0, 
            Instance, 
            &WindowPrivateData
        );

        if (Window)
        {
            f32 RendererRefreshHz = 60.0f;
            f32 TargetSecondsPerFrame = 1.0f / RendererRefreshHz;

            LocalRenderingBuffer.Width = 960;
            LocalRenderingBuffer.Height = 540;
            LocalRenderingBuffer.BytesPerPixel = 4;
            LocalRenderingBuffer.Pitch = LocalRenderingBuffer.Width * LocalRenderingBuffer.BytesPerPixel;

            LocalBufferBitmapinfo.bmiHeader.biSize = sizeof(LocalBufferBitmapinfo.bmiHeader);
            LocalBufferBitmapinfo.bmiHeader.biWidth = LocalRenderingBuffer.Width;
            LocalBufferBitmapinfo.bmiHeader.biHeight = -(i32)LocalRenderingBuffer.Height;
            LocalBufferBitmapinfo.bmiHeader.biPlanes = 1;
            LocalBufferBitmapinfo.bmiHeader.biBitCount = 32;
            LocalBufferBitmapinfo.bmiHeader.biCompression = BI_RGB;

            i32 BitmapMemorySize = LocalRenderingBuffer.Width * LocalRenderingBuffer.Height * LocalRenderingBuffer.BytesPerPixel;
            LocalRenderingBuffer.Memory = VirtualAlloc
            (
                0,
                BitmapMemorySize,
                MEM_COMMIT | MEM_RESERVE,
                PAGE_READWRITE
            );

            RunningState = true;
            while (RunningState)
            {
                LARGE_INTEGER FrameStartTime = GetWindowsTimerValue();

                ProcessPendingMessages();
                
                UpdateSimulation(TargetSecondsPerFrame);
                RenderSimulation(&LocalRenderingBuffer);

                HDC DeviceContext = GetDC(Window);
                DisplayRenderBufferInWindow(Window, DeviceContext, &LocalRenderingBuffer, &LocalBufferBitmapinfo);
                ReleaseDC(Window, DeviceContext);

                SleepRestOfFrame(FrameStartTime, WindowsTimerFrequency, TargetSecondsPerFrame);
            }
        }
    }
}
