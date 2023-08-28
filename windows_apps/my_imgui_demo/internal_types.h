#pragma once

struct window_data
{
    HDC DeviceContext;
};

struct win32_backend_data
{
    HWND Window;
    HWND MouseWindow;
    i32 MouseTrackedArea;
    i32 MouseButtonsDown;
    i64 Time;
    i64 TicksPerSecond;
    ImGuiMouseCursor LastMouseCursor;
    b32 WantUpdateMonitors;
};