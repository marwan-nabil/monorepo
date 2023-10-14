#pragma once

struct application_data
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

struct d3d_state
{
    IDXGISwapChain *SwapChain;
    ID3D11Device *Device;
    ID3D11DeviceContext *DeviceContext;
    ID3D11RenderTargetView *RenderTargetView;
    ID3D11DepthStencilView *DepthStencilView;
    ID3D11Texture2D *DepthStencilBuffer;
    ID3D11DepthStencilState *DepthStencilState;
    ID3D11RasterizerState *RasterizerState;
    D3D11_VIEWPORT ViewPort;
    ID3D11InputLayout *InputLayout;
    ID3D11Buffer *VertexBuffer;
    ID3D11Buffer *IndexBuffer;
    ID3D11Buffer *ConstantBuffers[CBT_COUNT];
    ID3D11VertexShader *VertexShader;
    ID3D11PixelShader *PixelShader;
    DirectX::XMMATRIX WorldMatrix;
    DirectX::XMMATRIX ViewMatrix;
    DirectX::XMMATRIX ProjectionMatrix;
};

struct vertex
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT3 Color;
};

struct scene_data
{
    vertex CubeVertices[8];
    u16 CubeVertexIndices[36];
    f32 CubeRotationAngle;
};