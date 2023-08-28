#include <windows.h>
#include <stdint.h>

#include "..\..\miscellaneous\base_types.h"
#include "..\..\miscellaneous\basic_defines.h"

#include "..\..\imgui\imgui.h"

#include "internal_types.h"

#include "ImGuiBackend.cpp"

window_data GlobalWindowData;
static HGLRC GlobalHandleToOpenGLRenderingContext;

b32 CreateOpenGLDevice(HWND Window, window_data *WindowData)
{
    HDC DeviceContext = GetDC(Window);

    PIXELFORMATDESCRIPTOR PixelFormatDescriptor = {};
    PixelFormatDescriptor.nSize = sizeof(PixelFormatDescriptor);
    PixelFormatDescriptor.nVersion = 1;
    PixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    PixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
    PixelFormatDescriptor.cColorBits = 32;

    i32 ChosenPixelFormat = ChoosePixelFormat(DeviceContext, &PixelFormatDescriptor);
    if (ChosenPixelFormat == 0)
    {
        return false;
    }

    b32 Result = SetPixelFormat(DeviceContext, ChosenPixelFormat, &PixelFormatDescriptor);
    if (!Result)
    {
        return false;
    }

    ReleaseDC(Window, DeviceContext);

    WindowData->DeviceContext = GetDC(Window);

    if (GlobalHandleToOpenGLRenderingContext == NULL)
    {
        GlobalHandleToOpenGLRenderingContext = wglCreateContext(WindowData->DeviceContext);
    }

    return true;
}

void CleanupOpenGLDevice(HWND Window, window_data *WindowData)
{
    wglMakeCurrent(NULL, NULL);
    ReleaseDC(Window, WindowData->DeviceContext);
}

LRESULT CALLBACK
MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
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
    WNDCLASSEX MainWindowClass = {};

    MainWindowClass.cbSize = sizeof(MainWindowClass);
    MainWindowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
    MainWindowClass.lpfnWndProc = MainWindowCallback;
    MainWindowClass.cbClsExtra = 0;
    MainWindowClass.cbWndExtra = 0;
    MainWindowClass.hInstance = Instance;
    MainWindowClass.hIcon = NULL;
    MainWindowClass.hCursor = NULL;
    MainWindowClass.hbrBackground = NULL;
    MainWindowClass.lpszMenuName = NULL;
    MainWindowClass.lpszClassName = L"MainWindowClass";
    MainWindowClass.hIconSm = NULL;

    RegisterClassExW(&MainWindowClass);
    HWND Window = CreateWindowW
    (
        MainWindowClass.lpszClassName,
        L"My IMGUI Demo program",
        WS_OVERLAPPEDWINDOW,
        100, 100, 1280, 800, 
        NULL, NULL, MainWindowClass.hInstance, NULL
    );

    b32 Result = CreateOpenGLDevice(Window, &GlobalWindowData);
    if (!Result)
    {
        CleanupOpenGLDevice(Window, &GlobalWindowData);
        DestroyWindow(Window);
        UnregisterClassW(MainWindowClass.lpszClassName, MainWindowClass.hInstance);
        return 1;
    }

    wglMakeCurrent(GlobalWindowData.DeviceContext, GlobalHandleToOpenGLRenderingContext);

    ShowWindow(Window, SW_SHOWDEFAULT);
    UpdateWindow(Window);

    ImGui::CreateContext();

    ImGuiIO IoObject = ImGui::GetIO();
    IoObject.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    IoObject.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    IoObject.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    ImGui::StyleColorsClassic();

    ImGuiStyle ImGuiStyleObject = ImGui::GetStyle();
    ImGuiStyleObject.WindowRounding = 0.0f;
    ImGuiStyleObject.Colors[ImGuiCol_WindowBg].w = 1.0f;

    InitializeImGuiWin32Backend(Window);

    return 0;
}