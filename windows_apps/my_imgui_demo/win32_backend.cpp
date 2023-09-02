static HINSTANCE ShcoreDllModule;
static GetDpiForMonitorFunctionType GetDpiForMonitorFunction;

static i32 Win32_IsWindowsVersionOrGreater(u16 MajorVersion, u16 MinorVersion, u16 PatchVersion)
{
    RtlVerifyVersionInfoFunctionType RtlVerifyVersionInfoFn = NULL;

    HMODULE NtDllModule = GetModuleHandleA("ntdll.dll");
    if (NtDllModule)
    {
        RtlVerifyVersionInfoFn =
            (RtlVerifyVersionInfoFunctionType)
            GetProcAddress(NtDllModule, "RtlVerifyVersionInfo");
    }

    if (RtlVerifyVersionInfoFn == NULL)
    {
        return FALSE;
    }

    RTL_OSVERSIONINFOEXW VersionInfo = {};
    u64 ConditionMask = 0;

    VersionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
    VersionInfo.dwMajorVersion = MajorVersion;
    VersionInfo.dwMinorVersion = MinorVersion;

    VER_SET_CONDITION(ConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(ConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);

    u32 Result = RtlVerifyVersionInfoFn(&VersionInfo, VER_MAJORVERSION | VER_MINORVERSION, ConditionMask);
    if (Result == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static f32 Win32_GetDpiScaleForMonitor(void *Monitor)
{
    u32 DpiX = 96;
    u32 DpiY = 96;

    if (Win32_IsWindowsVersionOrGreater(HIBYTE(0x0603), LOBYTE(0x0603), 0)) // _WIN32_WINNT_WINBLUE
    {
        ShcoreDllModule = LoadLibraryA("shcore.dll"); // Reference counted per-process
        GetDpiForMonitorFunction = NULL;

        if (ShcoreDllModule)
        {
            GetDpiForMonitorFunction =
                (GetDpiForMonitorFunctionType)
                GetProcAddress(ShcoreDllModule, "GetDpiForMonitor");
        }

        if (GetDpiForMonitorFunction)
        {
            GetDpiForMonitorFunction((HMONITOR)Monitor, MDT_EFFECTIVE_DPI, &DpiX, &DpiY);
            Assert(DpiX == DpiY);
            return DpiX / 96.0f;
        }
    }

    HDC DeviceContext = GetDC(NULL);
    DpiX = GetDeviceCaps(DeviceContext, LOGPIXELSX);
    DpiY = GetDeviceCaps(DeviceContext, LOGPIXELSY);
    Assert(DpiX == DpiY);
    ReleaseDC(NULL, DeviceContext);

    return DpiX / 96.0f;
}

static void Win32_ShutdownPlatformInterface()
{
    UnregisterClass(L"ImGui Platform", GetModuleHandle(NULL));
    ImGui::DestroyPlatformWindows();
}

static win32_backend_data *Win32_GetBackendData()
{
    if (ImGui::GetCurrentContext())
    {
        return (win32_backend_data *)ImGui::GetIO().BackendPlatformUserData;
    }
    else
    {
        return NULL;
    }
}

static bool Win32_InitializeForOpenGL(void *Window)
{
    ImGuiIO *ImGuiIoInterface = &ImGui::GetIO();
    Assert(ImGuiIoInterface->BackendPlatformUserData == NULL && "Already initialized a platform backend!");

    i64 PerformanceCounterFrequency;
    i64 PerformanceCounter;
    if (!QueryPerformanceFrequency((LARGE_INTEGER*)&PerformanceCounterFrequency))
    {
        return false;
    }
    
    if (!QueryPerformanceCounter((LARGE_INTEGER*)&PerformanceCounter))
    {
        return false;
    }

    // Setup backend capabilities flags
    win32_backend_data *BackendData = (win32_backend_data *)ImGui::MemAlloc(sizeof(win32_backend_data));
    *BackendData = {};

    ImGuiIoInterface->BackendPlatformUserData = (void *)BackendData;
    ImGuiIoInterface->BackendPlatformName = "win32";
    ImGuiIoInterface->BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    ImGuiIoInterface->BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor ImGuiIoInterface->WantSetMousePos requests (optional, rarely used)
    ImGuiIoInterface->BackendFlags |= ImGuiBackendFlags_PlatformHasViewports; // We can create multi-viewports on the Platform side (optional)
    ImGuiIoInterface->BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can call ImGuiIoInterface->AddMouseViewportEvent() with correct data (optional)

    BackendData->Window = (HWND)Window;
    BackendData->WantUpdateMonitors = true;
    BackendData->TicksPerSecond = PerformanceCounterFrequency;
    BackendData->Time = PerformanceCounter;
    BackendData->LastMouseCursor = ImGuiMouseCursor_COUNT;

    // Our mouse update function expect PlatformHandle to be filled for the main viewport
    ImGuiViewport *MainViewport = ImGui::GetMainViewport();
    MainViewport->PlatformHandle = (void *)BackendData->Window;
    MainViewport->PlatformHandleRaw = (void *)BackendData->Window;

    if (ImGuiIoInterface->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        Win32_InitPlatformInterface();
    }

    return true;
}

static void Win32_Shutdown()
{
    win32_backend_data *BackendData = Win32_GetBackendData();
    Assert(BackendData != NULL && "No platform backend to shutdown, or already shutdown?");

    ImGuiIO *ImGuiIoInterface = &ImGui::GetIO();

    Win32_ShutdownPlatformInterface();

    ImGuiIoInterface->BackendPlatformName = NULL;
    ImGuiIoInterface->BackendPlatformUserData = NULL;

    ImGuiBackendFlags FlagsToClear =
        ImGuiBackendFlags_HasMouseCursors |
        ImGuiBackendFlags_HasSetMousePos |
        ImGuiBackendFlags_HasGamepad |
        ImGuiBackendFlags_PlatformHasViewports |
        ImGuiBackendFlags_HasMouseHoveredViewport;

    ImGuiIoInterface->BackendFlags &= ~FlagsToClear;

    ImGui::MemFree(BackendData);
}

static bool Win32_UpdateMouseCursor()
{
    ImGuiIO *ImGuiIoInterface = &ImGui::GetIO();
    if (ImGuiIoInterface->ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
    {
        return false;
    }

    ImGuiMouseCursor ImGuiCursor = ImGui::GetMouseCursor();
    if (ImGuiCursor == ImGuiMouseCursor_None || ImGuiIoInterface->MouseDrawCursor)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        SetCursor(NULL);
    }
    else
    {
        // Show OS mouse cursor
        LPTSTR Win32Cursor = IDC_ARROW;
        switch (ImGuiCursor)
        {
            case ImGuiMouseCursor_Arrow: Win32Cursor = IDC_ARROW; break;
            case ImGuiMouseCursor_TextInput: Win32Cursor = IDC_IBEAM; break;
            case ImGuiMouseCursor_ResizeAll: Win32Cursor = IDC_SIZEALL; break;
            case ImGuiMouseCursor_ResizeEW: Win32Cursor = IDC_SIZEWE; break;
            case ImGuiMouseCursor_ResizeNS: Win32Cursor = IDC_SIZENS; break;
            case ImGuiMouseCursor_ResizeNESW: Win32Cursor = IDC_SIZENESW; break;
            case ImGuiMouseCursor_ResizeNWSE: Win32Cursor = IDC_SIZENWSE; break;
            case ImGuiMouseCursor_Hand: Win32Cursor = IDC_HAND; break;
            case ImGuiMouseCursor_NotAllowed: Win32Cursor = IDC_NO; break;
        }
        SetCursor(LoadCursor(NULL, Win32Cursor));
    }
    return true;
}

static bool Win32_IsVkDown(i32 VirtualKey)
{
    if ((GetKeyState(VirtualKey) & 0x8000) != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void Win32_AddKeyEvent(ImGuiKey Key, bool Down, i32 NativeKeyCode, i32 NativeScanCode)
{
    ImGuiIO *ImGuiIoInterface = &ImGui::GetIO();
    ImGuiIoInterface->AddKeyEvent(Key, Down);
    ImGuiIoInterface->SetKeyEventNativeData(Key, NativeKeyCode, NativeScanCode);
}

static void Win32_ProcessKeyEventsWorkarounds()
{
    // Left & right Shift keys: when both are pressed together, Windows tend to not generate the WM_KEYUP event for the first released one.
    if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && !Win32_IsVkDown(VK_LSHIFT))
    {
        Win32_AddKeyEvent(ImGuiKey_LeftShift, false, VK_LSHIFT, -1);
    }

    if (ImGui::IsKeyDown(ImGuiKey_RightShift) && !Win32_IsVkDown(VK_RSHIFT))
    {
        Win32_AddKeyEvent(ImGuiKey_RightShift, false, VK_RSHIFT, -1);
    }

    // Sometimes WM_KEYUP for Win Key is not passed Down to the app (e.g. for Win+V on some setups, according to GLFW).
    if (ImGui::IsKeyDown(ImGuiKey_LeftSuper) && !Win32_IsVkDown(VK_LWIN))
    {
        Win32_AddKeyEvent(ImGuiKey_LeftSuper, false, VK_LWIN, -1);
    }

    if (ImGui::IsKeyDown(ImGuiKey_RightSuper) && !Win32_IsVkDown(VK_RWIN))
    {
        Win32_AddKeyEvent(ImGuiKey_RightSuper, false, VK_RWIN, -1);
    }
}

static void Win32_UpdateKeyModifiers()
{
    ImGuiIO *ImGuiIoInterface = &ImGui::GetIO();
    ImGuiIoInterface->AddKeyEvent(ImGuiMod_Ctrl, Win32_IsVkDown(VK_CONTROL));
    ImGuiIoInterface->AddKeyEvent(ImGuiMod_Shift, Win32_IsVkDown(VK_SHIFT));
    ImGuiIoInterface->AddKeyEvent(ImGuiMod_Alt, Win32_IsVkDown(VK_MENU));
    ImGuiIoInterface->AddKeyEvent(ImGuiMod_Super, Win32_IsVkDown(VK_APPS));
}

// This code supports multi-viewports (multiple OS Windows mapped into different Dear ImGui viewports)
// Because of that, it is a little more complicated than your typical single-viewport binding code!
static void Win32_UpdateMouseData()
{
    win32_backend_data *BackendData = Win32_GetBackendData();
    ImGuiIO *ImGuiIoInterface = &ImGui::GetIO();
    Assert(BackendData->Window != 0);

    POINT MouseScreenPosition;
    bool HasMouseScreenPosition = (GetCursorPos(&MouseScreenPosition) != 0);

    HWND FocusedWindow = GetForegroundWindow();
    bool IsAppFocused =
    ( 
        FocusedWindow && 
        (
            (FocusedWindow == BackendData->Window) || 
            IsChild(FocusedWindow, BackendData->Window) || 
            ImGui::FindViewportByPlatformHandle((void *)FocusedWindow)
        )
    );

    if (IsAppFocused)
    {
        // (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
        // When multi-viewports are enabled, all Dear ImGui positions are same as OS positions.
        if (ImGuiIoInterface->WantSetMousePos)
        {
            POINT MousePosition =
            {
                (i32)ImGuiIoInterface->MousePos.x,
                (i32)ImGuiIoInterface->MousePos.y
            };

            if ((ImGuiIoInterface->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) == 0)
            {
                ClientToScreen(FocusedWindow, &MousePosition);
            }
            SetCursorPos(MousePosition.x, MousePosition.y);
        }

        // (Optional) Fallback to provide mouse position when focused (WM_MOUSEMOVE already provides this when hovered or captured)
        // This also fills a short gap when clicking non-client area: WM_NCMOUSELEAVE -> modal OS move -> gap -> WM_NCMOUSEMOVE
        if
        (
            !ImGuiIoInterface->WantSetMousePos &&
            BackendData->MouseTrackedArea == 0 &&
            HasMouseScreenPosition
        )
        {
            // Single viewport mode: mouse position in client window coordinates (ImGuiIoInterface->MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
            // (This is the position you can get with GetCursorPos() + ScreenToClient() or WM_MOUSEMOVE.)
            // Multi-viewport mode: mouse position in OS absolute coordinates (ImGuiIoInterface->MousePos is (0,0) when the mouse is on the upper-left of the primary Monitor)
            // (This is the position you can get with GetCursorPos() or WM_MOUSEMOVE + ClientToScreen(). In theory adding viewport->Pos to a client position would also be the same.)
            POINT MousePosition = MouseScreenPosition;
            if (!(ImGuiIoInterface->ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
            {
                ScreenToClient(BackendData->Window, &MousePosition);
            }
            ImGuiIoInterface->AddMousePosEvent((f32)MousePosition.x, (f32)MousePosition.y);
        }
    }

    // (Optional) When using multiple viewports: call ImGuiIoInterface->AddMouseViewportEvent() with the viewport the OS mouse cursor is hovering.
    // If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the backend, Dear imGui will ignore this field and infer the information using its flawed heuristic.
    // - [X] Win32 backend correctly ignore viewports with the _NoInputs flag (here using WindowFromPoint with WM_NCHITTEST + HTTRANSPARENT in WndProc does that)
    //       Some backend are not able to handle that correctly. If a backend report an hovered viewport that has the _NoInputs flag (e.g. when dragging a window
    //       for docking, the viewport has the _NoInputs flag in order to allow us to find the viewport under), then Dear ImGui is forced to ignore the value reported
    //       by the backend, and use its flawed heuristic to guess the viewport behind.
    // - [X] Win32 backend correctly reports this regardless of another viewport behind focused and dragged from (we need this to find a useful drag and drop target).
    ImGuiID MouseViewportId = 0;

    if (HasMouseScreenPosition)
    {
        HWND HoveredWindow = WindowFromPoint(MouseScreenPosition);
        if (HoveredWindow)
        {
            ImGuiViewport *ViewPort = ImGui::FindViewportByPlatformHandle((void *)HoveredWindow);
            if (ViewPort)
            {
                MouseViewportId = ViewPort->ID;
            }
        }
    }
    ImGuiIoInterface->AddMouseViewportEvent(MouseViewportId);
}

static i32 CALLBACK Win32_UpdateMonitorsEnumFunction(HMONITOR Monitor, HDC, LPRECT, LPARAM)
{
    MONITORINFO MonitorInfo = {};
    MonitorInfo.cbSize = sizeof(MONITORINFO);

    if (!GetMonitorInfo(Monitor, &MonitorInfo))
    {
        return TRUE;
    }

    ImGuiPlatformMonitor ImGuiMonitor;
    ImGuiMonitor.MainPos = ImVec2((f32)MonitorInfo.rcMonitor.left, (f32)MonitorInfo.rcMonitor.top);
    ImGuiMonitor.MainSize = ImVec2((f32)(MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left), (f32)(MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top));
    ImGuiMonitor.WorkPos = ImVec2((f32)MonitorInfo.rcWork.left, (f32)MonitorInfo.rcWork.top);
    ImGuiMonitor.WorkSize = ImVec2((f32)(MonitorInfo.rcWork.right - MonitorInfo.rcWork.left), (f32)(MonitorInfo.rcWork.bottom - MonitorInfo.rcWork.top));
    ImGuiMonitor.DpiScale = Win32_GetDpiScaleForMonitor(Monitor);
    ImGuiMonitor.PlatformHandle = (void *)Monitor;

    ImGuiPlatformIO *ImGuiIoInterface = &ImGui::GetPlatformIO();

    if (MonitorInfo.dwFlags & MONITORINFOF_PRIMARY)
    {
        ImGuiIoInterface->Monitors.push_front(ImGuiMonitor);
    }
    else
    {
        ImGuiIoInterface->Monitors.push_back(ImGuiMonitor);
    }

    return TRUE;
}

static void Win32_UpdateMonitors()
{
    win32_backend_data *BackendData = Win32_GetBackendData();
    (&ImGui::GetPlatformIO())->Monitors.resize(0);
    EnumDisplayMonitors(NULL, NULL, Win32_UpdateMonitorsEnumFunction, 0);
    BackendData->WantUpdateMonitors = false;
}

void Win32_NewFrame()
{
    ImGuiIO *ImGuiIoInterface = &ImGui::GetIO();
    win32_backend_data *BackendData = Win32_GetBackendData();
    Assert(BackendData != NULL && "Did you call ImGui_ImplWin32_Init()?");

    // Setup display size (every frame to accommodate for window resizing)
    RECT Rectangle = {0, 0, 0, 0};
    GetClientRect(BackendData->Window, &Rectangle);
    ImGuiIoInterface->DisplaySize = 
        ImVec2((f32)(Rectangle.right - Rectangle.left), (f32)(Rectangle.bottom - Rectangle.top));

    if (BackendData->WantUpdateMonitors)
    {
        Win32_UpdateMonitors();
    }

    // Setup time step
    i64 CurrentTime = 0;
    QueryPerformanceCounter((LARGE_INTEGER *)&CurrentTime);
    ImGuiIoInterface->DeltaTime = (f32)(CurrentTime - BackendData->Time) / BackendData->TicksPerSecond;
    BackendData->Time = CurrentTime;

    // Update OS mouse position
    Win32_UpdateMouseData();

    // Process workarounds for known Windows Key handling issues
    Win32_ProcessKeyEventsWorkarounds();

    // Update OS mouse cursor with the cursor requested by imgui
    ImGuiMouseCursor MouseCursor;
    if (ImGuiIoInterface->MouseDrawCursor)
    {
        MouseCursor = ImGuiMouseCursor_None;
    }
    else
    {
        MouseCursor = ImGui::GetMouseCursor();
    }

    if (BackendData->LastMouseCursor != MouseCursor)
    {
        BackendData->LastMouseCursor = MouseCursor;
        Win32_UpdateMouseCursor();
    }
}

// Map VK_xxx to ImGuiKey_xxx.
static ImGuiKey ImGui_ImplWin32_VirtualKeyToImGuiKey(WPARAM wParam)
{
    switch (wParam)
    {
        case VK_TAB: return ImGuiKey_Tab;
        case VK_LEFT: return ImGuiKey_LeftArrow;
        case VK_RIGHT: return ImGuiKey_RightArrow;
        case VK_UP: return ImGuiKey_UpArrow;
        case VK_DOWN: return ImGuiKey_DownArrow;
        case VK_PRIOR: return ImGuiKey_PageUp;
        case VK_NEXT: return ImGuiKey_PageDown;
        case VK_HOME: return ImGuiKey_Home;
        case VK_END: return ImGuiKey_End;
        case VK_INSERT: return ImGuiKey_Insert;
        case VK_DELETE: return ImGuiKey_Delete;
        case VK_BACK: return ImGuiKey_Backspace;
        case VK_SPACE: return ImGuiKey_Space;
        case VK_RETURN: return ImGuiKey_Enter;
        case VK_ESCAPE: return ImGuiKey_Escape;
        case VK_OEM_7: return ImGuiKey_Apostrophe;
        case VK_OEM_COMMA: return ImGuiKey_Comma;
        case VK_OEM_MINUS: return ImGuiKey_Minus;
        case VK_OEM_PERIOD: return ImGuiKey_Period;
        case VK_OEM_2: return ImGuiKey_Slash;
        case VK_OEM_1: return ImGuiKey_Semicolon;
        case VK_OEM_PLUS: return ImGuiKey_Equal;
        case VK_OEM_4: return ImGuiKey_LeftBracket;
        case VK_OEM_5: return ImGuiKey_Backslash;
        case VK_OEM_6: return ImGuiKey_RightBracket;
        case VK_OEM_3: return ImGuiKey_GraveAccent;
        case VK_CAPITAL: return ImGuiKey_CapsLock;
        case VK_SCROLL: return ImGuiKey_ScrollLock;
        case VK_NUMLOCK: return ImGuiKey_NumLock;
        case VK_SNAPSHOT: return ImGuiKey_PrintScreen;
        case VK_PAUSE: return ImGuiKey_Pause;
        case VK_NUMPAD0: return ImGuiKey_Keypad0;
        case VK_NUMPAD1: return ImGuiKey_Keypad1;
        case VK_NUMPAD2: return ImGuiKey_Keypad2;
        case VK_NUMPAD3: return ImGuiKey_Keypad3;
        case VK_NUMPAD4: return ImGuiKey_Keypad4;
        case VK_NUMPAD5: return ImGuiKey_Keypad5;
        case VK_NUMPAD6: return ImGuiKey_Keypad6;
        case VK_NUMPAD7: return ImGuiKey_Keypad7;
        case VK_NUMPAD8: return ImGuiKey_Keypad8;
        case VK_NUMPAD9: return ImGuiKey_Keypad9;
        case VK_DECIMAL: return ImGuiKey_KeypadDecimal;
        case VK_DIVIDE: return ImGuiKey_KeypadDivide;
        case VK_MULTIPLY: return ImGuiKey_KeypadMultiply;
        case VK_SUBTRACT: return ImGuiKey_KeypadSubtract;
        case VK_ADD: return ImGuiKey_KeypadAdd;
        case IM_VK_KEYPAD_ENTER: return ImGuiKey_KeypadEnter;
        case VK_LSHIFT: return ImGuiKey_LeftShift;
        case VK_LCONTROL: return ImGuiKey_LeftCtrl;
        case VK_LMENU: return ImGuiKey_LeftAlt;
        case VK_LWIN: return ImGuiKey_LeftSuper;
        case VK_RSHIFT: return ImGuiKey_RightShift;
        case VK_RCONTROL: return ImGuiKey_RightCtrl;
        case VK_RMENU: return ImGuiKey_RightAlt;
        case VK_RWIN: return ImGuiKey_RightSuper;
        case VK_APPS: return ImGuiKey_Menu;
        case '0': return ImGuiKey_0;
        case '1': return ImGuiKey_1;
        case '2': return ImGuiKey_2;
        case '3': return ImGuiKey_3;
        case '4': return ImGuiKey_4;
        case '5': return ImGuiKey_5;
        case '6': return ImGuiKey_6;
        case '7': return ImGuiKey_7;
        case '8': return ImGuiKey_8;
        case '9': return ImGuiKey_9;
        case 'A': return ImGuiKey_A;
        case 'B': return ImGuiKey_B;
        case 'C': return ImGuiKey_C;
        case 'D': return ImGuiKey_D;
        case 'E': return ImGuiKey_E;
        case 'F': return ImGuiKey_F;
        case 'G': return ImGuiKey_G;
        case 'H': return ImGuiKey_H;
        case 'I': return ImGuiKey_I;
        case 'J': return ImGuiKey_J;
        case 'K': return ImGuiKey_K;
        case 'L': return ImGuiKey_L;
        case 'M': return ImGuiKey_M;
        case 'N': return ImGuiKey_N;
        case 'O': return ImGuiKey_O;
        case 'P': return ImGuiKey_P;
        case 'Q': return ImGuiKey_Q;
        case 'R': return ImGuiKey_R;
        case 'S': return ImGuiKey_S;
        case 'T': return ImGuiKey_T;
        case 'U': return ImGuiKey_U;
        case 'V': return ImGuiKey_V;
        case 'W': return ImGuiKey_W;
        case 'X': return ImGuiKey_X;
        case 'Y': return ImGuiKey_Y;
        case 'Z': return ImGuiKey_Z;
        case VK_F1: return ImGuiKey_F1;
        case VK_F2: return ImGuiKey_F2;
        case VK_F3: return ImGuiKey_F3;
        case VK_F4: return ImGuiKey_F4;
        case VK_F5: return ImGuiKey_F5;
        case VK_F6: return ImGuiKey_F6;
        case VK_F7: return ImGuiKey_F7;
        case VK_F8: return ImGuiKey_F8;
        case VK_F9: return ImGuiKey_F9;
        case VK_F10: return ImGuiKey_F10;
        case VK_F11: return ImGuiKey_F11;
        case VK_F12: return ImGuiKey_F12;
        default: return ImGuiKey_None;
    }
}

// Win32 message handler (process Win32 mouse/keyboard inputs, etc.)
// Call from your application's message handler. Keep calling your message handler unless this function returns TRUE.
// When implementing your own backend, you can read the ImGuiIoInterface.WantCaptureMouse, ImGuiIoInterface.WantCaptureKeyboard flags to tell if Dear ImGui wants to use your inputs.
// - When ImGuiIoInterface.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When ImGuiIoInterface.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to Dear ImGui, and hide them from your application based on those two flags.
// PS: In this Win32 handler, we use the capture API (GetCapture/SetCapture/ReleaseCapture) to be able to read mouse coordinates when dragging mouse outside of our window bounds.
// PS: We treat DBLCLK messages as regular mouse Down messages, so this code will work on windows classes that have the CS_DBLCLKS flag set. Our own example app code doesn't set this flag.

// See https://learn.microsoft.com/en-us/windows/win32/tablet/system-events-and-mouse-messages
// Prefer to call this at the top of the message handler to avoid the possibility of other Win32 calls interfering with this.
static ImGuiMouseSource GetMouseSourceFromMessageExtraInfo()
{
    LPARAM extra_info = GetMessageExtraInfo();
    if ((extra_info & 0xFFFFFF80) == 0xFF515700)
        return ImGuiMouseSource_Pen;
    if ((extra_info & 0xFFFFFF80) == 0xFF515780)
        return ImGuiMouseSource_TouchScreen;
    return ImGuiMouseSource_Mouse;
}

IMGUI_IMPL_API LRESULT Win32_CustomCallbackHandler(HWND Window, u32 msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui::GetCurrentContext() == NULL)
        return 0;

    ImGuiIO& ImGuiIoInterface = ImGui::GetIO();
    win32_backend_data* BackendData = Win32_GetBackendData();

    switch (msg)
    {
    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
    {
        // We need to call TrackMouseEvent in order to receive WM_MOUSELEAVE events
        ImGuiMouseSource mouse_source = GetMouseSourceFromMessageExtraInfo();
        i32 area = (msg == WM_MOUSEMOVE) ? 1 : 2;
        BackendData->MouseWindow = Window;
        if (BackendData->MouseTrackedArea != area)
        {
            TRACKMOUSEEVENT tme_cancel = { sizeof(tme_cancel), TME_CANCEL, Window, 0 };
            TRACKMOUSEEVENT tme_track = { sizeof(tme_track), (DWORD)((area == 2) ? (TME_LEAVE | TME_NONCLIENT) : TME_LEAVE), Window, 0 };
            if (BackendData->MouseTrackedArea != 0)
                TrackMouseEvent(&tme_cancel);
            TrackMouseEvent(&tme_track);
            BackendData->MouseTrackedArea = area;
        }
        POINT MousePosition = { (LONG)GET_X_LPARAM(lParam), (LONG)GET_Y_LPARAM(lParam) };
        bool want_absolute_pos = (ImGuiIoInterface.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0;
        if (msg == WM_MOUSEMOVE && want_absolute_pos)    // WM_MOUSEMOVE are client-relative coordinates.
            ClientToScreen(Window, &MousePosition);
        if (msg == WM_NCMOUSEMOVE && !want_absolute_pos) // WM_NCMOUSEMOVE are absolute coordinates.
            ScreenToClient(Window, &MousePosition);
        ImGuiIoInterface.AddMouseSourceEvent(mouse_source);
        ImGuiIoInterface.AddMousePosEvent((f32)MousePosition.x, (f32)MousePosition.y);
        break;
    }
    case WM_MOUSELEAVE:
    case WM_NCMOUSELEAVE:
    {
        i32 area = (msg == WM_MOUSELEAVE) ? 1 : 2;
        if (BackendData->MouseTrackedArea == area)
        {
            if (BackendData->MouseWindow == Window)
                BackendData->MouseWindow = NULL;
            BackendData->MouseTrackedArea = 0;
            ImGuiIoInterface.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
        }
        break;
    }
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
    {
        ImGuiMouseSource mouse_source = GetMouseSourceFromMessageExtraInfo();
        i32 button = 0;
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
        if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
        if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
        if (BackendData->MouseButtonsDown == 0 && GetCapture() == NULL)
            SetCapture(Window);
        BackendData->MouseButtonsDown |= 1 << button;
        ImGuiIoInterface.AddMouseSourceEvent(mouse_source);
        ImGuiIoInterface.AddMouseButtonEvent(button, true);
        return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    {
        ImGuiMouseSource mouse_source = GetMouseSourceFromMessageExtraInfo();
        i32 button = 0;
        if (msg == WM_LBUTTONUP) { button = 0; }
        if (msg == WM_RBUTTONUP) { button = 1; }
        if (msg == WM_MBUTTONUP) { button = 2; }
        if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
        BackendData->MouseButtonsDown &= ~(1 << button);
        if (BackendData->MouseButtonsDown == 0 && GetCapture() == Window)
            ReleaseCapture();
        ImGuiIoInterface.AddMouseSourceEvent(mouse_source);
        ImGuiIoInterface.AddMouseButtonEvent(button, false);
        return 0;
    }
    case WM_MOUSEWHEEL:
        ImGuiIoInterface.AddMouseWheelEvent(0.0f, (f32)GET_WHEEL_DELTA_WPARAM(wParam) / (f32)WHEEL_DELTA);
        return 0;
    case WM_MOUSEHWHEEL:
        ImGuiIoInterface.AddMouseWheelEvent(-(f32)GET_WHEEL_DELTA_WPARAM(wParam) / (f32)WHEEL_DELTA, 0.0f);
        return 0;
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    {
        bool is_key_down = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
        if (wParam < 256)
        {
            // Submit modifiers
            Win32_UpdateKeyModifiers();

            // Obtain virtual Key code
            // (keypad enter doesn't have its own... VK_RETURN with KF_EXTENDED flag means keypad enter, see IM_VK_KEYPAD_ENTER definition for details, it is mapped to ImGuiKey_KeyPadEnter.)
            i32 VirtualKey = (i32)wParam;
            if ((wParam == VK_RETURN) && (HIWORD(lParam) & KF_EXTENDED))
                VirtualKey = IM_VK_KEYPAD_ENTER;

            // Submit Key event
            ImGuiKey Key = ImGui_ImplWin32_VirtualKeyToImGuiKey(VirtualKey);
            i32 scancode = (i32)LOBYTE(HIWORD(lParam));
            if (Key != ImGuiKey_None)
                Win32_AddKeyEvent(Key, is_key_down, VirtualKey, scancode);

            // Submit individual left/right modifier events
            if (VirtualKey == VK_SHIFT)
            {
                // Important: Shift keys tend to get stuck when pressed together, missing Key-up events are corrected in Win32_ProcessKeyEventsWorkarounds()
                if (Win32_IsVkDown(VK_LSHIFT) == is_key_down) { Win32_AddKeyEvent(ImGuiKey_LeftShift, is_key_down, VK_LSHIFT, scancode); }
                if (Win32_IsVkDown(VK_RSHIFT) == is_key_down) { Win32_AddKeyEvent(ImGuiKey_RightShift, is_key_down, VK_RSHIFT, scancode); }
            }
            else if (VirtualKey == VK_CONTROL)
            {
                if (Win32_IsVkDown(VK_LCONTROL) == is_key_down) { Win32_AddKeyEvent(ImGuiKey_LeftCtrl, is_key_down, VK_LCONTROL, scancode); }
                if (Win32_IsVkDown(VK_RCONTROL) == is_key_down) { Win32_AddKeyEvent(ImGuiKey_RightCtrl, is_key_down, VK_RCONTROL, scancode); }
            }
            else if (VirtualKey == VK_MENU)
            {
                if (Win32_IsVkDown(VK_LMENU) == is_key_down) { Win32_AddKeyEvent(ImGuiKey_LeftAlt, is_key_down, VK_LMENU, scancode); }
                if (Win32_IsVkDown(VK_RMENU) == is_key_down) { Win32_AddKeyEvent(ImGuiKey_RightAlt, is_key_down, VK_RMENU, scancode); }
            }
        }
        return 0;
    }
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
        ImGuiIoInterface.AddFocusEvent(msg == WM_SETFOCUS);
        return 0;
    case WM_CHAR:
        if (IsWindowUnicode(Window))
        {
            // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
            if (wParam > 0 && wParam < 0x10000)
                ImGuiIoInterface.AddInputCharacterUTF16((unsigned short)wParam);
        }
        else
        {
            wchar_t wch = 0;
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char*)&wParam, 1, &wch, 1);
            ImGuiIoInterface.AddInputCharacter(wch);
        }
        return 0;
    case WM_SETCURSOR:
        // This is required to restore cursor when transitioning from e.g resize borders to client area.
        if (LOWORD(lParam) == HTCLIENT && Win32_UpdateMouseCursor())
            return 1;
        return 0;
    // case WM_DEVICECHANGE:
    //     return 0;
    case WM_DISPLAYCHANGE:
        BackendData->WantUpdateMonitors = true;
        return 0;
    }
    return 0;
}


//--------------------------------------------------------------------------------------------------------
// DPI-related helpers (optional)
//--------------------------------------------------------------------------------------------------------
// - Use to enable DPI awareness without having to create an application manifest.
// - Your own app may already do this via a manifest or explicit calls. This is mostly useful for our examples/ apps.
// - In theory we could call simple functions from Windows SDK such as SetProcessDPIAware(), SetProcessDpiAwareness(), etc.
//   but most of the functions provided by Microsoft require Windows 8.1/10+ SDK at compile time and Windows 8/10+ at runtime,
//   neither we want to require the user to have. So we dynamically select and load those functions to avoid dependencies.
//---------------------------------------------------------------------------------------------------------
// This is the scheme successfully used by GLFW (from which we borrowed some of the code) and other apps aiming to be highly portable.
// Win32_EnableDpiAwareness() is just a helper called by main.cpp, we don't call it automatically.
// If you are trying to implement your own backend for your own engine, you may ignore that noise.
//---------------------------------------------------------------------------------------------------------

// Helper function to enable DPI awareness without setting up a manifest
void Win32_EnableDpiAwareness()
{
    // Make sure monitors will be updated with latest correct scaling
    if (win32_backend_data* BackendData = Win32_GetBackendData())
        BackendData->WantUpdateMonitors = true;

    if (Win32_IsWindowsVersionOrGreater(HIBYTE(0x0A00), LOBYTE(0x0A00), 0)) // _WIN32_WINNT_WINTHRESHOLD / _WIN32_WINNT_WIN10
    {
        static HINSTANCE user32_dll = LoadLibraryA("user32.dll"); // Reference counted per-process
        if (SetThreadDpiAwarenessContextFunctionType SetThreadDpiAwarenessContextFn = (SetThreadDpiAwarenessContextFunctionType)GetProcAddress(user32_dll, "SetThreadDpiAwarenessContext"))
        {
            SetThreadDpiAwarenessContextFn(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            return;
        }
    }
    if (Win32_IsWindowsVersionOrGreater(HIBYTE(0x0603), LOBYTE(0x0603), 0)) // _WIN32_WINNT_WINBLUE
    {
        static HINSTANCE ShcoreDllModule = LoadLibraryA("shcore.dll"); // Reference counted per-process
        if (SetProcessDpiAwarenessFunctionType SetProcessDpiAwarenessFn = (SetProcessDpiAwarenessFunctionType)GetProcAddress(ShcoreDllModule, "SetProcessDpiAwareness"))
        {
            SetProcessDpiAwarenessFn(PROCESS_PER_MONITOR_DPI_AWARE);
            return;
        }
    }
#if _WIN32_WINNT >= 0x0600
    SetProcessDPIAware();
#endif
}

f32 ImGui_ImplWin32_GetDpiScaleForHwnd(void* Window)
{
    HMONITOR Monitor = MonitorFromWindow((HWND)Window, MONITOR_DEFAULTTONEAREST);
    return Win32_GetDpiScaleForMonitor(Monitor);
}

//---------------------------------------------------------------------------------------------------------
// Transparency related helpers (optional)
//--------------------------------------------------------------------------------------------------------

// [experimental]
// Borrowed from GLFW's function updateFramebufferTransparency() in src/win32_window.c
// (the Dwm* functions are Vista era functions but we are borrowing logic from GLFW)
void ImGui_ImplWin32_EnableAlphaCompositing(void* Window)
{
    if (!Win32_IsWindowsVersionOrGreater(HIBYTE(0x0600), LOBYTE(0x0600), 0)) // _WIN32_WINNT_VISTA
        return;

    i32 composition;
    if (FAILED(DwmIsCompositionEnabled(&composition)) || !composition)
        return;

    i32 opaque;
    DWORD color;
    // _WIN32_WINNT_WIN8
    if
    (
        Win32_IsWindowsVersionOrGreater(HIBYTE(0x0602), LOBYTE(0x0602), 0) ||
        (SUCCEEDED(DwmGetColorizationColor(&color, &opaque)) && !opaque)
    )
    {
        HRGN region = CreateRectRgn(0, 0, -1, -1);
        DWM_BLURBEHIND bb = {};
        bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
        bb.hRgnBlur = region;
        bb.fEnable = TRUE;
        DwmEnableBlurBehindWindow((HWND)Window, &bb);
        DeleteObject(region);
    }
    else
    {
        DWM_BLURBEHIND bb = {};
        bb.dwFlags = DWM_BB_ENABLE;
        DwmEnableBlurBehindWindow((HWND)Window, &bb);
    }
}

//---------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the backend to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------
static void ImGui_ImplWin32_GetWin32StyleFromViewportFlags(ImGuiViewportFlags flags, DWORD* out_style, DWORD* out_ex_style)
{
    if (flags & ImGuiViewportFlags_NoDecoration)
        *out_style = WS_POPUP;
    else
        *out_style = WS_OVERLAPPEDWINDOW;

    if (flags & ImGuiViewportFlags_NoTaskBarIcon)
        *out_ex_style = WS_EX_TOOLWINDOW;
    else
        *out_ex_style = WS_EX_APPWINDOW;

    if (flags & ImGuiViewportFlags_TopMost)
        *out_ex_style |= WS_EX_TOPMOST;
}

static HWND ImGui_ImplWin32_GetHwndFromViewportID(ImGuiID viewport_id)
{
    if (viewport_id != 0)
        if (ImGuiViewport* viewport = ImGui::FindViewportByID(viewport_id))
            return (HWND)viewport->PlatformHandle;
    return NULL;
}

static void ImGui_ImplWin32_CreateWindow(ImGuiViewport* viewport)
{
    win32_viewport_data *vd = (win32_viewport_data *)ImGui::MemAlloc(sizeof(win32_viewport_data));
    *vd = {};
    viewport->PlatformUserData = vd;

    // Select style and parent window
    ImGui_ImplWin32_GetWin32StyleFromViewportFlags(viewport->Flags, &vd->StyleFlags, &vd->ExtendedStyleFlags);
    vd->ParentWindow = ImGui_ImplWin32_GetHwndFromViewportID(viewport->ParentViewportId);

    // Create window
    RECT Rectangle = { (LONG)viewport->Pos.x, (LONG)viewport->Pos.y, (LONG)(viewport->Pos.x + viewport->Size.x), (LONG)(viewport->Pos.y + viewport->Size.y) };
    AdjustWindowRectEx(&Rectangle, vd->StyleFlags, FALSE, vd->ExtendedStyleFlags);
    vd->Window = CreateWindowEx(
        vd->ExtendedStyleFlags, _T("ImGui Platform"), _T("Untitled"), vd->StyleFlags,       // Style, class name, window name
        Rectangle.left, Rectangle.top, Rectangle.right - Rectangle.left, Rectangle.bottom - Rectangle.top,    // Window area
        vd->ParentWindow, NULL, GetModuleHandle(NULL), NULL);          // Owner window, Menu, Instance, Param
    vd->WindowOwned = true;
    viewport->PlatformRequestResize = false;
    viewport->PlatformHandle = viewport->PlatformHandleRaw = vd->Window;
}

static void ImGui_ImplWin32_DestroyWindow(ImGuiViewport* viewport)
{
    win32_backend_data* BackendData = Win32_GetBackendData();
    if (win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData)
    {
        if (GetCapture() == vd->Window)
        {
            // Transfer capture so if we started dragging from a window that later disappears, we'll still receive the MOUSEUP event.
            ReleaseCapture();
            SetCapture(BackendData->Window);
        }
        if (vd->Window && vd->WindowOwned)
            DestroyWindow(vd->Window);
        vd->Window = NULL;

        ImGui::MemFree(vd);
    }
    viewport->PlatformUserData = viewport->PlatformHandle = NULL;
}

static void ImGui_ImplWin32_ShowWindow(ImGuiViewport* viewport)
{
    win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData;
    Assert(vd->Window != 0);
    if (viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing)
        ShowWindow(vd->Window, SW_SHOWNA);
    else
        ShowWindow(vd->Window, SW_SHOW);
}

static void ImGui_ImplWin32_UpdateWindow(ImGuiViewport* viewport)
{
    win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData;
    Assert(vd->Window != 0);

    // Update Win32 parent if it changed _after_ creation
    // Unlike style settings derived from configuration flags, this is more likely to change for advanced apps that are manipulating ParentViewportID manually.
    HWND new_parent = ImGui_ImplWin32_GetHwndFromViewportID(viewport->ParentViewportId);
    if (new_parent != vd->ParentWindow)
    {
        // Win32 windows can either have a "Parent" (for WS_CHILD window) or an "Owner" (which among other thing keeps window above its owner).
        // Our Dear Imgui-side concept of parenting only mostly care about what Win32 call "Owner".
        // The parent parameter of CreateWindowEx() sets up Parent OR Owner depending on WS_CHILD flag. In our case an Owner as we never use WS_CHILD.
        // Calling SetParent() here would be incorrect: it will create a full child relation, alter coordinate system and clipping.
        // Calling SetWindowLongPtr() with GWLP_HWNDPARENT seems correct although poorly documented.
        // https://devblogs.microsoft.com/oldnewthing/20100315-00/?p=14613
        vd->ParentWindow = new_parent;
        SetWindowLongPtr(vd->Window, GWLP_HWNDPARENT, (LONG_PTR)vd->ParentWindow);
    }

    // (Optional) Update Win32 style if it changed _after_ creation.
    // Generally they won't change unless configuration flags are changed, but advanced uses (such as manually rewriting viewport flags) make this useful.
    DWORD new_style;
    DWORD new_ex_style;
    ImGui_ImplWin32_GetWin32StyleFromViewportFlags(viewport->Flags, &new_style, &new_ex_style);

    // Only reapply the flags that have been changed from our point of view (as other flags are being modified by Windows)
    if (vd->StyleFlags != new_style || vd->ExtendedStyleFlags != new_ex_style)
    {
        // (Optional) Update TopMost state if it changed _after_ creation
        bool top_most_changed = (vd->ExtendedStyleFlags & WS_EX_TOPMOST) != (new_ex_style & WS_EX_TOPMOST);
        HWND insert_after = top_most_changed ? ((viewport->Flags & ImGuiViewportFlags_TopMost) ? HWND_TOPMOST : HWND_NOTOPMOST) : 0;
        u32 swp_flag = top_most_changed ? 0 : SWP_NOZORDER;

        // Apply flags and position (since it is affected by flags)
        vd->StyleFlags = new_style;
        vd->ExtendedStyleFlags = new_ex_style;
        SetWindowLong(vd->Window, GWL_STYLE, vd->StyleFlags);
        SetWindowLong(vd->Window, GWL_EXSTYLE, vd->ExtendedStyleFlags);
        RECT Rectangle = { (LONG)viewport->Pos.x, (LONG)viewport->Pos.y, (LONG)(viewport->Pos.x + viewport->Size.x), (LONG)(viewport->Pos.y + viewport->Size.y) };
        AdjustWindowRectEx(&Rectangle, vd->StyleFlags, FALSE, vd->ExtendedStyleFlags); // Client to Screen
        SetWindowPos(vd->Window, insert_after, Rectangle.left, Rectangle.top, Rectangle.right - Rectangle.left, Rectangle.bottom - Rectangle.top, swp_flag | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        ShowWindow(vd->Window, SW_SHOWNA); // This is necessary when we alter the style
        viewport->PlatformRequestMove = viewport->PlatformRequestResize = true;
    }
}

static ImVec2 ImGui_ImplWin32_GetWindowPos(ImGuiViewport* viewport)
{
    win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData;
    Assert(vd->Window != 0);
    POINT pos = { 0, 0 };
    ClientToScreen(vd->Window, &pos);
    return ImVec2((f32)pos.x, (f32)pos.y);
}

static void ImGui_ImplWin32_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
    win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData;
    Assert(vd->Window != 0);
    RECT Rectangle = { (LONG)pos.x, (LONG)pos.y, (LONG)pos.x, (LONG)pos.y };
    AdjustWindowRectEx(&Rectangle, vd->StyleFlags, FALSE, vd->ExtendedStyleFlags);
    SetWindowPos(vd->Window, NULL, Rectangle.left, Rectangle.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

static ImVec2 ImGui_ImplWin32_GetWindowSize(ImGuiViewport* viewport)
{
    win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData;
    Assert(vd->Window != 0);
    RECT Rectangle;
    GetClientRect(vd->Window, &Rectangle);
    return ImVec2(f32(Rectangle.right - Rectangle.left), f32(Rectangle.bottom - Rectangle.top));
}

static void ImGui_ImplWin32_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
    win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData;
    Assert(vd->Window != 0);
    RECT Rectangle = { 0, 0, (LONG)size.x, (LONG)size.y };
    AdjustWindowRectEx(&Rectangle, vd->StyleFlags, FALSE, vd->ExtendedStyleFlags); // Client to Screen
    SetWindowPos(vd->Window, NULL, 0, 0, Rectangle.right - Rectangle.left, Rectangle.bottom - Rectangle.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

static void ImGui_ImplWin32_SetWindowFocus(ImGuiViewport* viewport)
{
    win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData;
    Assert(vd->Window != 0);
    BringWindowToTop(vd->Window);
    SetForegroundWindow(vd->Window);
    SetFocus(vd->Window);
}

static bool ImGui_ImplWin32_GetWindowFocus(ImGuiViewport* viewport)
{
    win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData;
    Assert(vd->Window != 0);
    return GetForegroundWindow() == vd->Window;
}

static bool ImGui_ImplWin32_GetWindowMinimized(ImGuiViewport* viewport)
{
    win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData;
    Assert(vd->Window != 0);
    return IsIconic(vd->Window) != 0;
}

static void ImGui_ImplWin32_SetWindowTitle(ImGuiViewport* viewport, const char *title)
{
    // SetWindowTextA() doesn't properly handle UTF-8 so we explicitely convert our string.
    win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData;
    Assert(vd->Window != 0);
    i32 n = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
    ImVector<wchar_t> title_w;
    title_w.resize(n);
    MultiByteToWideChar(CP_UTF8, 0, title, -1, title_w.Data, n);
    SetWindowTextW(vd->Window, title_w.Data);
}

static void ImGui_ImplWin32_SetWindowAlpha(ImGuiViewport* viewport, f32 alpha)
{
    win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData;
    Assert(vd->Window != 0);
    Assert(alpha >= 0.0f && alpha <= 1.0f);
    if (alpha < 1.0f)
    {
        DWORD style = GetWindowLongW(vd->Window, GWL_EXSTYLE) | WS_EX_LAYERED;
        SetWindowLongW(vd->Window, GWL_EXSTYLE, style);
        SetLayeredWindowAttributes(vd->Window, 0, (BYTE)(255 * alpha), LWA_ALPHA);
    }
    else
    {
        DWORD style = GetWindowLongW(vd->Window, GWL_EXSTYLE) & ~WS_EX_LAYERED;
        SetWindowLongW(vd->Window, GWL_EXSTYLE, style);
    }
}

static f32 ImGui_ImplWin32_GetWindowDpiScale(ImGuiViewport* viewport)
{
    win32_viewport_data* vd = (win32_viewport_data*)viewport->PlatformUserData;
    Assert(vd->Window != 0);
    return ImGui_ImplWin32_GetDpiScaleForHwnd(vd->Window);
}

// FIXME-DPI: Testing DPI related ideas
static void ImGui_ImplWin32_OnChangedViewport(ImGuiViewport* viewport)
{
    (void)viewport;
#if 0
    ImGuiStyle default_style;
    //default_style.WindowPadding = ImVec2(0, 0);
    //default_style.WindowBorderSize = 0.0f;
    //default_style.ItemSpacing.y = 3.0f;
    //default_style.FramePadding = ImVec2(0, 0);
    default_style.ScaleAllSizes(viewport->DpiScale);
    ImGuiStyle& style = ImGui::GetStyle();
    style = default_style;
#endif
}

static LRESULT CALLBACK ImGui_ImplWin32_WndProcHandler_PlatformWindow(HWND Window, u32 msg, WPARAM wParam, LPARAM lParam)
{
    if (Win32_CustomCallbackHandler(Window, msg, wParam, lParam))
        return true;

    if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle((void*)Window))
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
            // Let mouse pass-through the window. This will allow the backend to call ImGuiIoInterface.AddMouseViewportEvent() correctly. (which is optional).
            // The ImGuiViewportFlags_NoInputs flag is set while dragging a viewport, as want to detect the window behind the one we are dragging.
            // If you cannot easily access those viewport flags from your windowing/event code: you may manually synchronize its state e.g. in
            // your main loop after calling UpdatePlatformWindows(). Iterate all viewports/platform windows and pass the flag to your windowing system.
            if (viewport->Flags & ImGuiViewportFlags_NoInputs)
                return HTTRANSPARENT;
            break;
        }
    }

    return DefWindowProc(Window, msg, wParam, lParam);
}

static void Win32_InitPlatformInterface()
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = ImGui_ImplWin32_WndProcHandler_PlatformWindow;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = _T("ImGui Platform");
    wcex.hIconSm = NULL;
    RegisterClassEx(&wcex);

    Win32_UpdateMonitors();

    // Register platform interface (will be coupled with a renderer interface)
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Platform_CreateWindow = ImGui_ImplWin32_CreateWindow;
    platform_io.Platform_DestroyWindow = ImGui_ImplWin32_DestroyWindow;
    platform_io.Platform_ShowWindow = ImGui_ImplWin32_ShowWindow;
    platform_io.Platform_SetWindowPos = ImGui_ImplWin32_SetWindowPos;
    platform_io.Platform_GetWindowPos = ImGui_ImplWin32_GetWindowPos;
    platform_io.Platform_SetWindowSize = ImGui_ImplWin32_SetWindowSize;
    platform_io.Platform_GetWindowSize = ImGui_ImplWin32_GetWindowSize;
    platform_io.Platform_SetWindowFocus = ImGui_ImplWin32_SetWindowFocus;
    platform_io.Platform_GetWindowFocus = ImGui_ImplWin32_GetWindowFocus;
    platform_io.Platform_GetWindowMinimized = ImGui_ImplWin32_GetWindowMinimized;
    platform_io.Platform_SetWindowTitle = ImGui_ImplWin32_SetWindowTitle;
    platform_io.Platform_SetWindowAlpha = ImGui_ImplWin32_SetWindowAlpha;
    platform_io.Platform_UpdateWindow = ImGui_ImplWin32_UpdateWindow;
    platform_io.Platform_GetWindowDpiScale = ImGui_ImplWin32_GetWindowDpiScale; // FIXME-DPI
    platform_io.Platform_OnChangedViewport = ImGui_ImplWin32_OnChangedViewport; // FIXME-DPI

    // Register main window handle (which is owned by the main application, not by us)
    // This is mostly for simplicity and consistency, so that our code (e.g. mouse handling etc.) can use same logic for main and secondary viewports.
    ImGuiViewport* MainViewport = ImGui::GetMainViewport();
    win32_backend_data* BackendData = Win32_GetBackendData();
    win32_viewport_data *vd = (win32_viewport_data *)ImGui::MemAlloc(sizeof(win32_viewport_data));
    *vd = {};
    vd->Window = BackendData->Window;
    vd->WindowOwned = false;
    MainViewport->PlatformUserData = vd;
    MainViewport->PlatformHandle = (void*)BackendData->Window;
}