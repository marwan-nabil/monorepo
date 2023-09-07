static HMODULE GlobalShcoreDllModule;
static HMODULE GlobalUser32DllModule;
static HMODULE GlobalNtDllModule;

static b32 Win32_IsWindowsVersionOrGreater(u16 MajorVersion, u16 MinorVersion, u16 PatchVersion)
{
    RtlVerifyVersionInfoFunctionType RtlVerifyVersionInfoFunction = NULL;

    if (GlobalNtDllModule)
    {
        RtlVerifyVersionInfoFunction =
            (RtlVerifyVersionInfoFunctionType)GetProcAddress(GlobalNtDllModule, "RtlVerifyVersionInfo");
    }

    if (RtlVerifyVersionInfoFunction == NULL)
    {
        return FALSE;
    }

    RTL_OSVERSIONINFOEXW VersionInfo = {};
    VersionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
    VersionInfo.dwMajorVersion = MajorVersion;
    VersionInfo.dwMinorVersion = MinorVersion;

    u64 ConditionMask = 0;

    VER_SET_CONDITION(ConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(ConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);

    u32 Result = RtlVerifyVersionInfoFunction(&VersionInfo, VER_MAJORVERSION | VER_MINORVERSION, ConditionMask);
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
        GetDpiForMonitorFunctionType GetDpiForMonitorFunction = NULL;

        if (GlobalShcoreDllModule)
        {
            GetDpiForMonitorFunction =
                (GetDpiForMonitorFunctionType)
                GetProcAddress(GlobalShcoreDllModule, "GetDpiForMonitor");
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

static b32 Win32_ConfigureDpiAwareness()
{
    b32 DpiAwarenessSet = FALSE;

    if (Win32_IsWindowsVersionOrGreater(HIBYTE(0x0A00), LOBYTE(0x0A00), 0)) // _WIN32_WINNT_WIN10
    {
        SetThreadDpiAwarenessContextFunctionType SetThreadDpiAwarenessContextFunction =
            (SetThreadDpiAwarenessContextFunctionType)GetProcAddress(GlobalUser32DllModule, "SetThreadDpiAwarenessContext");

        if (SetThreadDpiAwarenessContextFunction)
        {
            SetThreadDpiAwarenessContextFunction(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            DpiAwarenessSet = TRUE;
        }
    }

    if (SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
    {
        DpiAwarenessSet = TRUE;
    }

    return DpiAwarenessSet;
}

static f32 Win32_GetDpiScaleForHwnd(void *Window)
{
    HMONITOR Monitor = MonitorFromWindow((HWND)Window, MONITOR_DEFAULTTONEAREST);
    return Win32_GetDpiScaleForMonitor(Monitor);
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

static b32 Win32_UpdateMouseCursor()
{
    ImGuiIO *ImGuiIoInterface = &ImGui::GetIO();
    if (ImGuiIoInterface->ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
    {
        return FALSE;
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
    return TRUE;
}

static b32 Win32_IsVkDown(i32 VirtualKey)
{
    if ((GetKeyState(VirtualKey) & 0x8000) != 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void Win32_AddKeyEvent(ImGuiKey Key, b32 Down, i32 NativeKeyCode, i32 NativeScanCode)
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
        Win32_AddKeyEvent(ImGuiKey_LeftShift, FALSE, VK_LSHIFT, -1);
    }

    if (ImGui::IsKeyDown(ImGuiKey_RightShift) && !Win32_IsVkDown(VK_RSHIFT))
    {
        Win32_AddKeyEvent(ImGuiKey_RightShift, FALSE, VK_RSHIFT, -1);
    }

    // Sometimes WM_KEYUP for Win Key is not passed Down to the app (e.g. for Win+V on some setups, according to GLFW).
    if (ImGui::IsKeyDown(ImGuiKey_LeftSuper) && !Win32_IsVkDown(VK_LWIN))
    {
        Win32_AddKeyEvent(ImGuiKey_LeftSuper, FALSE, VK_LWIN, -1);
    }

    if (ImGui::IsKeyDown(ImGuiKey_RightSuper) && !Win32_IsVkDown(VK_RWIN))
    {
        Win32_AddKeyEvent(ImGuiKey_RightSuper, FALSE, VK_RWIN, -1);
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
// Because of that, it is a little more complicated than your typical single-ViewPort binding code!
static void Win32_UpdateMouseData()
{
    win32_backend_data *BackendData = Win32_GetBackendData();
    ImGuiIO *ImGuiIoInterface = &ImGui::GetIO();
    Assert(BackendData->Window != 0);

    POINT MouseScreenPosition;
    b32 HasMouseScreenPosition = (GetCursorPos(&MouseScreenPosition) != 0);

    HWND FocusedWindow = GetForegroundWindow();
    b32 IsAppFocused =
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
        // This also fills a short gap when clicking non-client Area: WM_NCMOUSELEAVE -> modal OS move -> gap -> WM_NCMOUSEMOVE
        if
        (
            !ImGuiIoInterface->WantSetMousePos &&
            BackendData->MouseTrackedArea == 0 &&
            HasMouseScreenPosition
        )
        {
            // Single ViewPort mode: mouse position in client window coordinates (ImGuiIoInterface->MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
            // (This is the position you can get with GetCursorPos() + ScreenToClient() or WM_MOUSEMOVE.)
            // Multi-ViewPort mode: mouse position in OS absolute coordinates (ImGuiIoInterface->MousePos is (0,0) when the mouse is on the upper-left of the primary Monitor)
            // (This is the position you can get with GetCursorPos() or WM_MOUSEMOVE + ClientToScreen(). In theory adding ViewPort->Pos to a client position would also be the same.)
            POINT MousePosition = MouseScreenPosition;
            if (!(ImGuiIoInterface->ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
            {
                ScreenToClient(BackendData->Window, &MousePosition);
            }
            ImGuiIoInterface->AddMousePosEvent((f32)MousePosition.x, (f32)MousePosition.y);
        }
    }

    // (Optional) When using multiple viewports: call ImGuiIoInterface->AddMouseViewportEvent() with the ViewPort the OS mouse cursor is hovering.
    // If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the backend, Dear imGui will ignore this field and infer the information using its flawed heuristic.
    // - [X] Win32 backend correctly ignore viewports with the _NoInputs flag (here using WindowFromPoint with WM_NCHITTEST + HTTRANSPARENT in WndProc does that)
    //       Some backend are not able to handle that correctly. If a backend report an hovered ViewPort that has the _NoInputs flag (e.g. when dragging a window
    //       for docking, the ViewPort has the _NoInputs flag in order to allow us to find the ViewPort under), then Dear ImGui is forced to ignore the value reported
    //       by the backend, and use its flawed heuristic to guess the ViewPort behind.
    // - [X] Win32 backend correctly reports this regardless of another ViewPort behind focused and dragged from (we need this to find a useful drag and drop target).
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
    BackendData->MonitorsNeedUpdate = FALSE;
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

    if (BackendData->MonitorsNeedUpdate)
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
static ImGuiKey Win32_VirtualKeyToImGuiKey(WPARAM WParam)
{
    switch (WParam)
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
        case VK_KEYPAD_ENTER: return ImGuiKey_KeypadEnter;
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
// When implementing your own backend, you can read the ImGuiIoInterface.WantCaptureMouse, ImGuiIoInterface.WantCaptureKeyboard Flags to tell if Dear ImGui wants to use your inputs.
// - When ImGuiIoInterface.WantCaptureMouse is TRUE, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When ImGuiIoInterface.WantCaptureKeyboard is TRUE, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to Dear ImGui, and hide them from your application based on those two Flags.
// PS: In this Win32 handler, we use the capture API (GetCapture/SetCapture/ReleaseCapture) to be able to read mouse coordinates when dragging mouse outside of our window bounds.
// PS: We treat DBLCLK messages as regular mouse Down messages, so this code will work on windows classes that have the CS_DBLCLKS flag set. Our own example app code doesn't set this flag.

// See https://learn.microsoft.com/en-us/windows/win32/tablet/system-events-and-mouse-messages
// Prefer to call this at the top of the message handler to avoid the possibility of other Win32 calls interfering with this.
static ImGuiMouseSource Win32_GetMouseSourceFromMessageExtraInfo()
{
    LPARAM ExtraInfo = GetMessageExtraInfo();

    if ((ExtraInfo & 0xFFFFFF80) == 0xFF515700)
    {
        return ImGuiMouseSource_Pen;
    }

    if ((ExtraInfo & 0xFFFFFF80) == 0xFF515780)
    {
        return ImGuiMouseSource_TouchScreen;
    }

    return ImGuiMouseSource_Mouse;
}

LRESULT Win32_CustomCallbackHandler(HWND Window, u32 Message, WPARAM WParam, LPARAM LParam)
{
    if (ImGui::GetCurrentContext() == NULL)
    {
        return 0;
    }

    ImGuiIO *ImGuiIoInterface = &ImGui::GetIO();
    win32_backend_data *BackendData = Win32_GetBackendData();

    switch (Message)
    {
        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
        {
            // We need to call TrackMouseEvent in order to receive WM_MOUSELEAVE events
            ImGuiMouseSource MouseSource = Win32_GetMouseSourceFromMessageExtraInfo();
            i32 Area = (Message == WM_MOUSEMOVE)? 1 : 2;
            BackendData->MouseWindow = Window;
            if (BackendData->MouseTrackedArea != Area)
            {
                TRACKMOUSEEVENT TrackMouseEventCancel =
                {
                    sizeof(TrackMouseEventCancel),
                    TME_CANCEL,
                    Window,
                    0
                };

                TRACKMOUSEEVENT TrackMouseEventTrack =
                {
                    sizeof(TrackMouseEventTrack),
                    (DWORD)((Area == 2) ? (TME_LEAVE | TME_NONCLIENT) : TME_LEAVE),
                    Window,
                    0
                };

                if (BackendData->MouseTrackedArea != 0)
                {
                    TrackMouseEvent(&TrackMouseEventCancel);
                }
                TrackMouseEvent(&TrackMouseEventTrack);
                BackendData->MouseTrackedArea = Area;
            }

            POINT MousePosition = {(LONG)GET_X_LPARAM(LParam), (LONG)GET_Y_LPARAM(LParam)};
            b32 WantAbsolutePosition = (ImGuiIoInterface->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0;
            if (Message == WM_MOUSEMOVE && WantAbsolutePosition)    // WM_MOUSEMOVE are client-relative coordinates.
            {
                ClientToScreen(Window, &MousePosition);
            }

            if (Message == WM_NCMOUSEMOVE && !WantAbsolutePosition) // WM_NCMOUSEMOVE are absolute coordinates.
            {
                ScreenToClient(Window, &MousePosition);
            }

            ImGuiIoInterface->AddMouseSourceEvent(MouseSource);
            ImGuiIoInterface->AddMousePosEvent((f32)MousePosition.x, (f32)MousePosition.y);
        } break;

        case WM_MOUSELEAVE:
        case WM_NCMOUSELEAVE:
        {
            i32 Area = (Message == WM_MOUSELEAVE) ? 1 : 2;
            if (BackendData->MouseTrackedArea == Area)
            {
                if (BackendData->MouseWindow == Window)
                {
                    BackendData->MouseWindow = NULL;
                }

                BackendData->MouseTrackedArea = 0;
                ImGuiIoInterface->AddMousePosEvent(-FLT_MAX, -FLT_MAX);
            }
        } break;

        case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
        {
            ImGuiMouseSource MouseSource = Win32_GetMouseSourceFromMessageExtraInfo();

            i32 Button = 0;
            if (Message == WM_LBUTTONDOWN || Message == WM_LBUTTONDBLCLK)
            {
                Button = 0;
            }
            if (Message == WM_RBUTTONDOWN || Message == WM_RBUTTONDBLCLK)
            {
                Button = 1;
            }
            if (Message == WM_MBUTTONDOWN || Message == WM_MBUTTONDBLCLK)
            {
                Button = 2;
            }
            if (Message == WM_XBUTTONDOWN || Message == WM_XBUTTONDBLCLK)
            {
                Button = (GET_XBUTTON_WPARAM(WParam) == XBUTTON1) ? 3 : 4;
            }
            if (BackendData->MouseButtonsDown == 0 && GetCapture() == NULL)
            {
                SetCapture(Window);
            }

            BackendData->MouseButtonsDown |= 1 << Button;
            ImGuiIoInterface->AddMouseSourceEvent(MouseSource);
            ImGuiIoInterface->AddMouseButtonEvent(Button, TRUE);
            return 0;
        } break;

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
            i32 Button = 0;
            if (Message == WM_LBUTTONUP)
            {
                Button = 0;
            }
            if (Message == WM_RBUTTONUP)
            {
                Button = 1;
            }
            if (Message == WM_MBUTTONUP)
            {
                Button = 2;
            }
            if (Message == WM_XBUTTONUP)
            {
                Button = (GET_XBUTTON_WPARAM(WParam) == XBUTTON1)? 3 : 4;
            }

            BackendData->MouseButtonsDown &= ~(1 << Button);
            if (BackendData->MouseButtonsDown == 0 && GetCapture() == Window)
            {
                ReleaseCapture();
            }

            ImGuiMouseSource MouseSource = Win32_GetMouseSourceFromMessageExtraInfo();
            ImGuiIoInterface->AddMouseSourceEvent(MouseSource);
            ImGuiIoInterface->AddMouseButtonEvent(Button, FALSE);
            return 0;
        } break;

        case WM_MOUSEWHEEL:
        {
            ImGuiIoInterface->AddMouseWheelEvent(0.0f, (f32)GET_WHEEL_DELTA_WPARAM(WParam) / (f32)WHEEL_DELTA);
            return 0;
        } break;

        case WM_MOUSEHWHEEL:
        {
            ImGuiIoInterface->AddMouseWheelEvent(-(f32)GET_WHEEL_DELTA_WPARAM(WParam) / (f32)WHEEL_DELTA, 0.0f);
            return 0;
        } break;

        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        {
            b32 IsKeyDown = (Message == WM_KEYDOWN || Message == WM_SYSKEYDOWN);
            if (WParam < 256)
            {
                // Submit modifiers
                Win32_UpdateKeyModifiers();

                // Obtain virtual Key code
                // (keypad enter doesn't have its own... VK_RETURN with KF_EXTENDED flag means keypad enter, see VK_KEYPAD_ENTER definition for details, it is mapped to ImGuiKey_KeyPadEnter.)
                i32 VirtualKey = (i32)WParam;
                if ((WParam == VK_RETURN) && (HIWORD(LParam) & KF_EXTENDED))
                {
                    VirtualKey = VK_KEYPAD_ENTER;
                }

                // Submit Key event
                ImGuiKey Key = Win32_VirtualKeyToImGuiKey(VirtualKey);
                i32 ScanCode = (i32)LOBYTE(HIWORD(LParam));
                if (Key != ImGuiKey_None)
                {
                    Win32_AddKeyEvent(Key, IsKeyDown, VirtualKey, ScanCode);
                }

                // Submit individual left/right modifier events
                if (VirtualKey == VK_SHIFT)
                {
                    // Important: Shift keys tend to get stuck when pressed together, missing Key-up events are corrected in Win32_ProcessKeyEventsWorkarounds()
                    if (Win32_IsVkDown(VK_LSHIFT) == IsKeyDown)
                    {
                        Win32_AddKeyEvent(ImGuiKey_LeftShift, IsKeyDown, VK_LSHIFT, ScanCode);
                    }
                    if (Win32_IsVkDown(VK_RSHIFT) == IsKeyDown)
                    {
                        Win32_AddKeyEvent(ImGuiKey_RightShift, IsKeyDown, VK_RSHIFT, ScanCode);
                    }
                }
                else if (VirtualKey == VK_CONTROL)
                {
                    if (Win32_IsVkDown(VK_LCONTROL) == IsKeyDown)
                    {
                        Win32_AddKeyEvent(ImGuiKey_LeftCtrl, IsKeyDown, VK_LCONTROL, ScanCode);
                    }
                    if (Win32_IsVkDown(VK_RCONTROL) == IsKeyDown)
                    {
                        Win32_AddKeyEvent(ImGuiKey_RightCtrl, IsKeyDown, VK_RCONTROL, ScanCode);
                    }
                }
                else if (VirtualKey == VK_MENU)
                {
                    if (Win32_IsVkDown(VK_LMENU) == IsKeyDown)
                    {
                        Win32_AddKeyEvent(ImGuiKey_LeftAlt, IsKeyDown, VK_LMENU, ScanCode);
                    }
                    if (Win32_IsVkDown(VK_RMENU) == IsKeyDown)
                    {
                        Win32_AddKeyEvent(ImGuiKey_RightAlt, IsKeyDown, VK_RMENU, ScanCode);
                    }
                }
            }
            return 0;
        } break;

        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        {
            ImGuiIoInterface->AddFocusEvent(Message == WM_SETFOCUS);
            return 0;
        } break;

        case WM_CHAR:
        {
            if (IsWindowUnicode(Window))
            {
                // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
                if ((WParam > 0) && (WParam < 0x10000))
                {
                    ImGuiIoInterface->AddInputCharacterUTF16((u16)WParam);
                }
            }
            else
            {
                wchar_t WideChar = 0;
                MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char *)&WParam, 1, &WideChar, 1);
                ImGuiIoInterface->AddInputCharacter(WideChar);
            }
            return 0;
        } break;

        case WM_SETCURSOR:
        {
            // This is required to restore cursor when transitioning from e.g resize borders to client Area.
            if ((LOWORD(LParam) == HTCLIENT) && Win32_UpdateMouseCursor())
            {
                return 1;
            }
            else
            {
                return 0;
            }
        } break;

        case WM_DISPLAYCHANGE:
        {
            BackendData->MonitorsNeedUpdate = TRUE;
            return 0;
        } break;
    }
    return 0;
}

static b32 Win32_LoadNecessaryDlls()
{
    GlobalUser32DllModule = LoadLibraryA("user32.dll");
    GlobalShcoreDllModule = LoadLibraryA("shcore.dll");
    GlobalNtDllModule = GetModuleHandleA("ntdll.dll");

    if (!GlobalUser32DllModule || !GlobalShcoreDllModule || !GlobalNtDllModule)
    {
        return FALSE;
    }

    return TRUE;
}

//---------------------------------------------------------------------------------------------------------
// Transparency related helpers (optional)
//--------------------------------------------------------------------------------------------------------

// [experimental]
// Borrowed from GLFW's function updateFramebufferTransparency() in src/win32_window.c
// (the Dwm* functions are Vista era functions but we are borrowing logic from GLFW)
void Win32_EnableAlphaCompositing(void *Window)
{
    if (!Win32_IsWindowsVersionOrGreater(HIBYTE(0x0600), LOBYTE(0x0600), 0)) // _WIN32_WINNT_VISTA
    {
        return;
    }

    i32 Composition;
    if (FAILED(DwmIsCompositionEnabled(&Composition)) || !Composition)
    {
        return;
    }

    i32 Opaque;
    DWORD Color;
    if
    (
        // _WIN32_WINNT_WIN8
        Win32_IsWindowsVersionOrGreater(HIBYTE(0x0602), LOBYTE(0x0602), 0) ||
        (SUCCEEDED(DwmGetColorizationColor(&Color, &Opaque)) && !Opaque)
    )
    {
        HRGN Region = CreateRectRgn(0, 0, -1, -1);
        DWM_BLURBEHIND BlurredBehind = {};
        BlurredBehind.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
        BlurredBehind.hRgnBlur = Region;
        BlurredBehind.fEnable = TRUE;
        DwmEnableBlurBehindWindow((HWND)Window, &BlurredBehind);
        DeleteObject(Region);
    }
    else
    {
        DWM_BLURBEHIND BlurredBehind = {};
        BlurredBehind.dwFlags = DWM_BB_ENABLE;
        DwmEnableBlurBehindWindow((HWND)Window, &BlurredBehind);
    }
}

//---------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the backend to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------
static void Win32_GetWin32StyleFromViewportFlags(ImGuiViewportFlags Flags, DWORD *OutputStyle, DWORD *OutputStyleExtended)
{
    if (Flags & ImGuiViewportFlags_NoDecoration)
    {
        *OutputStyle = WS_POPUP;
    }
    else
    {
        *OutputStyle = WS_OVERLAPPEDWINDOW;
    }

    if (Flags & ImGuiViewportFlags_NoTaskBarIcon)
    {
        *OutputStyleExtended = WS_EX_TOOLWINDOW;
    }
    else
    {
        *OutputStyleExtended = WS_EX_APPWINDOW;
    }

    if (Flags & ImGuiViewportFlags_TopMost)
    {
        *OutputStyleExtended |= WS_EX_TOPMOST;
    }
}

static HWND Win32_GetHwndFromViewportID(ImGuiID ViewPortId)
{
    if (ViewPortId != 0)
    {
        ImGuiViewport *ViewPort = ImGui::FindViewportByID(ViewPortId);
        if (ViewPort)
        {
            return (HWND)ViewPort->PlatformHandle;
        }
    }
    return NULL;
}

static void Win32_CreateWindow(ImGuiViewport *ViewPort)
{
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ImGui::MemAlloc(sizeof(win32_viewport_data));
    *ViewPortData = {};
    ViewPort->PlatformUserData = ViewPortData;

    // Select Style and parent window
    Win32_GetWin32StyleFromViewportFlags(ViewPort->Flags, &ViewPortData->StyleFlags, &ViewPortData->ExtendedStyleFlags);
    ViewPortData->ParentWindow = Win32_GetHwndFromViewportID(ViewPort->ParentViewportId);

    // Create window
    RECT Rectangle =
    {
        (LONG)ViewPort->Pos.x,
        (LONG)ViewPort->Pos.y,
        (LONG)(ViewPort->Pos.x + ViewPort->Size.x),
        (LONG)(ViewPort->Pos.y + ViewPort->Size.y)
    };

    AdjustWindowRectEx(&Rectangle, ViewPortData->StyleFlags, FALSE, ViewPortData->ExtendedStyleFlags);

    // Owner window, Menu, Instance, Param
    ViewPortData->Window = CreateWindowEx
    (
        ViewPortData->ExtendedStyleFlags,
        _T("ImGui Platform"),
        _T("Untitled"),
        ViewPortData->StyleFlags, // Style, class name, window name
        Rectangle.left, Rectangle.top,
        Rectangle.right - Rectangle.left, Rectangle.bottom - Rectangle.top, // Window Area
        ViewPortData->ParentWindow, NULL, GetModuleHandle(NULL), NULL
    );

    ViewPortData->WindowOwned = TRUE;
    ViewPort->PlatformRequestResize = FALSE;
    ViewPort->PlatformHandle = ViewPortData->Window;
    ViewPort->PlatformHandleRaw = ViewPortData->Window;
}

static void Win32_DestroyWindow(ImGuiViewport *ViewPort)
{
    win32_backend_data *BackendData = Win32_GetBackendData();
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    if (ViewPortData)
    {
        if (GetCapture() == ViewPortData->Window)
        {
            // Transfer capture so if we started dragging from a window that later disappears, we'll still receive the MOUSEUP event.
            ReleaseCapture();
            SetCapture(BackendData->Window);
        }

        if (ViewPortData->Window && ViewPortData->WindowOwned)
        {
            DestroyWindow(ViewPortData->Window);
        }

        ViewPortData->Window = NULL;
        ImGui::MemFree(ViewPortData);
    }
    ViewPort->PlatformUserData = NULL;
    ViewPort->PlatformHandle = NULL;
}

static void Win32_ShowWindow(ImGuiViewport *ViewPort)
{
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    Assert(ViewPortData->Window != 0);

    if (ViewPort->Flags & ImGuiViewportFlags_NoFocusOnAppearing)
    {
        ShowWindow(ViewPortData->Window, SW_SHOWNA);
    }
    else
    {
        ShowWindow(ViewPortData->Window, SW_SHOW);
    }
}

static void Win32_UpdateWindow(ImGuiViewport *ViewPort)
{
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    Assert(ViewPortData->Window != 0);

    // Update Win32 parent if it changed _after_ creation
    // Unlike Style settings derived from configuration Flags, this is more likely to change for advanced apps that are manipulating ParentViewportID manually.
    HWND NewParent = Win32_GetHwndFromViewportID(ViewPort->ParentViewportId);
    if (NewParent != ViewPortData->ParentWindow)
    {
        // Win32 windows can either have a "Parent" (for WS_CHILD window) or an "Owner" (which among other thing keeps window above its owner).
        // Our Dear Imgui-side concept of parenting only mostly care about what Win32 call "Owner".
        // The parent parameter of CreateWindowEx() sets up Parent OR Owner depending on WS_CHILD flag. In our case an Owner as we never use WS_CHILD.
        // Calling SetParent() here would be incorrect: it will create a full child relation, alter coordinate system and clipping.
        // Calling SetWindowLongPtr() with GWLP_HWNDPARENT seems correct although poorly documented.
        // https://devblogs.microsoft.com/oldnewthing/20100315-00/?p=14613
        ViewPortData->ParentWindow = NewParent;
        SetWindowLongPtr(ViewPortData->Window, GWLP_HWNDPARENT, (u64)ViewPortData->ParentWindow);
    }

    // (Optional) Update Win32 Style if it changed _after_ creation.
    // Generally they won't change unless configuration Flags are changed, but advanced uses (such as manually rewriting ViewPort Flags) make this useful.
    DWORD NewStyle;
    DWORD NewExtendedStyle;
    Win32_GetWin32StyleFromViewportFlags(ViewPort->Flags, &NewStyle, &NewExtendedStyle);

    // Only reapply the Flags that have been changed from our point of view (as other Flags are being modified by Windows)
    if
    (
        (ViewPortData->StyleFlags != NewStyle) || 
        (ViewPortData->ExtendedStyleFlags != NewExtendedStyle)
    )
    {
        // (Optional) Update TopMost state if it changed _after_ creation
        b32 TopMostChanged =
            (ViewPortData->ExtendedStyleFlags & WS_EX_TOPMOST) !=
            (NewExtendedStyle & WS_EX_TOPMOST);

        HWND InsertAfter;
        if (TopMostChanged)
        {
            if (ViewPort->Flags & ImGuiViewportFlags_TopMost)
            {
                InsertAfter = HWND_TOPMOST;
            }
            else
            {
                InsertAfter = HWND_NOTOPMOST;
            }
        }
        else
        {
            InsertAfter = 0;
        }

        u32 SetWindowPositionFlags;
        if (TopMostChanged)
        {
            SetWindowPositionFlags = 0;
        }
        else
        {
            SetWindowPositionFlags = SWP_NOZORDER;
        }

        // Apply Flags and position (since it is affected by Flags)
        ViewPortData->StyleFlags = NewStyle;
        ViewPortData->ExtendedStyleFlags = NewExtendedStyle;

        SetWindowLong(ViewPortData->Window, GWL_STYLE, ViewPortData->StyleFlags);
        SetWindowLong(ViewPortData->Window, GWL_EXSTYLE, ViewPortData->ExtendedStyleFlags);

        RECT Rectangle =
        {
            (LONG)ViewPort->Pos.x,
            (LONG)ViewPort->Pos.y,
            (LONG)(ViewPort->Pos.x + ViewPort->Size.x),
            (LONG)(ViewPort->Pos.y + ViewPort->Size.y)
        };

        AdjustWindowRectEx(&Rectangle, ViewPortData->StyleFlags, FALSE, ViewPortData->ExtendedStyleFlags); // Client to Screen

        SetWindowPos
        (
            ViewPortData->Window, InsertAfter,
            Rectangle.left, Rectangle.top, Rectangle.right - Rectangle.left, Rectangle.bottom - Rectangle.top,
            SetWindowPositionFlags | SWP_NOACTIVATE | SWP_FRAMECHANGED
        );

        ShowWindow(ViewPortData->Window, SW_SHOWNA); // This is necessary when we alter the Style
        ViewPort->PlatformRequestMove = ViewPort->PlatformRequestResize = TRUE;
    }
}

static ImVec2 Win32_GetWindowPos(ImGuiViewport *ViewPort)
{
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    Assert(ViewPortData->Window != 0);

    POINT Position = {};
    ClientToScreen(ViewPortData->Window, &Position);

    ImVec2 Result = ImVec2((f32)Position.x, (f32)Position.y);
    return Result;
}

static void Win32_SetWindowPos(ImGuiViewport *ViewPort, ImVec2 Position)
{
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    Assert(ViewPortData->Window != 0);

    RECT Rectangle =
    {
        (LONG)Position.x,
        (LONG)Position.y,
        (LONG)Position.x,
        (LONG)Position.y
    };

    AdjustWindowRectEx(&Rectangle, ViewPortData->StyleFlags, FALSE, ViewPortData->ExtendedStyleFlags);
    SetWindowPos(ViewPortData->Window, NULL, Rectangle.left, Rectangle.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

static ImVec2 Win32_GetWindowSize(ImGuiViewport *ViewPort)
{
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    Assert(ViewPortData->Window != 0);

    RECT Rectangle;
    GetClientRect(ViewPortData->Window, &Rectangle);
    ImVec2 Result = ImVec2(f32(Rectangle.right - Rectangle.left), f32(Rectangle.bottom - Rectangle.top));
    return Result;
}

static void Win32_SetWindowSize(ImGuiViewport *ViewPort, ImVec2 size)
{
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    Assert(ViewPortData->Window != 0);

    RECT Rectangle = { 0, 0, (LONG)size.x, (LONG)size.y };
    AdjustWindowRectEx(&Rectangle, ViewPortData->StyleFlags, FALSE, ViewPortData->ExtendedStyleFlags); // Client to Screen
    SetWindowPos
    (
        ViewPortData->Window, NULL,
        0, 0, Rectangle.right - Rectangle.left, Rectangle.bottom - Rectangle.top,
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE
    );
}

static void Win32_SetWindowFocus(ImGuiViewport *ViewPort)
{
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    Assert(ViewPortData->Window != 0);

    BringWindowToTop(ViewPortData->Window);
    SetForegroundWindow(ViewPortData->Window);
    SetFocus(ViewPortData->Window);
}

static b32 Win32_GetWindowFocus(ImGuiViewport *ViewPort)
{
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    Assert(ViewPortData->Window != 0);

    if (GetForegroundWindow() == ViewPortData->Window)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static b32 Win32_GetWindowMinimized(ImGuiViewport *ViewPort)
{
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    Assert(ViewPortData->Window != 0);

    if (IsIconic(ViewPortData->Window) != 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void Win32_SetWindowTitle(ImGuiViewport *ViewPort, const char *title)
{
    // SetWindowTextA() doesn't properly handle UTF-8 so we explicitely convert our string.
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    Assert(ViewPortData->Window != 0);

    i32 CharCount = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);

    ImVector<wchar_t> TitleWideChar;
    TitleWideChar.resize(CharCount);

    MultiByteToWideChar(CP_UTF8, 0, title, -1, TitleWideChar.Data, CharCount);
    SetWindowTextW(ViewPortData->Window, TitleWideChar.Data);
}

static void Win32_SetWindowAlpha(ImGuiViewport *ViewPort, f32 alpha)
{
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    Assert(ViewPortData->Window != 0);
    Assert((alpha >= 0.0f) && (alpha <= 1.0f));

    if (alpha < 1.0f)
    {
        DWORD Style = GetWindowLongW(ViewPortData->Window, GWL_EXSTYLE) | WS_EX_LAYERED;
        SetWindowLongW(ViewPortData->Window, GWL_EXSTYLE, Style);
        SetLayeredWindowAttributes(ViewPortData->Window, 0, (BYTE)(255 * alpha), LWA_ALPHA);
    }
    else
    {
        DWORD Style = GetWindowLongW(ViewPortData->Window, GWL_EXSTYLE) & ~WS_EX_LAYERED;
        SetWindowLongW(ViewPortData->Window, GWL_EXSTYLE, Style);
    }
}

static f32 Win32_GetWindowDpiScale(ImGuiViewport *ViewPort)
{
    win32_viewport_data *ViewPortData = (win32_viewport_data *)ViewPort->PlatformUserData;
    Assert(ViewPortData->Window != 0);

    return Win32_GetDpiScaleForHwnd(ViewPortData->Window);
}

// FIXME-DPI: Testing DPI related ideas
static void Win32_OnChangedViewport(ImGuiViewport *ViewPort)
{
    (void)ViewPort;

    // ImGuiStyle default_style;
    //default_style.WindowPadding = ImVec2(0, 0);
    //default_style.WindowBorderSize = 0.0f;
    //default_style.ItemSpacing.y = 3.0f;
    //default_style.FramePadding = ImVec2(0, 0);
    // default_style.ScaleAllSizes(ViewPort->DpiScale);
    // ImGuiStyle& Style = ImGui::GetStyle();
    // Style = default_style;
}

static LRESULT CALLBACK Win32_WindowProcedureHandler(HWND Window, u32 Message, WPARAM WParam, LPARAM LParam)
{
    if (Win32_CustomCallbackHandler(Window, Message, WParam, LParam))
    {
        return TRUE;
    }

    ImGuiViewport *ViewPort = ImGui::FindViewportByPlatformHandle((void *)Window);
    if (ViewPort)
    {
        switch (Message)
        {
            case WM_CLOSE:
            {
                ViewPort->PlatformRequestClose = TRUE;
                return 0;
            } break;

            case WM_MOVE:
            {
                ViewPort->PlatformRequestMove = TRUE;
            } break;

            case WM_SIZE:
            {
                ViewPort->PlatformRequestResize = TRUE;
            } break;

            case WM_MOUSEACTIVATE:
            {
                if (ViewPort->Flags & ImGuiViewportFlags_NoFocusOnClick)
                {
                    return MA_NOACTIVATE;
                }
            } break;

            case WM_NCHITTEST:
            {
                // Let mouse pass-through the window. This will allow the backend to call ImGuiIoInterface.AddMouseViewportEvent() correctly. (which is optional).
                // The ImGuiViewportFlags_NoInputs flag is set while dragging a ViewPort, as want to detect the window behind the one we are dragging.
                // If you cannot easily access those ViewPort Flags from your windowing/event code: you may manually synchronize its state e.g. in
                // your main loop after calling UpdatePlatformWindows(). Iterate all viewports/platform windows and pass the flag to your windowing system.
                if (ViewPort->Flags & ImGuiViewportFlags_NoInputs)
                {
                    return HTTRANSPARENT;
                }
            } break;
        }
    }

    return DefWindowProc(Window, Message, WParam, LParam);
}

static b32 Win32_Initialize(void *Window, win32_renderer_backend RendererType)
{
    ImGuiIO *ImGuiIoInterface = &ImGui::GetIO();
    Assert(ImGuiIoInterface->BackendPlatformUserData == NULL && "Already initialized a platform backend!");

    i64 PerformanceCounterFrequency;
    i64 PerformanceCounter;
    if (!QueryPerformanceFrequency((LARGE_INTEGER*)&PerformanceCounterFrequency))
    {
        return FALSE;
    }
    
    if (!QueryPerformanceCounter((LARGE_INTEGER*)&PerformanceCounter))
    {
        return FALSE;
    }

    // Setup backend capabilities Flags
    win32_backend_data *BackendData = (win32_backend_data *)ImGui::MemAlloc(sizeof(win32_backend_data));
    *BackendData = {};

    ImGuiIoInterface->BackendPlatformUserData = (void *)BackendData;
    ImGuiIoInterface->BackendPlatformName = "win32";
    ImGuiIoInterface->BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    ImGuiIoInterface->BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor ImGuiIoInterface->WantSetMousePos requests (optional, rarely used)
    ImGuiIoInterface->BackendFlags |= ImGuiBackendFlags_PlatformHasViewports; // We can create multi-viewports on the Platform side (optional)
    ImGuiIoInterface->BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can call ImGuiIoInterface->AddMouseViewportEvent() with correct data (optional)

    BackendData->Window = (HWND)Window;
    BackendData->MonitorsNeedUpdate = TRUE;
    BackendData->TicksPerSecond = PerformanceCounterFrequency;
    BackendData->Time = PerformanceCounter;
    BackendData->LastMouseCursor = ImGuiMouseCursor_COUNT;

    // Our mouse update function expect PlatformHandle to be filled for the main ViewPort
    ImGuiViewport *MainViewport = ImGui::GetMainViewport();
    MainViewport->PlatformHandle = (void *)BackendData->Window;
    MainViewport->PlatformHandleRaw = (void *)BackendData->Window;

    if (ImGuiIoInterface->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        WNDCLASSEX WindowClassExtended;
        WindowClassExtended.cbSize = sizeof(WNDCLASSEX);
        WindowClassExtended.style = CS_HREDRAW | CS_VREDRAW;
        if (RendererType == W32RB_OPENGL2)
        {
            WindowClassExtended.style |= CS_OWNDC;
        }
        WindowClassExtended.lpfnWndProc = Win32_WindowProcedureHandler;
        WindowClassExtended.cbClsExtra = 0;
        WindowClassExtended.cbWndExtra = 0;
        WindowClassExtended.hInstance = GetModuleHandle(NULL);
        WindowClassExtended.hIcon = NULL;
        WindowClassExtended.hCursor = NULL;
        WindowClassExtended.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
        WindowClassExtended.lpszMenuName = NULL;
        WindowClassExtended.lpszClassName = L"ImGui Platform";
        WindowClassExtended.hIconSm = NULL;
        RegisterClassEx(&WindowClassExtended);

        Win32_UpdateMonitors();

        // Register platform interface (will be coupled with a renderer interface)
        ImGuiPlatformIO *ImGuiPlatformIoInterface = &ImGui::GetPlatformIO();
        ImGuiPlatformIoInterface->Platform_CreateWindow = Win32_CreateWindow;
        ImGuiPlatformIoInterface->Platform_DestroyWindow = Win32_DestroyWindow;
        ImGuiPlatformIoInterface->Platform_ShowWindow = Win32_ShowWindow;
        ImGuiPlatformIoInterface->Platform_SetWindowPos = Win32_SetWindowPos;
        ImGuiPlatformIoInterface->Platform_GetWindowPos = Win32_GetWindowPos;
        ImGuiPlatformIoInterface->Platform_SetWindowSize = Win32_SetWindowSize;
        ImGuiPlatformIoInterface->Platform_GetWindowSize = Win32_GetWindowSize;
        ImGuiPlatformIoInterface->Platform_SetWindowFocus = Win32_SetWindowFocus;
        ImGuiPlatformIoInterface->Platform_GetWindowFocus = (bool (*)(ImGuiViewport *))Win32_GetWindowFocus;
        ImGuiPlatformIoInterface->Platform_GetWindowMinimized = (bool (*)(ImGuiViewport *))Win32_GetWindowMinimized;
        ImGuiPlatformIoInterface->Platform_SetWindowTitle = Win32_SetWindowTitle;
        ImGuiPlatformIoInterface->Platform_SetWindowAlpha = Win32_SetWindowAlpha;
        ImGuiPlatformIoInterface->Platform_UpdateWindow = Win32_UpdateWindow;
        ImGuiPlatformIoInterface->Platform_GetWindowDpiScale = Win32_GetWindowDpiScale; // FIXME-DPI
        ImGuiPlatformIoInterface->Platform_OnChangedViewport = Win32_OnChangedViewport; // FIXME-DPI

        // Register main window handle (which is owned by the main application, not by us)
        // This is mostly for simplicity and consistency, so that our code (e.g. mouse handling etc.) can use same logic for main and secondary viewports.
        ImGuiViewport *MainViewport = ImGui::GetMainViewport();
        win32_backend_data *BackendData = Win32_GetBackendData();
        win32_viewport_data *ViewPortData = (win32_viewport_data *)ImGui::MemAlloc(sizeof(win32_viewport_data));
        *ViewPortData = {};
        ViewPortData->Window = BackendData->Window;
        ViewPortData->WindowOwned = FALSE;
        MainViewport->PlatformUserData = ViewPortData;
        MainViewport->PlatformHandle = (void *)BackendData->Window;
    }

    return TRUE;
}