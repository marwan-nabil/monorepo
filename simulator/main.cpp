#include <Windows.h>
#include <stdint.h>
#include <math.h>

#define ENABLE_ASSERTIONS 1

#include "..\miscellaneous\base_types.h"
#include "..\miscellaneous\assertions.h"
#include "..\miscellaneous\basic_defines.h"
#include "..\miscellaneous\input_utils.h"

#include "..\math\vector2.h"
#include "..\math\vector3.h"
#include "..\math\vector4.h"
#include "..\math\rectangle2.h"

#include "..\software_rendering\software_rendering.h"

#include "simulator.h"

#include "..\math\conversions.cpp"
#include "..\math\floats.cpp"
#include "..\math\integers.cpp"
#include "..\math\vector2.cpp"
#include "..\math\vector4.cpp"
#include "..\math\rectangle2.cpp"

#include "..\miscellaneous\timing_utils.cpp"
#include "..\miscellaneous\input_utils.cpp"

#include "..\software_rendering\software_rendering.cpp"
#include "rendering.cpp"

user_input GlobalUserInput;
simulation_state GlobalSimulationState;

void ElectricPointUpdate(electric_point *Point)
{
    Assert(Point);
    Point->CurrentPotential = Point->NextPotential;
}

void WireUpdate(electric_wire *Wire)
{
    Assert(Wire);
    Assert(Wire->InverseEquillibriumRate);

    ElectricPointUpdate(Wire->A);
    ElectricPointUpdate(Wire->B);

    electric_point *LowerPotentialPoint;
    electric_point *HigherPotentialPoint;

    if (Wire->A->CurrentPotential > Wire->B->CurrentPotential)
    {
        LowerPotentialPoint = Wire->B;
        HigherPotentialPoint = Wire->A;
    }
    else
    {
        LowerPotentialPoint = Wire->A;
        HigherPotentialPoint = Wire->B;
    }

    f32 PotentialDifference = HigherPotentialPoint->CurrentPotential - LowerPotentialPoint->CurrentPotential;
    f32 StepSize = PotentialDifference / Wire->InverseEquillibriumRate;
    HigherPotentialPoint->NextPotential -= StepSize;
    LowerPotentialPoint->NextPotential += StepSize;
}

void UpdateSimulation(f32 TimeDelta, user_input *UserInput, simulation_state *SimulationState)
{
    SimulationState->Up = UserInput->Up;
    SimulationState->Down = UserInput->Down;
    SimulationState->Left = UserInput->Left;
    SimulationState->Right = UserInput->Right;
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
                Window,
                DeviceContext,
                WindowPrivateData->LocalRenderingBuffer
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

i32
WinMain
(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CmdLine,
    i32 ShowCmd
)
{
    b32 RunningState = false;

    rendering_buffer LocalRenderingBuffer = {};
    LocalRenderingBuffer.Width = 1920;
    LocalRenderingBuffer.Height = 1080;
    LocalRenderingBuffer.BytesPerPixel = 4;
    LocalRenderingBuffer.Pitch = LocalRenderingBuffer.Width * LocalRenderingBuffer.BytesPerPixel;

    LocalRenderingBuffer.Bitmapinfo.bmiHeader.biSize = sizeof(LocalRenderingBuffer.Bitmapinfo.bmiHeader);
    LocalRenderingBuffer.Bitmapinfo.bmiHeader.biWidth = LocalRenderingBuffer.Width;
    LocalRenderingBuffer.Bitmapinfo.bmiHeader.biHeight = (LONG)LocalRenderingBuffer.Height;
    LocalRenderingBuffer.Bitmapinfo.bmiHeader.biPlanes = 1;
    LocalRenderingBuffer.Bitmapinfo.bmiHeader.biBitCount = 32;
    LocalRenderingBuffer.Bitmapinfo.bmiHeader.biCompression = BI_RGB;

    LocalRenderingBuffer.Memory = VirtualAlloc
    (
        0,
        LocalRenderingBuffer.Width * LocalRenderingBuffer.Height * LocalRenderingBuffer.BytesPerPixel,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    f32 RendererRefreshHz = 60.0f;
    f32 TargetSecondsPerFrame = 1.0f / RendererRefreshHz;

    LARGE_INTEGER ProfileCounterFrequency;
    QueryPerformanceFrequency(&ProfileCounterFrequency);
    i64 WindowsTimerFrequency = ProfileCounterFrequency.QuadPart;

    WNDCLASSA MainWindowClass = {};
    MainWindowClass.style = CS_VREDRAW | CS_HREDRAW;
    MainWindowClass.lpfnWndProc = MainWindowCallback;
    MainWindowClass.hInstance = Instance;
    MainWindowClass.lpszClassName = "MainWindowClass";
    MainWindowClass.hCursor = LoadCursor(0, IDC_ARROW);

    if (RegisterClassA(&MainWindowClass))
    {
        window_private_data WindowPrivateData = {};
        WindowPrivateData.LocalRenderingBuffer = &LocalRenderingBuffer;
        WindowPrivateData.RunningState = &RunningState;

        HWND Window = CreateWindowExA
        (
            0,
            MainWindowClass.lpszClassName,
            "simulator",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            0,
            0,
            Instance,
            &WindowPrivateData
        );

        if (Window)
        {
            RunningState = true;
            while (RunningState)
            {
                LARGE_INTEGER FrameStartTime = GetWindowsTimerValue();

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
                            ProcessWindowsMessage(Message, &GlobalUserInput);
                            if (GlobalUserInput.ExitSignal)
                            {
                                *WindowPrivateData.RunningState = false;
                            }
                        } break;

                        default:
                        {
                            TranslateMessage(&Message);
                            DispatchMessage(&Message);
                        } break;
                    }
                }

                UpdateSimulation(TargetSecondsPerFrame, &GlobalUserInput, &GlobalSimulationState);
                RenderSimulation(&LocalRenderingBuffer, &GlobalSimulationState);

                HDC DeviceContext = GetDC(Window);
                DisplayRenderBufferInWindow(Window, DeviceContext, &LocalRenderingBuffer);
                ReleaseDC(Window, DeviceContext);

                f32 SecondsElapsedForFrame = GetSecondsElapsed(FrameStartTime, GetWindowsTimerValue(), WindowsTimerFrequency);
                if (SecondsElapsedForFrame < TargetSecondsPerFrame)
                {
                    DWORD TimeToSleepInMilliSeconds = (DWORD)(1000.0f * (TargetSecondsPerFrame - SecondsElapsedForFrame));
                    if (TimeToSleepInMilliSeconds > 0)
                    {
                        Sleep(TimeToSleepInMilliSeconds);
                    }

                    do
                    {
                        SecondsElapsedForFrame = GetSecondsElapsed(FrameStartTime, GetWindowsTimerValue(), WindowsTimerFrequency);
                    } while (SecondsElapsedForFrame < TargetSecondsPerFrame);
                }
                else
                {
                    // NOTE: frame missed
                    // TODO: log frame miss on screen
                }
            }
        }
    }
}
