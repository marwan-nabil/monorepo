#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <stdint.h>
#include <GL/GL.h>
#include <tchar.h>

#include "..\..\miscellaneous\base_types.h"
#include "..\..\miscellaneous\basic_defines.h"
#include "..\..\miscellaneous\assertions.h"

#include "..\..\imgui\imgui.h"

#include "opengl2_backend.h"
#include "win32_backend.h"
#include "internal_types.h"

#include "opengl2_backend.cpp"
#include "win32_backend.cpp"

HGLRC GlobalOpenGLRenderingContext;
window_data GlobalMainWindowData;
i32 GlobalWidth;
i32 GlobalHeight;

static b32 CreateDeviceWGL(HWND Window, window_data *WindowData)
{
    HDC DeviceContext = GetDC(Window);
    PIXELFORMATDESCRIPTOR PixelFormatDescriptor = {};
    PixelFormatDescriptor.nSize = sizeof(PixelFormatDescriptor);
    PixelFormatDescriptor.nVersion = 1;
    PixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    PixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
    PixelFormatDescriptor.cColorBits = 32;

    i32 PixelFormat = ChoosePixelFormat(DeviceContext, &PixelFormatDescriptor);
    if (PixelFormat == 0)
    {
        return false;
    }
    if (SetPixelFormat(DeviceContext, PixelFormat, &PixelFormatDescriptor) == FALSE)
    {
        return false;
    }
    ReleaseDC(Window, DeviceContext);

    WindowData->DeviceContext = GetDC(Window);
    if (!GlobalOpenGLRenderingContext)
    {
        GlobalOpenGLRenderingContext = wglCreateContext(WindowData->DeviceContext);
    }

    return true;
}

static void CleanupDeviceWGL(HWND Window, window_data *WindowData)
{
    wglMakeCurrent(NULL, NULL);
    ReleaseDC(Window, WindowData->DeviceContext);
}

static LRESULT WINAPI MainWindowCallbackHandler(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    // Win32 message handler
    // You can read the ImGuiIOInterface.WantCaptureMouse, ImGuiIOInterface.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When ImGuiIOInterface.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When ImGuiIOInterface.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

    if (Win32_CustomCallbackHandler(Window, Message, WParam, LParam))
    {
        return true;
    }

    switch (Message)
    {
        case WM_SIZE:
        {
            if (WParam != SIZE_MINIMIZED)
            {
                GlobalWidth = LOWORD(LParam);
                GlobalHeight = HIWORD(LParam);
            }
            return 0;
        } break;

        case WM_SYSCOMMAND:
        {
            if ((WParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            {
                return 0;
            }
        } break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        } break;
    }

    return DefWindowProcW(Window, Message, WParam, LParam);
}

// Support function for multi-viewports
// Unlike most other backend combination, we need specific hooks to combine Win32+OpenGL.
// We could in theory decide to support Win32-specific code in OpenGL backend via e.g. an hypothetical ImGui_ImplOpenGL2_InitForRawWin32().
static void CreateWindowHook(ImGuiViewport *ViewPort)
{
    assert(ViewPort->RendererUserData == NULL);

    window_data *WindowData = (window_data *)ImGui::MemAlloc(sizeof(window_data));
    *WindowData = {};
    CreateDeviceWGL((HWND)ViewPort->PlatformHandle, WindowData);
    ViewPort->RendererUserData = WindowData;
}

static void DestroyWindowHook(ImGuiViewport *ViewPort)
{
    if (ViewPort->RendererUserData != NULL)
    {
        window_data *WindowData = (window_data *)ViewPort->RendererUserData;
        CleanupDeviceWGL((HWND)ViewPort->PlatformHandle, WindowData);
        if (WindowData)
        {
            ImGui::MemFree(WindowData);
        }
        ViewPort->RendererUserData = NULL;
    }
}

static void RenderWindowHook(ImGuiViewport *ViewPort, void *RenderArgument)
{
    // Activate the platform window DC in the OpenGL rendering context
    if (window_data *WindowData = (window_data *)ViewPort->RendererUserData)
    {
        wglMakeCurrent(WindowData->DeviceContext, GlobalOpenGLRenderingContext);
    }
}

static void SwapBuffersHook(ImGuiViewport *ViewPort, void *RenderArgument)
{
    if (window_data *WindowData = (window_data *)ViewPort->RendererUserData)
    {
        SwapBuffers(WindowData->DeviceContext);
    }
}

// Main code
i32 main(i32 argc, char **argv)
{
    // Win32_EnableDpiAwareness();

    WNDCLASSEXW MainWindowClass = {}; 
    MainWindowClass.cbSize = sizeof(MainWindowClass);
    MainWindowClass.style = CS_OWNDC;
    MainWindowClass.lpfnWndProc = MainWindowCallbackHandler;
    MainWindowClass.cbClsExtra = 0;
    MainWindowClass.cbWndExtra = 0;
    MainWindowClass.hInstance = GetModuleHandle(NULL);
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
        L"my imgui demo",
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        1280,
        800,
        NULL,
        NULL,
        MainWindowClass.hInstance,
        NULL
    );

    // Initialize OpenGL
    if (!CreateDeviceWGL(Window, &GlobalMainWindowData))
    {
        CleanupDeviceWGL(Window, &GlobalMainWindowData);
        DestroyWindow(Window);
        UnregisterClassW(MainWindowClass.lpszClassName, MainWindowClass.hInstance);
        return 1;
    }
    wglMakeCurrent(GlobalMainWindowData.DeviceContext, GlobalOpenGLRenderingContext);

    // Show the window
    ShowWindow(Window, SW_SHOWDEFAULT);
    UpdateWindow(Window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO *ImGuiIOInterface = &ImGui::GetIO();
    ImGuiIOInterface->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    ImGuiIOInterface->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    ImGuiIOInterface->ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    ImGuiIOInterface->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle *ImGuiStyleInterface = &ImGui::GetStyle();
    if (ImGuiIOInterface->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGuiStyleInterface->WindowRounding = 0.0f;
        ImGuiStyleInterface->Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    Win32_InitializeForOpenGL(Window);
    OpenGL2_Initialize();

    // Win32+GL needs specific hooks for ViewPort, as there are specific things needed to tie Win32 and GL api.
    if (ImGuiIOInterface->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGuiPlatformIO *ImGuiPlatformIOInterface = &ImGui::GetPlatformIO();
        IM_ASSERT(ImGuiPlatformIOInterface->Renderer_CreateWindow == NULL);
        IM_ASSERT(ImGuiPlatformIOInterface->Renderer_DestroyWindow == NULL);
        IM_ASSERT(ImGuiPlatformIOInterface->Renderer_SwapBuffers == NULL);
        IM_ASSERT(ImGuiPlatformIOInterface->Platform_RenderWindow == NULL);
        ImGuiPlatformIOInterface->Renderer_CreateWindow = CreateWindowHook;
        ImGuiPlatformIOInterface->Renderer_DestroyWindow = DestroyWindowHook;
        ImGuiPlatformIOInterface->Renderer_SwapBuffers = SwapBuffersHook;
        ImGuiPlatformIOInterface->Platform_RenderWindow = RenderWindowHook;
    }

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //ImGuiIOInterface->Fonts->AddFontDefault();
    //ImGuiIOInterface->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //ImGuiIOInterface->Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //ImGuiIOInterface->Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //ImGuiIOInterface->Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = ImGuiIOInterface->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, ImGuiIOInterface->Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool ShowDemoWindow = true;
    bool ShowAnotherWindow = false;
    ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    b32 Done = false;
    while (!Done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the MainWindowCallbackHandler() function below for our to dispatch events to the Win32 backend.
        MSG Message;
        while (PeekMessage(&Message, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
            if (Message.message == WM_QUIT)
            {
                Done = true;
            }
        }

        if (Done)
        {
            break;
        }

        // Start the Dear ImGui frame
        OpenGL2_NewFrame();
        Win32_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (ShowDemoWindow)
        {
            ImGui::ShowDemoWindow(&ShowDemoWindow);
        }

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static f32 FloatSliderValue = 0.0f;
            static i32 CounterValue = 0;

            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &ShowDemoWindow); // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &ShowAnotherWindow);

            ImGui::SliderFloat("f32", &FloatSliderValue, 0.0f, 1.0f); // Edit 1 f32 using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (f32 *)&ClearColor); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            {
                CounterValue++;
            }
            ImGui::SameLine();
            ImGui::Text("CounterValue = %d", CounterValue);
            ImGui::Text
            (
                "Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGuiIOInterface->Framerate,
                ImGuiIOInterface->Framerate
            );
            ImGui::End();
        }

        // 3. Show another simple window.
        if (ShowAnotherWindow)
        {
            ImGui::Begin("Another Window", &ShowAnotherWindow);   // Pass a pointer to our b32 variable (the window will have a closing button that will clear the b32 when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
            {
                ShowAnotherWindow = false;
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, GlobalWidth, GlobalHeight);
        glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        OpenGL2_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (ImGuiIOInterface->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();

            // Restore the OpenGL rendering context to the main window DC, since platform windows might have changed it.
            wglMakeCurrent(GlobalMainWindowData.DeviceContext, GlobalOpenGLRenderingContext);
        }

        // Present
        SwapBuffers(GlobalMainWindowData.DeviceContext);
    }

    OpenGL2_Shutdown();
    Win32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceWGL(Window, &GlobalMainWindowData);
    wglDeleteContext(GlobalOpenGLRenderingContext);
    DestroyWindow(Window);
    UnregisterClassW(MainWindowClass.lpszClassName, MainWindowClass.hInstance);

    return 0;
}