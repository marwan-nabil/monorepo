// ---------------------------------------
// ---------------------------------------
// ---------------------------------------
#if 0
LRESULT CALLBACK 
ImGuiPlatformInterfaceWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle((void*)hWnd))
    {
        switch (msg)
        {
        case WM_CLOSE:
            viewport->PlatformRequestClose = true;
            return 0;
        case WM_MOVE:
            viewport->PlatformRequestMove = true;
            break;
        case WM_SIZE:
            viewport->PlatformRequestResize = true;
            break;
        case WM_MOUSEACTIVATE:
            if (viewport->Flags & ImGuiViewportFlags_NoFocusOnClick)
                return MA_NOACTIVATE;
            break;
        case WM_NCHITTEST:
            // Let mouse pass-through the window. This will allow the backend to call io.AddMouseViewportEvent() correctly. (which is optional).
            // The ImGuiViewportFlags_NoInputs flag is set while dragging a viewport, as want to detect the window behind the one we are dragging.
            // If you cannot easily access those viewport flags from your windowing/event code: you may manually synchronize its state e.g. in
            // your main loop after calling UpdatePlatformWindows(). Iterate all viewports/platform windows and pass the flag to your windowing system.
            if (viewport->Flags & ImGuiViewportFlags_NoInputs)
                return HTTRANSPARENT;
            break;
        }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

static void InitializeImGuiPlatformInterface()
{
    WNDCLASSEX WindowClass;
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = ImGuiPlatformInterfaceWindowProcedure;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = GetModuleHandle(NULL);
    WindowClass.hIcon = NULL;
    WindowClass.hCursor = NULL;
    WindowClass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
    WindowClass.lpszMenuName = NULL;
    WindowClass.lpszClassName = L"ImGuiPlatformInterfaceWindowClass";
    WindowClass.hIconSm = NULL;
    RegisterClassEx(&WindowClass);

    ImGui_ImplWin32_UpdateMonitors();

    // Register platform interface (will be coupled with a renderer interface)
    ImGuiPlatformIO PlatformIoObject = ImGui::GetPlatformIO();
    PlatformIoObject.Platform_CreateWindow = ImGui_ImplWin32_CreateWindow;
    PlatformIoObject.Platform_DestroyWindow = ImGui_ImplWin32_DestroyWindow;
    PlatformIoObject.Platform_ShowWindow = ImGui_ImplWin32_ShowWindow;
    PlatformIoObject.Platform_SetWindowPos = ImGui_ImplWin32_SetWindowPos;
    PlatformIoObject.Platform_GetWindowPos = ImGui_ImplWin32_GetWindowPos;
    PlatformIoObject.Platform_SetWindowSize = ImGui_ImplWin32_SetWindowSize;
    PlatformIoObject.Platform_GetWindowSize = ImGui_ImplWin32_GetWindowSize;
    PlatformIoObject.Platform_SetWindowFocus = ImGui_ImplWin32_SetWindowFocus;
    PlatformIoObject.Platform_GetWindowFocus = ImGui_ImplWin32_GetWindowFocus;
    PlatformIoObject.Platform_GetWindowMinimized = ImGui_ImplWin32_GetWindowMinimized;
    PlatformIoObject.Platform_SetWindowTitle = ImGui_ImplWin32_SetWindowTitle;
    PlatformIoObject.Platform_SetWindowAlpha = ImGui_ImplWin32_SetWindowAlpha;
    PlatformIoObject.Platform_UpdateWindow = ImGui_ImplWin32_UpdateWindow;
    PlatformIoObject.Platform_GetWindowDpiScale = ImGui_ImplWin32_GetWindowDpiScale;
    PlatformIoObject.Platform_OnChangedViewport = ImGui_ImplWin32_OnChangedViewport;

    // Register main window handle (which is owned by the main application, not by us)
    // This is mostly for simplicity and consistency, so that our code (e.g. mouse handling etc.) can use same logic for main and secondary viewports.
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
    ImGui_ImplWin32_ViewportData* vd = IM_NEW(ImGui_ImplWin32_ViewportData)();
    vd->Hwnd = bd->hWnd;
    vd->HwndOwned = false;
    main_viewport->PlatformUserData = vd;
    main_viewport->PlatformHandle = (void*)bd->hWnd;
}

#endif

// ---------------------------------------
// ---------------------------------------
// ---------------------------------------

b32 InitializeImGuiWin32Backend(HWND Window)
{
    ImGuiIO IoObject = ImGui::GetIO();

    i64 PerformanceFrequency, PerformanceCounter;
    QueryPerformanceFrequency((LARGE_INTEGER *)&PerformanceFrequency);
    QueryPerformanceCounter((LARGE_INTEGER *)&PerformanceCounter);

    win32_backend_data *BackendData = (win32_backend_data *)malloc(sizeof(win32_backend_data));
    memset((void *)BackendData, 0, sizeof(win32_backend_data));

    IoObject.BackendPlatformUserData = (void *)BackendData;
    IoObject.BackendPlatformName = "Win32Backend";
    IoObject.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    IoObject.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor IoObject.WantSetMousePos requests (optional, rarely used)
    IoObject.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports; // We can create multi-viewports on the Platform side (optional)
    IoObject.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can call IoObject.AddMouseViewportEvent() with correct data (optional)

    BackendData->Window = Window;
    BackendData->WantUpdateMonitors = true;
    BackendData->TicksPerSecond = PerformanceFrequency;
    BackendData->Time = PerformanceCounter;
    BackendData->LastMouseCursor = ImGuiMouseCursor_COUNT;

    // Our mouse update function expect PlatformHandle to be filled for the main viewport
    ImGuiViewport *MainViewPort = ImGui::GetMainViewport();
    MainViewPort->PlatformHandle = MainViewPort->PlatformHandleRaw = (void *)BackendData->Window;
    if (IoObject.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        // InitializeImGuiPlatformInterface();
    }

    return true;
}