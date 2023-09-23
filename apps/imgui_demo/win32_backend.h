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

typedef HRESULT (WINAPI *set_process_dpi_awareness)(PROCESS_DPI_AWARENESS);
typedef HRESULT (WINAPI *get_dpi_for_monitror)(HMONITOR, MONITOR_DPI_TYPE, u32 *, u32 *);
typedef DPI_AWARENESS_CONTEXT (WINAPI *set_thread_dpi_awareness)(DPI_AWARENESS_CONTEXT);
typedef u32 (WINAPI *rtl_verify_version_info)(OSVERSIONINFOEXW *, u32, ULONGLONG);