#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <timeapi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <dxgi.h>

#include "../../miscellaneous/base_types.h"
#include "../../miscellaneous/basic_defines.h"
#include "../../miscellaneous/assertions.h"

#include "directx_demo.h"

#include "../../math/floats.cpp"

window_data GlobalWindowData;
d3d_state GlobalD3dState;

vertex GlobalCubeVertices[8];
u16 GlobalCubeVertexIndices[36];

void Update(f32 TimeDelta)
{

}

void Render()
{

}

void
InitializeGlobalState()
{
    GlobalWindowData.Width = 1280;
    GlobalWindowData.Height = 720;
    GlobalWindowData.Name = "DirectXDemoWindow";
    GlobalWindowData.ClassName = "directx_demo_window_class";
    GlobalWindowData.EnableVSync = TRUE;

    vertex CubeVertices[8] =
    {
        {DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)},
        {DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)},
        {DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f)},
        {DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f)},
        {DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f)},
        {DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f)},
        {DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)},
        {DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f)}
    };
    memcpy(GlobalCubeVertices, CubeVertices, sizeof(CubeVertices));

    u16 CubeVertexIndices[36] =
    {
        0, 1, 2,
        0, 2, 3,
        4, 6, 5,
        4, 7, 6,
        4, 5, 1,
        4, 1, 0,
        3, 2, 6,
        3, 6, 7,
        1, 5, 6,
        1, 6, 2,
        4, 0, 3,
        4, 3, 7
    };
    memcpy(GlobalCubeVertexIndices, CubeVertexIndices, sizeof(CubeVertexIndices));
}

inline void SafeReleaseComObject(IUnknown **Object)
{
    if (*Object != NULL)
    {
        (*Object)->Release();
        *Object = NULL;
    }
}

DXGI_RATIONAL QueryRefreshRate(u32 ScreenWidth, u32 ScreenHeight, b32 EnableVSync)
{
    DXGI_RATIONAL FoundRefreshRate = {0, 1};

    if (EnableVSync)
    {
        IDXGIFactory *DxgiFactory;
        IDXGIAdapter *DxgiAdapter;
        IDXGIOutput *DxgiAdapterOutput;
        DXGI_MODE_DESC *DxgiDisplayModes;

        HRESULT Result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&DxgiFactory);
        if (FAILED(Result))
        {
            printf("ERROR: cannot create IDXGIFactory.\n");
            return FoundRefreshRate;
        }

        Result = DxgiFactory->EnumAdapters(0, &DxgiAdapter);
        if (FAILED(Result))
        {
            printf("ERROR: cannot enumerate IDXGIAdapters.\n");
            return FoundRefreshRate;
        }

        Result = DxgiAdapter->EnumOutputs(0, &DxgiAdapterOutput);
        if (FAILED(Result))
        {
            printf("ERROR: cannot enumerate IDXGIAdapters.\n");
            return FoundRefreshRate;
        }

        u32 NumberOfDisplayModes;
        Result = DxgiAdapterOutput->GetDisplayModeList
        (
            DXGI_FORMAT_B8G8R8A8_UNORM,
            DXGI_ENUM_MODES_INTERLACED,
            &NumberOfDisplayModes,
            NULL
        );

        if (FAILED(Result))
        {
            printf("ERROR: cannot get the number of display modes for an adapter output.\n");
            return FoundRefreshRate;
        }

        DxgiDisplayModes = (DXGI_MODE_DESC *)malloc(sizeof(DXGI_MODE_DESC) * NumberOfDisplayModes);
        Assert(DxgiDisplayModes);

        Result = DxgiAdapterOutput->GetDisplayModeList
        (
            DXGI_FORMAT_B8G8R8A8_UNORM,
            DXGI_ENUM_MODES_INTERLACED,
            &NumberOfDisplayModes,
            DxgiDisplayModes
        );

        if (FAILED(Result))
        {
            printf("ERROR: cannot get the display modes for an adapter output.\n");
            return FoundRefreshRate;
        }

        for (u32 Index = 0; Index < NumberOfDisplayModes; Index++)
        {
            if ((DxgiDisplayModes[Index].Width == ScreenWidth) && (DxgiDisplayModes[Index].Height == ScreenHeight))
            {
                FoundRefreshRate = DxgiDisplayModes[Index].RefreshRate;
            }
        }

        free(DxgiDisplayModes);
        SafeReleaseComObject((IUnknown **)&DxgiAdapterOutput);
        SafeReleaseComObject((IUnknown **)&DxgiAdapter);
        SafeReleaseComObject((IUnknown **)&DxgiFactory);
    }

    return FoundRefreshRate;
}

i32
InitializeD3dState(HINSTANCE Instance, b32 EnableVSync)
{
    Assert(GlobalWindowData.Handle);

    RECT ClientRectangle;
    GetClientRect(GlobalWindowData.Handle, &ClientRectangle);

    u32 ClientAreaWidth = ClientRectangle.right - ClientRectangle.left;
    u32 ClientAreaHeight = ClientRectangle.bottom - ClientRectangle.top;

    DXGI_SWAP_CHAIN_DESC SwapChainDescriptor;
    ZeroMemory(&SwapChainDescriptor, sizeof(DXGI_SWAP_CHAIN_DESC));
    SwapChainDescriptor.BufferCount = 1;
    SwapChainDescriptor.BufferDesc.Width = ClientAreaWidth;
    SwapChainDescriptor.BufferDesc.Height = ClientAreaHeight;
    SwapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDescriptor.BufferDesc.RefreshRate =
        QueryRefreshRate(ClientAreaWidth, ClientAreaHeight, EnableVSync);
    SwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDescriptor.OutputWindow = GlobalWindowData.Handle;
    SwapChainDescriptor.SampleDesc.Count = 1;
    SwapChainDescriptor.SampleDesc.Quality = 0;
    SwapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    SwapChainDescriptor.Windowed = TRUE;

    u32 DeviceCreationFlags = 0;
#if _DEBUG
    DeviceCreationFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL FeatureLevelOptions[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    D3D_FEATURE_LEVEL FeatureLevelPicked;

    HRESULT Result = D3D11CreateDeviceAndSwapChain
    (
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, DeviceCreationFlags,
        FeatureLevelOptions, ArrayCount(FeatureLevelOptions), D3D11_SDK_VERSION,
        &SwapChainDescriptor, &GlobalD3dState.SwapChain,
        &GlobalD3dState.Device, &FeatureLevelPicked,
        &GlobalD3dState.DeviceContext
    );

    if (FAILED(Result))
    {
        return -1;
    }

    ID3D11Texture2D *BackBuffer;

    Result = GlobalD3dState.SwapChain->GetBuffer
    (
        0, __uuidof(ID3D11Texture2D), (void **)&BackBuffer
    );

    if (FAILED(Result))
    {
        return -1;
    }

    Result = GlobalD3dState.Device->CreateRenderTargetView
    (
        BackBuffer, NULL, &GlobalD3dState.RenderTargetView
    );

    if (FAILED(Result))
    {
        return -1;
    }

    SafeReleaseComObject((IUnknown **)&BackBuffer);

    D3D11_TEXTURE2D_DESC DepthStencilBufferDescriptor = {};
    DepthStencilBufferDescriptor.ArraySize = 1;
    DepthStencilBufferDescriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    DepthStencilBufferDescriptor.CPUAccessFlags = 0;
    DepthStencilBufferDescriptor.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DepthStencilBufferDescriptor.Width = ClientAreaWidth;
    DepthStencilBufferDescriptor.Height = ClientAreaHeight;
    DepthStencilBufferDescriptor.MipLevels = 1;
    DepthStencilBufferDescriptor.SampleDesc.Count = 1;
    DepthStencilBufferDescriptor.SampleDesc.Quality = 0;
    DepthStencilBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;

    Result = GlobalD3dState.Device->CreateTexture2D
    (
        &DepthStencilBufferDescriptor, NULL,
        &GlobalD3dState.DepthStencilBuffer
    );

    if (FAILED(Result))
    {
        return -1;
    }

    Result = GlobalD3dState.Device->CreateDepthStencilView
    (
        GlobalD3dState.DepthStencilBuffer, NULL, &GlobalD3dState.DepthStencilView
    );

    if (FAILED(Result))
    {
        return -1;
    }

    D3D11_DEPTH_STENCIL_DESC DepthStencilDescriptor = {};
    DepthStencilDescriptor.DepthEnable = TRUE;
    DepthStencilDescriptor.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DepthStencilDescriptor.DepthFunc = D3D11_COMPARISON_LESS;
    DepthStencilDescriptor.StencilEnable = FALSE;

    Result = GlobalD3dState.Device->CreateDepthStencilState
    (
        &DepthStencilDescriptor, &GlobalD3dState.DepthStencilState
    );

    if (FAILED(Result))
    {
        return -1;
    }

    D3D11_RASTERIZER_DESC RasterizerDescriptor = {};
    RasterizerDescriptor.AntialiasedLineEnable = FALSE;
    RasterizerDescriptor.CullMode = D3D11_CULL_BACK;
    RasterizerDescriptor.DepthBias = 0;
    RasterizerDescriptor.DepthBiasClamp = 0.0f;
    RasterizerDescriptor.DepthClipEnable = TRUE;
    RasterizerDescriptor.FillMode = D3D11_FILL_SOLID;
    RasterizerDescriptor.FrontCounterClockwise = FALSE;
    RasterizerDescriptor.MultisampleEnable = FALSE;
    RasterizerDescriptor.ScissorEnable = FALSE;
    RasterizerDescriptor.SlopeScaledDepthBias = 0.0f;

    Result = GlobalD3dState.Device->CreateRasterizerState
    (
        &RasterizerDescriptor, &GlobalD3dState.RasterizerState
    );

    if (FAILED(Result))
    {
        return -1;
    }

    GlobalD3dState.ViewPort.Width = (f32)ClientAreaWidth;
    GlobalD3dState.ViewPort.Height = (f32)ClientAreaHeight;
    GlobalD3dState.ViewPort.TopLeftX = 0;
    GlobalD3dState.ViewPort.TopLeftY = 0;
    GlobalD3dState.ViewPort.MinDepth = 0;
    GlobalD3dState.ViewPort.MaxDepth = 1.0f;

    return 0;
}

LRESULT CALLBACK
MainWindowCallback(HWND Window, u32 Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT PaintStruct;
            HDC DC = BeginPaint(Window, &PaintStruct);
            EndPaint(Window, &PaintStruct);
        } break;

        case WM_DESTROY:
        case WM_CLOSE:
        {
            PostQuitMessage(0);
        } break;

        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }

    return Result;
}

int CALLBACK WinMain
(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    i32 nCmdShow
)
{
    InitializeGlobalState();

    WNDCLASSEX WindowClass = {};
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = &MainWindowCallback;
    WindowClass.hInstance = hInstance;
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    WindowClass.lpszMenuName = NULL;
    WindowClass.lpszClassName = GlobalWindowData.ClassName;

    if (!RegisterClassEx(&WindowClass))
    {
        printf("ERROR: cannot register the main window class.\n");
        return -1;
    }

    RECT WindowRectangle;
    WindowRectangle.left = 0;
    WindowRectangle.top = 0;
    WindowRectangle.right = GlobalWindowData.Width;
    WindowRectangle.bottom = GlobalWindowData.Height;
    AdjustWindowRect(&WindowRectangle, WS_OVERLAPPEDWINDOW, FALSE);

    GlobalWindowData.Handle = CreateWindowA
    (
        GlobalWindowData.ClassName,
        GlobalWindowData.Name,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WindowRectangle.right - WindowRectangle.left,
        WindowRectangle.bottom - WindowRectangle.top,
        NULL, NULL, hInstance, NULL
    );

    if (!GlobalWindowData.Handle)
    {
        printf("ERROR: cannot create the main window.\n");
        return -1;
    }

    InitializeD3dState(hInstance, GlobalWindowData.EnableVSync);

    ShowWindow(GlobalWindowData.Handle, nCmdShow);
    UpdateWindow(GlobalWindowData.Handle);

    MSG Message = {};

    f32 TargetFrameRate = 30.0f;
    f32 TargetFrameTime = 1.0f / TargetFrameRate;
    DWORD PreviousFrameTime = timeGetTime();

    while (Message.message != WM_QUIT)
    {
        if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        else
        {
            DWORD CurrentTime = timeGetTime();
            f32 TimeDelta = (CurrentTime - PreviousFrameTime) / 1000.0f;
            PreviousFrameTime = CurrentTime;

            TimeDelta = Min(TimeDelta, TargetFrameTime);

            Update(TimeDelta);
            Render();
        }
    }

    return (i32)Message.wParam;
}