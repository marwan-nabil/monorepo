#pragma once

struct win32_backend_data
{
    HWND Window;
    HWND MouseWindow;
    i32 MouseTrackedArea; // 0: not tracked, 1: client are, 2: non-client area
    i32 MouseButtonsDown;
    i64 Time;
    i64 TicksPerSecond;
    ImGuiMouseCursor LastMouseCursor;
    bool WantUpdateMonitors;
};

struct win32_viewport_data
{
    HWND Window;
    HWND ParentWindow;
    bool WindowOwned;
    DWORD StyleFlags;
    DWORD ExtendedStyleFlags;
};

enum process_dpi_awareness
{
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
};

enum monitor_dpi_type
{
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
};

typedef HRESULT(WINAPI *SetProcessDpiAwarenessFunctionType)(process_dpi_awareness);
typedef HRESULT(WINAPI *GetDpiForMonitorFunctionType)(HMONITOR, monitor_dpi_type, u32 *, u32 *);
typedef DPI_AWARENESS_CONTEXT(WINAPI *SetThreadDpiAwarenessContextFunctionType)(DPI_AWARENESS_CONTEXT);
typedef u32(WINAPI *RtlVerifyVersionInfoFunctionType)(OSVERSIONINFOEXW *, u32, ULONGLONG);

#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)

// There is no distinct VK_xxx for keypad enter, instead it is VK_RETURN + KF_EXTENDED, we assign it an arbitrary value to make code more readable (VK_ codes go up to 255)
#define IM_VK_KEYPAD_ENTER (VK_RETURN + 256)

static void Win32_InitPlatformInterface();