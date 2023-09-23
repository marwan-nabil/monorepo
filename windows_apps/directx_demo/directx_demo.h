#pragma once

struct window_data
{
    u32 Width;
    u32 Height;
    char *Name;
    char *ClassName;
    HWND Handle;
    b32 EnableVSync;
};

enum constant_buffer_type
{
    CBT_APPLICATION,
    CBT_FRAME,
    CBT_OBJECT,

    CBT_COUNT,
};

struct directx_state
{
    IDXGISwapChain *DxgiSwapChain;
    ID3D11Device *D3dDevice;
    ID3D11DeviceContext *D3dDeviceContext;

    ID3D11RenderTargetView *D3dRenderTargetView;
    ID3D11DepthStencilView *D3dDepthStencilView;
    ID3D11Texture2D *D3dDepthStencilBuffer;

    ID3D11DepthStencilState *D3dDepthStencilState;
    ID3D11RasterizerState *D3dRasterizerState;

    D3D11_VIEWPORT ViewPort;

    ID3D11InputLayout *D3dInputLayout;
    ID3D11Buffer *D3dVertexBuffer;
    ID3D11Buffer *D3dIndexBuffer;

    ID3D11VertexShader *D3dVertexShader;
    ID3D11PixelShader *D3dPixelShader;

    ID3D11Buffer *D3dConstantBuffers[CBT_COUNT];

    DirectX::XMMATRIX WorldMatrix;
    DirectX::XMMATRIX ViewMatrix;
    DirectX::XMMATRIX ProjectionMatrix;
};

struct vertex
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT3 Color;
};