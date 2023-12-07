#pragma once

#define VK_KEYPAD_ENTER (VK_RETURN + 256)

enum win32_mouse_area
{
    MA_NONE = 0,
    MA_CLIENT_AREA = 1,
    MA_NON_CLIENT_AREA = 2,
};

struct win32_backend_state
{
    HWND Window;
    HWND MouseWindow;
    win32_mouse_area MouseTrackedArea;
    i32 MouseButtonsDown;
    i64 Time;
    i64 TicksPerSecond;
    ImGuiMouseCursor LastMouseCursor;
    b32 MonitorsNeedUpdate;
};

struct win32_viewport_data
{
    HWND Window;
    HWND ParentWindow;
    b32 WindowOwned;
    DWORD StyleFlags;
    DWORD ExtendedStyleFlags;
};

enum win32_renderer_backend
{
    W32RB_OPENGL2,
    W32RB_DX11
};

struct win32_global_handles
{
    HMODULE ShcoreDllModule;
    HMODULE User32DllModule;
    HMODULE NtDllModule;
};