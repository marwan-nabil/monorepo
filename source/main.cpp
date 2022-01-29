#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#include <Windows.h>
#include <stdint.h>
#include <math.h>

#include "platform_types.h"
#include "utility_macros.h"
#include "constants.h"

#include "math/math.cpp"
#include "physics.cpp"
#include "simulation.cpp"
#include "renderer.cpp"

struct window_data
{
    BITMAPINFO *OffscreenBufferBitmapInfo;
    render_buffer *OffscreenRenderBuffer;
    i32 *RunningState;
};

void
DisplayRenderBufferInWindow
(
    HWND Window, HDC DeviceContext, 
    render_buffer *Buffer, BITMAPINFO *OffscreenBufferBitmapInfo
)
{
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);

    i32 WindowWidth = ClientRect.right - ClientRect.left;
    i32 WindowHeight = ClientRect.bottom - ClientRect.top;

    if 
    (
        (WindowWidth == 1280) && (WindowHeight == 800)
    )
    {
        i32 DestinationWidth = (i32)(Buffer->Width * 1.3);
        i32 DestinationHeight = (i32)(Buffer->Height * 1.3);
        StretchDIBits
        (
            DeviceContext,
            0, 0, DestinationWidth, DestinationHeight,
            0, 0, Buffer->Width, Buffer->Height,
            Buffer->Memory, OffscreenBufferBitmapInfo, DIB_RGB_COLORS, SRCCOPY
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
            Buffer->Memory, OffscreenBufferBitmapInfo, DIB_RGB_COLORS, SRCCOPY
        );
    }
}

LRESULT CALLBACK
MainWindowCallback
(
    HWND Window, UINT Message,
    WPARAM WParam, LPARAM LParam
)
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
        window_data *WindowData = (window_data *)GetWindowLongPtr(Window, GWLP_USERDATA);
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(Window, &Paint);
        DisplayRenderBufferInWindow
        (
            Window, DeviceContext,
            WindowData->OffscreenRenderBuffer,
            WindowData->OffscreenBufferBitmapInfo
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
        window_data *WindowData = (window_data *)GetWindowLongPtr(Window, GWLP_USERDATA);
        *WindowData->RunningState = false;
    } break;

    default:
    {
        Result = DefWindowProcA(Window, Message, WParam, LParam);
    } break;
    }

    return Result;
}

void
ResizeRenderBuffer(render_buffer *Buffer, BITMAPINFO *OffscreenBufferBitmapInfo, i32 Width, i32 Height)
{
    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;
    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;

    OffscreenBufferBitmapInfo->bmiHeader.biSize = sizeof(OffscreenBufferBitmapInfo->bmiHeader);
    OffscreenBufferBitmapInfo->bmiHeader.biWidth = Buffer->Width;
    OffscreenBufferBitmapInfo->bmiHeader.biHeight = -Buffer->Height;
    OffscreenBufferBitmapInfo->bmiHeader.biPlanes = 1;
    OffscreenBufferBitmapInfo->bmiHeader.biBitCount = 32;
    OffscreenBufferBitmapInfo->bmiHeader.biCompression = BI_RGB;

    if (Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    i32 BitmapMemorySize = Buffer->Width * Buffer->Height * Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc
    (
        0, BitmapMemorySize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
}

void
ProcessPendingMessages()
{
    MSG Message;
    while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        // NOTE: we could intercept messages here before they go to the WindowProc
        //switch (Message.message)
        //{
        //default:
        //{
        TranslateMessage(&Message);
        DispatchMessage(&Message);
        //} break;
        //}
    }
}

void
UpdateSimulationStateAndRender(render_buffer *Buffer, f32 TimeDelta, simulation_state *SimulationState)
{
    UpdateSimulation(TimeDelta, SimulationState);
    RenderSimulation(Buffer, SimulationState);
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
SleepRestOfFrame(LARGE_INTEGER FrameStartTimerValue, i64 WindowsTimerFrequency, f32 TargetSecondsPerFrame)
{
    f32 SecondsElapsedForFrame = GetSecondsElapsed(FrameStartTimerValue, GetWindowsTimerValue(), WindowsTimerFrequency);
    if (SecondsElapsedForFrame < TargetSecondsPerFrame)
    {
        DWORD SleepMilliSeconds = (DWORD)(1000.0f * (TargetSecondsPerFrame - SecondsElapsedForFrame));
        if (SleepMilliSeconds > 0)
        {
            Sleep(SleepMilliSeconds);
        }

        do
        {
            SecondsElapsedForFrame = GetSecondsElapsed(FrameStartTimerValue, GetWindowsTimerValue(), WindowsTimerFrequency);
        } while (SecondsElapsedForFrame < TargetSecondsPerFrame);
    }
    else
    {
        // NOTE: frame missed
    }
}

i32
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
        LPSTR CmdLine, i32 ShowCmd)
{
    i32 RunningState = false;
    i64 WindowsTimerFrequency;
    render_buffer OffscreenRenderBuffer = {};
    BITMAPINFO OffscreenBufferBitmapInfo = {};
    simulation_state SimulationState = {};

    WNDCLASSA WindowClass1 = {};
    WindowClass1.style = CS_VREDRAW | CS_HREDRAW;
    WindowClass1.lpfnWndProc = MainWindowCallback;
    WindowClass1.hInstance = Instance;
    WindowClass1.lpszClassName = "VoidWindowClass";
    WindowClass1.hCursor = LoadCursor(0, IDC_ARROW);

    LARGE_INTEGER ProfileCounterFrequency;
    QueryPerformanceFrequency(&ProfileCounterFrequency);
    WindowsTimerFrequency = ProfileCounterFrequency.QuadPart;

    if (RegisterClassA(&WindowClass1))
    {
        window_data WindowData = {};
        WindowData.OffscreenRenderBuffer = &OffscreenRenderBuffer;
        WindowData.OffscreenBufferBitmapInfo = &OffscreenBufferBitmapInfo;
        WindowData.RunningState = &RunningState;

        HWND Window = CreateWindowExA
        (
            0, WindowClass1.lpszClassName,
            "Void",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            0, 0, Instance, &WindowData
        );

        if (Window)
        {
            i32 MonitorRefreshHz = 60;
            f32 RendererRefreshHz = (f32)MonitorRefreshHz;
            f32 TargetSecondsPerFrame = 1.0f / (f32)RendererRefreshHz;

            ResizeRenderBuffer(&OffscreenRenderBuffer, &OffscreenBufferBitmapInfo, 960, 540);

            RunningState = true;
            while (RunningState)
            {
                LARGE_INTEGER FrameStartTimerValue = GetWindowsTimerValue();

                ProcessPendingMessages();

                UpdateSimulationStateAndRender(&OffscreenRenderBuffer, TargetSecondsPerFrame, &SimulationState);

                HDC DeviceContext = GetDC(Window);
                DisplayRenderBufferInWindow(Window, DeviceContext, &OffscreenRenderBuffer, &OffscreenBufferBitmapInfo);
                ReleaseDC(Window, DeviceContext);

                SleepRestOfFrame(FrameStartTimerValue, WindowsTimerFrequency, TargetSecondsPerFrame);
            }
        }
    }
}