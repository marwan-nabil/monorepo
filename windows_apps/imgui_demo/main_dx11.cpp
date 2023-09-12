#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <stdint.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <tchar.h>
#include <stdio.h>
#include <shellscalingapi.h>

#include "..\..\miscellaneous\base_types.h"
#include "..\..\miscellaneous\basic_defines.h"

#define ENABLE_ASSERTIONS 1
#include "..\..\miscellaneous\assertions.h"

#include "..\..\imgui\imgui.h"

#include "win32_backend.h"
#include "dx11_backend.h"

#include "win32_backend.cpp"
#include "dx11_backend.cpp"

static ID3D11Device *GlobalD3dDevice;
static ID3D11DeviceContext *GlobalD3dDeviceContext;
static IDXGISwapChain *GlobalSwapChain;
static u32 GlobalResizeWidth;
static u32 GlobalResizeHeight;
static ID3D11RenderTargetView *GlobalMainRendererTargetView;

void CleanupRenderTarget()
{
    if (GlobalMainRendererTargetView)
    {
        GlobalMainRendererTargetView->Release();
        GlobalMainRendererTargetView = NULL;
    }
}

void CreateRenderTarget()
{
    ID3D11Texture2D *BackBuffer;
    GlobalSwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer));
    GlobalD3dDevice->CreateRenderTargetView(BackBuffer, NULL, &GlobalMainRendererTargetView);
    BackBuffer->Release();
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (GlobalSwapChain)
    {
        GlobalSwapChain->Release();
        GlobalSwapChain = NULL;
    }
    if (GlobalD3dDeviceContext)
    {
        GlobalD3dDeviceContext->Release();
        GlobalD3dDeviceContext = NULL;
    }
    if (GlobalD3dDevice)
    {
        GlobalD3dDevice->Release();
        GlobalD3dDevice = NULL;
    }
}

b32 CreateDeviceD3D(HWND Window)
{
    DXGI_SWAP_CHAIN_DESC SwapChainDescriptor;
    ZeroMemory(&SwapChainDescriptor, sizeof(SwapChainDescriptor));
    SwapChainDescriptor.BufferCount = 2;
    SwapChainDescriptor.BufferDesc.Width = 0;
    SwapChainDescriptor.BufferDesc.Height = 0;
    SwapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
    SwapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
    SwapChainDescriptor.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    SwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDescriptor.OutputWindow = Window;
    SwapChainDescriptor.SampleDesc.Count = 1;
    SwapChainDescriptor.SampleDesc.Quality = 0;
    SwapChainDescriptor.Windowed = TRUE;
    SwapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT CreateDeviceFlags = 0;
    //CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

    D3D_FEATURE_LEVEL ReturnedFeatureLevel;
    D3D_FEATURE_LEVEL FeatureLevelArray[2] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0
    };

    HRESULT Result = D3D11CreateDeviceAndSwapChain
    (
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        CreateDeviceFlags,
        FeatureLevelArray,
        2,
        D3D11_SDK_VERSION,
        &SwapChainDescriptor,
        &GlobalSwapChain,
        &GlobalD3dDevice,
        &ReturnedFeatureLevel,
        &GlobalD3dDeviceContext
    );

    // Try high-performance WARP software driver if hardware is not available.
    if (Result == DXGI_ERROR_UNSUPPORTED)
    {
        Result = D3D11CreateDeviceAndSwapChain
        (
            NULL,
            D3D_DRIVER_TYPE_WARP,
            NULL,
            CreateDeviceFlags,
            FeatureLevelArray,
            2,
            D3D11_SDK_VERSION,
            &SwapChainDescriptor,
            &GlobalSwapChain,
            &GlobalD3dDevice,
            &ReturnedFeatureLevel,
            &GlobalD3dDeviceContext
        );
    }

    if (Result != S_OK)
    {
        return FALSE;
    }

    CreateRenderTarget();

    return TRUE;
}

LRESULT WINAPI MainWindowCallbackHandler(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    if (Win32_CustomCallbackHandler(Window, Message, WParam, LParam))
    {
        return TRUE;
    }

    switch (Message)
    {
        case WM_SIZE:
        {
            if (WParam == SIZE_MINIMIZED)
            {
                return 0;
            }
            GlobalResizeWidth = (UINT)LOWORD(LParam); // Queue resize
            GlobalResizeHeight = (UINT)HIWORD(LParam);
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

i32 main(i32 argc, char **argv)
{
    b32 Result = Win32_LoadNecessaryDlls();
    if (!Result)
    {
        printf("ERROR: failed to load necessary DLLs.\n");
        return 1;
    }

    Result = Win32_ConfigureDpiAwareness();
    if (!Result)
    {
        printf("ERROR: Dpi Awareness could not be configured.\n");
        return 1;
    }

    WNDCLASSEXW WindowClass;
    WindowClass.cbSize = sizeof(WindowClass);
    WindowClass.style = CS_CLASSDC;
    WindowClass.lpfnWndProc = MainWindowCallbackHandler;
    WindowClass.cbClsExtra = 0L;
    WindowClass.cbWndExtra = 0L;
    WindowClass.hInstance = GetModuleHandle(NULL);
    WindowClass.hIcon = NULL;
    WindowClass.hCursor = NULL;
    WindowClass.hbrBackground = NULL;
    WindowClass.lpszMenuName = NULL;
    WindowClass.lpszClassName = L"WindowClass";
    WindowClass.hIconSm = NULL;
    RegisterClassExW(&WindowClass);

    HWND Window = CreateWindowW
    (
        WindowClass.lpszClassName,
        L"win32 - dx11 IMGUI example",
        WS_OVERLAPPEDWINDOW,
        100, 100, 1280, 800,
        NULL, NULL, WindowClass.hInstance, NULL
    );

    if (!CreateDeviceD3D(Window))
    {
        CleanupDeviceD3D();
        UnregisterClassW(WindowClass.lpszClassName, WindowClass.hInstance);
        return 1;
    }

    ShowWindow(Window, SW_SHOWDEFAULT);
    UpdateWindow(Window);

    ImGui::CreateContext();

    ImGuiIO *ImGuiIoInterface = &ImGui::GetIO();
    ImGuiIoInterface->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGuiIoInterface->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGuiIoInterface->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    ImGuiStyle *ImGuiStyleInterface = &ImGui::GetStyle();
    ImGuiStyleInterface->WindowRounding = 0.0f;
    ImGuiStyleInterface->Colors[ImGuiCol_WindowBg].w = 1.0f;

    // Setup Platform/Renderer backends
    Win32_Initialize(Window, W32RB_DX11);
    Dx11_Initialize(GlobalD3dDevice, GlobalD3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //ImGuiIoInterface->Fonts->AddFontDefault();
    //ImGuiIoInterface->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //ImGuiIoInterface->Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //ImGuiIoInterface->Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //ImGuiIoInterface->Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = ImGuiIoInterface->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, ImGuiIoInterface->Fonts->GetGlyphRangesJapanese());
    //Assert(font != NULL);

    b32 ShowDemoWindow = TRUE;
    b32 ShowAnotherWindow = FALSE;
    ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    b32 Done = FALSE;
    while (!Done)
    {
        MSG Message;
        while (PeekMessage(&Message, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
            if (Message.message == WM_QUIT)
            {
                Done = TRUE;
            }
        }

        if (Done)
        {
            break;
        }

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if ((GlobalResizeWidth != 0) && (GlobalResizeHeight != 0))
        {
            CleanupRenderTarget();
            GlobalSwapChain->ResizeBuffers(0, GlobalResizeWidth, GlobalResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            GlobalResizeWidth = 0;
            GlobalResizeHeight = 0;
            CreateRenderTarget();
        }

        Dx11_NewFrame();
        Win32_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (ShowDemoWindow)
        {
            ImGui::ShowDemoWindow((bool *)&ShowDemoWindow);
        }

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        static f32 FloatValue = 0.0f;
        static i32 CounterValue = 0;

        u32  CurrentDpi = GetDpiForWindow(Window);

        ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");
        ImGui::Text("Current DPI is: %d", CurrentDpi);
        ImGui::Checkbox("Demo Window", (bool *)&ShowDemoWindow); // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", (bool *)&ShowAnotherWindow);

        ImGui::SliderFloat("f32", &FloatValue, 0.0f, 1.0f); // Edit 1 f32 using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (f32*)&ClearColor); // Edit 3 floats representing a color

        if (ImGui::Button("Button")) // Buttons return TRUE when clicked (most widgets return TRUE when edited/activated)
        {
            CounterValue++;
        }
        ImGui::SameLine();
        ImGui::Text("CounterValue = %d", CounterValue);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGuiIoInterface->Framerate, ImGuiIoInterface->Framerate);
        ImGui::End();

        // 3. Show another simple window.
        if (ShowAnotherWindow)
        {
            // Pass a pointer to our b32 variable (the window will have a closing button that will clear the b32 when clicked)
            ImGui::Begin("Another Window", (bool *)&ShowAnotherWindow);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
            {
                ShowAnotherWindow = FALSE;
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        f32 ClearColorWithAlpha[4] =
        {
            ClearColor.x * ClearColor.w,
            ClearColor.y * ClearColor.w,
            ClearColor.z * ClearColor.w,
            ClearColor.w
        };
        GlobalD3dDeviceContext->OMSetRenderTargets(1, &GlobalMainRendererTargetView, NULL);
        GlobalD3dDeviceContext->ClearRenderTargetView(GlobalMainRendererTargetView, ClearColorWithAlpha);
        Dx11_RenderDrawData(ImGui::GetDrawData());

        GlobalSwapChain->Present(1, 0); // Present with vsync
        //GlobalSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    Dx11_Shutdown();
    Win32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(Window);
    UnregisterClassW(WindowClass.lpszClassName, WindowClass.hInstance);

    return 0;
}