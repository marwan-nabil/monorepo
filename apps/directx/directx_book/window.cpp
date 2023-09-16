#include <Windows.h>
#include <d3dx9.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "winmm.lib")

#include "window.h"
#include "cube.h"
#include "utils.h"


int Running;
IDirect3D9 *D3DInterface;
IDirect3DDevice9 *D3DDevice;
IDirect3DVertexBuffer9 *VertexBuffer;
IDirect3DIndexBuffer9 *IndexBuffer;
IDirect3DTexture9 *Texture;
cube Cube;

// -------------------------------------
//  using textures
// -------------------------------------


void SetupPipeline()
{
    Cube = MakeCube(D3DDevice);
    // make a directional light
    D3DXVECTOR3 Direction = {1.0f, -1.0f, 0.0f};
    D3DXCOLOR Color = {0.8f, 0.8f, 0.8f, 1.0f};
    D3DLIGHT9 DirLight = InitDirectionalLight(&Direction, &Color);
    D3DDevice->SetLight(0, &DirLight);
    D3DDevice->LightEnable(0, 1);

    D3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, 1);
    D3DDevice->SetRenderState(D3DRS_SPECULARENABLE, 1);

    D3DXCreateTextureFromFileA(D3DDevice, "crate.jpg", &Texture);

    // enable some texture filters
    D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    D3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    // set-up the projection matrix
    D3DXMATRIX ProjMat;
    D3DXMatrixPerspectiveFovLH(&ProjMat, D3DX_PI * 0.5, 
                               ((float) WINDOW_WIDTH / (float) WINDOW_HEIGHT),
                               1.0f, 1000.0f);
    D3DDevice->SetTransform(D3DTS_PROJECTION, &ProjMat);
}


int
UpdateAndRender(float TimeDelta)
{
    if(D3DDevice)
    {
        // 
        // Update the scene: update camera position.
        //
        static float angle = (3.0f * D3DX_PI) / 2.0f;
        static float height = 2.0f;

        if(GetAsyncKeyState(VK_LEFT) & 0x8000f)
            angle -= 0.5f * TimeDelta;

        if(GetAsyncKeyState(VK_RIGHT) & 0x8000f)
            angle += 0.5f * TimeDelta;

        if(GetAsyncKeyState(VK_UP) & 0x8000f)
            height += 5.0f * TimeDelta;

        if(GetAsyncKeyState(VK_DOWN) & 0x8000f)
            height -= 5.0f * TimeDelta;

        D3DXVECTOR3 position(cosf(angle) * 3.0f, height, sinf(angle) * 3.0f);
        D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
        D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
        D3DXMATRIX V;
        D3DXMatrixLookAtLH(&V, &position, &target, &up);

        D3DDevice->SetTransform(D3DTS_VIEW, &V);

        // Draw the scene:
        D3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
        D3DDevice->BeginScene();

        D3DDevice->SetMaterial(&WHITE_MTRL);
        D3DDevice->SetTexture(0, Texture);

        DrawCube(&Cube, 0, 0, 0);

        D3DDevice->EndScene();
        D3DDevice->Present(0, 0, 0, 0);
    }
    return 1;
}

void
InitializeDirect3D(HWND Window)
{
    D3DInterface = Direct3DCreate9(D3D_SDK_VERSION);
    D3DCAPS9 DeviceCaps;
    D3DInterface->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps);
    int Flags = 0;
    if(DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        Flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else
    {
        Flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    D3DPRESENT_PARAMETERS PresentParams;
    PresentParams.BackBufferWidth = WINDOW_WIDTH;
    PresentParams.BackBufferHeight = WINDOW_HEIGHT;
    PresentParams.BackBufferFormat = D3DFMT_A8R8G8B8;
    PresentParams.BackBufferCount = 1;
    PresentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
    PresentParams.MultiSampleQuality = 0;
    PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    PresentParams.hDeviceWindow = Window;
    PresentParams.Windowed = 1;
    PresentParams.EnableAutoDepthStencil = 1;
    PresentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
    PresentParams.Flags = 0;
    PresentParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    HRESULT Result = D3DInterface->CreateDevice(
        D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, Flags, &PresentParams, &D3DDevice
        );
    if(FAILED(Result))
    {
        MessageBoxA(0, "D3DInterface->CreateDevice() Failed.", 0, 0);
    }
}


void
CleanUp()
{
    D3DDevice->Release();
    D3DInterface->Release();
    VertexBuffer->Release();
}


LRESULT CALLBACK
MainWindowProcedure(HWND   Window,
                    UINT   Message,
                    WPARAM wParam,
                    LPARAM lParam)
{
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_KEYDOWN:
            if(wParam == VK_ESCAPE)
            {
                DestroyWindow(Window);
            }
            break;
        case WM_CLOSE:
        {
            PostQuitMessage(0);
        } break;

        default:
        {
            Result = DefWindowProcA(Window, Message, wParam, lParam);
        } break;
    }
    return Result;
}


int CALLBACK
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR     lpCmdLine,
        int       nCmdShow)
{
    // ---------------
    // create a window
    // ---------------
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = MainWindowProcedure;
    WindowClass.hInstance = hInstance;
    WindowClass.lpszClassName = "MainWindowClass";

    HRESULT LastResult = RegisterClassA(&WindowClass);
    if(FAILED(LastResult))
    {
        OutputDebugStringA("RegisterClassA() FAILED, Exiting.\n");
        return 0;
    }

    HWND Window = CreateWindowExA(0, "MainWindowClass", "Backbuffer Experiments",
                                  WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  0, 0, hInstance, 0);
    if(!Window)
    {
        OutputDebugStringA("CreateWindowExA() FAILED, Exiting.\n");
        return 0;
    }

    // ----------
    // init stuff
    // ----------
    InitializeDirect3D(Window);
    SetupPipeline();

    // ---------
    // main loop
    // ---------
    Running = 1;
    float LastTime = (float) timeGetTime();
    while(Running)
    {
        // -------------------
        // handle all messages
        // -------------------
        MSG Message;
        while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            if(Message.message == WM_QUIT)
            {
                return (int) Message.wParam;
            }
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }

        // -----------------
        // update and render
        // -----------------
        float Time = (float) timeGetTime();
        float TimeDelta = (Time - LastTime) * 0.001f;
        LastTime = Time;
        UpdateAndRender(TimeDelta);
    }

    CleanUp();
    return(0);
}