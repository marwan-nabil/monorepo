#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <stdint.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <tchar.h>
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

bool CreateDeviceD3D(HWND hWnd)
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
    SwapChainDescriptor.OutputWindow = hWnd;
    SwapChainDescriptor.SampleDesc.Count = 1;
    SwapChainDescriptor.SampleDesc.Quality = 0;
    SwapChainDescriptor.Windowed = TRUE;
    SwapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT CreateDeviceFlags = 0;
    //CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

    D3D_FEATURE_LEVEL FeatureLevel;
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
        &FeatureLevel,
        &GlobalD3dDeviceContext
    );

    if (Result == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
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
            &FeatureLevel,
            &GlobalD3dDeviceContext
        );
    }

    if (Result != S_OK)
    {
        return false;
    }

    CreateRenderTarget();

    return true;
}

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI MainWindowProcedureHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (Win32_CustomCallbackHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        GlobalResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        GlobalResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

int main(int, char**)
{
    // Create application window
    //Win32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, MainWindowProcedureHandler, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    Win32_Initialize(hwnd, W32RB_D3D11);
    ImGui_ImplDX11_Init(GlobalD3dDevice, GlobalD3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the MainWindowProcedureHandler() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (GlobalResizeWidth != 0 && GlobalResizeHeight != 0)
        {
            CleanupRenderTarget();
            GlobalSwapChain->ResizeBuffers(0, GlobalResizeWidth, GlobalResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            GlobalResizeWidth = GlobalResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        Win32_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        GlobalD3dDeviceContext->OMSetRenderTargets(1, &GlobalMainRendererTargetView, NULL);
        GlobalD3dDeviceContext->ClearRenderTargetView(GlobalMainRendererTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        GlobalSwapChain->Present(1, 0); // Present with vsync
        //GlobalSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    Win32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT Win32_CustomCallbackHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
