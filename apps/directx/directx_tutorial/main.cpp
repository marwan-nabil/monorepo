#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL)

struct custom_vertex
{
    FLOAT x, y, z;
    D3DVECTOR Normal;
};

LPDIRECT3DVERTEXBUFFER9 VertexBuffer = NULL;
LPDIRECT3DINDEXBUFFER9 IndexBuffer = NULL;
LPDIRECT3D9 D3DInterface;
LPDIRECT3DDEVICE9 D3DDevice;
bool Running;
float XPosition = 0.0f, YPosition = 0.0f;
float YRotation = 0.0f;

void InitD3D(HWND hWnd);
void RenderFrame(void);
void ReleaseD3D(void);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam);
void InitGraphics(void);
void InitLightAndMaterial(void);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND Window;
    WNDCLASSEX WindowClass;

    ZeroMemory(&WindowClass, sizeof(WNDCLASSEX));

    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = hInstance;
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    // WindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    WindowClass.lpszClassName = L"WindowClass";

    RegisterClassEx(&WindowClass);

    Window = CreateWindowEx(NULL,
                            L"WindowClass",
                            L"Direct3D App",
                            WS_OVERLAPPEDWINDOW,
                            0, 0,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            NULL, NULL, hInstance, NULL);

    ShowWindow(Window, nCmdShow);
    InitD3D(Window);

    MSG msg;
    Running = true;
    while(Running)
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        RenderFrame();
    }

    ReleaseD3D();
    return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    DWORD VKCode = wParam;
    switch(message)
    {
        case WM_KEYDOWN:
            switch(VKCode)
            {
                case 'W':
                    YPosition += 5;
                    break;
                case 'S':
                    YPosition -= 5;
                    break;
                case 'D':
                    XPosition += 5;
                    break;
                case 'A':
                    XPosition -= 5;
                    break;
                case 'E':
                    YRotation += 0.5f;
                    break;
                case 'Q':
                    YRotation -= 0.5f;
                    break;
                default:
                    break;
            }
            break;
        case WM_QUIT:
            Running = false;
            break;

        case WM_DESTROY:
            Running = false;
            PostQuitMessage(0);
            return 0;
            break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


void InitLightAndMaterial(void)
{
    D3DLIGHT9 Light;    // create the Light struct
    D3DMATERIAL9 Material; // create a material

    ZeroMemory(&Light, sizeof(Light));    // clear out the Light struct for use
    Light.Type = D3DLIGHT_POINT;    // make the Light type 'directional Light'
    Light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);    // set the Light's color
    Light.Direction = D3DXVECTOR3(-1.0f, -0.3f, -1.0f);
    Light.Position = D3DXVECTOR3(0.0f, 5.0f, 0.0f);
    Light.Range = 100.0f;    // a range of 100
    Light.Attenuation0 = 0.0f;    // no constant inverse attenuation
    Light.Attenuation1 = 0.125f;    // only .125 inverse attenuation
    Light.Attenuation2 = 0.0f;    // no square inverse attenuation

    D3DDevice->SetLight(0, &Light);    // send the Light struct properties to Light #0
    D3DDevice->LightEnable(0, TRUE);    // turn on Light #0

    ZeroMemory(&Material, sizeof(D3DMATERIAL9));    // clear out the struct for use
    Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set diffuse color to white
    Material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set ambient color to white

    D3DDevice->SetMaterial(&Material);
}



void InitD3D(HWND Window)
{
    D3DInterface = Direct3DCreate9(D3D_SDK_VERSION); // create the Direct3D interface

    D3DPRESENT_PARAMETERS D3DPresentParams;

    ZeroMemory(&D3DPresentParams, sizeof(D3DPresentParams)); // clear out the struct for use
    D3DPresentParams.Windowed = TRUE;
    D3DPresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD; // discard old frames
    D3DPresentParams.hDeviceWindow = Window; // set the window to be used by Direct3D
    D3DPresentParams.BackBufferFormat = D3DFMT_X8R8G8B8; // set the back buffer format to 32-bit
    D3DPresentParams.BackBufferWidth = SCREEN_WIDTH;
    D3DPresentParams.BackBufferHeight = SCREEN_HEIGHT;
    D3DPresentParams.EnableAutoDepthStencil = TRUE;
    D3DPresentParams.AutoDepthStencilFormat = D3DFMT_D16;
    // create a device class using this information and the info from the D3DPresentParams stuct
    D3DInterface->CreateDevice(D3DADAPTER_DEFAULT,
                               D3DDEVTYPE_HAL,
                               Window,
                               D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                               &D3DPresentParams,
                               &D3DDevice);
    InitGraphics();
    InitLightAndMaterial();
    D3DDevice->SetRenderState(D3DRS_LIGHTING, true);
    D3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));
    D3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE); // turn on the z-buffer
    D3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE); // handle normals scaling
}


// this is the function used to render a single frame
void RenderFrame(void)
{
    // clear the window to a deep blue, and the z-buffer
    D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 40, 100), 1.0f, 0);
    D3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    D3DDevice->BeginScene();
    D3DDevice->SetFVF(CUSTOMFVF); // select which vertex format we are using

    // view transform
    D3DXMATRIX MatView;     // the view transform matrix
    D3DXMatrixLookAtLH(&MatView,
                       &D3DXVECTOR3(XPosition, YPosition, 10.0f),     // the camera position
                       &D3DXVECTOR3(0.0f, 0.0f, 0.0f),      // the look-at position
                       &D3DXVECTOR3(0.0f, 1.0f, 0.0f));     // the up direction
    D3DDevice->SetTransform(D3DTS_VIEW, &MatView);  // set the view transform to MatView

    // projection transform
    D3DXMATRIX MatProjection;     // the projection transform matrix
    D3DXMatrixPerspectiveFovLH(&MatProjection,
                               D3DXToRadian(45),    // the horizontal field of view
                               (FLOAT) SCREEN_WIDTH / (FLOAT) SCREEN_HEIGHT, // aspect ratio
                               1.0f,    // the near view-plane
                               100.0f); // the far view-plane
    D3DDevice->SetTransform(D3DTS_PROJECTION, &MatProjection);    // set the projection

    // set the world transforms
    D3DXMATRIX MatRotateY; // a matrix to store the rotation for each triangle
    D3DXMatrixRotationY(&MatRotateY, YRotation); // the rotation matrix
    D3DXMATRIX MatTranslate;
    D3DXMatrixTranslation(&MatTranslate,
                          (float) sin(YRotation) * 12.0f, 0.0f, 
                          (float) cos(YRotation) * 25.0f);
    D3DDevice->SetTransform(D3DTS_WORLD, &(MatTranslate)); // set the world transform

    // select the vertex buffer to display
    D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(custom_vertex));
    D3DDevice->SetIndices(IndexBuffer);
    D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

    D3DDevice->EndScene();
    D3DDevice->Present(NULL, NULL, NULL, NULL);
}


void ReleaseD3D(void)
{
    VertexBuffer->Release();
    IndexBuffer->Release();
    D3DDevice->Release();
    D3DInterface->Release();
}


void InitGraphics()
{
    // --------------------------
    // create vertex buffer
    // create the Vertices using the custom_vertex struct
    custom_vertex Vertices[] =
    {
        {-3.0f, -3.0f, 3.0f, 0.0f, 0.0f, 1.0f, },    // side 1
        {3.0f, -3.0f, 3.0f, 0.0f, 0.0f, 1.0f, },
        {-3.0f, 3.0f, 3.0f, 0.0f, 0.0f, 1.0f, },
        {3.0f, 3.0f, 3.0f, 0.0f, 0.0f, 1.0f, },

        {-3.0f, -3.0f, -3.0f, 0.0f, 0.0f, -1.0f, },    // side 2
        {-3.0f, 3.0f, -3.0f, 0.0f, 0.0f, -1.0f, },
        {3.0f, -3.0f, -3.0f, 0.0f, 0.0f, -1.0f, },
        {3.0f, 3.0f, -3.0f, 0.0f, 0.0f, -1.0f, },

        {-3.0f, 3.0f, -3.0f, 0.0f, 1.0f, 0.0f, },    // side 3
        {-3.0f, 3.0f, 3.0f, 0.0f, 1.0f, 0.0f, },
        {3.0f, 3.0f, -3.0f, 0.0f, 1.0f, 0.0f, },
        {3.0f, 3.0f, 3.0f, 0.0f, 1.0f, 0.0f, },

        {-3.0f, -3.0f, -3.0f, 0.0f, -1.0f, 0.0f, },    // side 4
        {3.0f, -3.0f, -3.0f, 0.0f, -1.0f, 0.0f, },
        {-3.0f, -3.0f, 3.0f, 0.0f, -1.0f, 0.0f, },
        {3.0f, -3.0f, 3.0f, 0.0f, -1.0f, 0.0f, },

        {3.0f, -3.0f, -3.0f, 1.0f, 0.0f, 0.0f, },    // side 5
        {3.0f, 3.0f, -3.0f, 1.0f, 0.0f, 0.0f, },
        {3.0f, -3.0f, 3.0f, 1.0f, 0.0f, 0.0f, },
        {3.0f, 3.0f, 3.0f, 1.0f, 0.0f, 0.0f, },

        {-3.0f, -3.0f, -3.0f, -1.0f, 0.0f, 0.0f, },    // side 6
        {-3.0f, -3.0f, 3.0f, -1.0f, 0.0f, 0.0f, },
        {-3.0f, 3.0f, -3.0f, -1.0f, 0.0f, 0.0f, },
        {-3.0f, 3.0f, 3.0f, -1.0f, 0.0f, 0.0f, },
    };
    // create a vertex buffer interface called VertexBuffer
    D3DDevice->CreateVertexBuffer(24 * sizeof(custom_vertex),
                                  0, CUSTOMFVF,
                                  D3DPOOL_MANAGED,
                                  &VertexBuffer, NULL);
    VOID *BufferBeginning;
    // lock VertexBuffer and load the Vertices into it
    VertexBuffer->Lock(0, 0, (void **) &BufferBeginning, 0);
    memcpy(BufferBeginning, Vertices, sizeof(Vertices));
    VertexBuffer->Unlock();

    // ---------------------------
    // create index buffer
    // create the Indices using an int array
    short Indices[] =
    {
        0, 1, 2,    // side 1
        2, 1, 3,
        4, 5, 6,    // side 2
        6, 5, 7,
        8, 9, 10,    // side 3
        10, 9, 11,
        12, 13, 14,    // side 4
        14, 13, 15,
        16, 17, 18,    // side 5
        18, 17, 19,
        20, 21, 22,    // side 6
        22, 21, 23,
    };

    // create an index buffer interface called i_buffer
    D3DDevice->CreateIndexBuffer(36 * sizeof(short),
                              0,
                              D3DFMT_INDEX16,
                              D3DPOOL_MANAGED,
                              &IndexBuffer,
                              NULL);

    // lock i_buffer and load the Indices into it
    IndexBuffer->Lock(0, 0, (void **) &BufferBeginning, 0);
    memcpy(BufferBeginning, Indices, sizeof(Indices));
    IndexBuffer->Unlock();
}