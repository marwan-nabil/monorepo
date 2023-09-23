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

#include "../../math/floats.cpp"

#include "directx_demo.h"

window_data GlobalWindowData;
vertex GlobalCubeVertices[8];
u16 GlobalCubeVertexIndices[36];
directx_state GlobalDirectxState;

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

void Update(f32 TimeDelta)
{

}

void Render()
{

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
            HDC DC;
            DC = BeginPaint(Window, &PaintStruct);
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

void
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
    SwapChainDescriptor.BufferDesc.RefreshRate = QueryRefreshRate(ClientAreaWidth, ClientAreaHeight, EnableVSync);
    SwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDescriptor.OutputWindow = GlobalWindowData.Handle;
    SwapChainDescriptor.SampleDesc.Count = 1;
    SwapChainDescriptor.SampleDesc.Quality = 0;
    SwapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    SwapChainDescriptor.Windowed = TRUE;
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