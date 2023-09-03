#pragma once

struct dx11_backend_data
{
    ID3D11Device *pd3dDevice;
    ID3D11DeviceContext *pd3dDeviceContext;
    IDXGIFactory *pFactory;
    ID3D11Buffer *pVB;
    ID3D11Buffer *pIB;
    ID3D11VertexShader *pVertexShader;
    ID3D11InputLayout *pInputLayout;
    ID3D11Buffer *pVertexConstantBuffer;
    ID3D11PixelShader *pPixelShader;
    ID3D11SamplerState *pFontSampler;
    ID3D11ShaderResourceView *pFontTextureView;
    ID3D11RasterizerState *pRasterizerState;
    ID3D11BlendState *pBlendState;
    ID3D11DepthStencilState *pDepthStencilState;
    i32 VertexBufferSize;
    i32 IndexBufferSize;

    dx11_backend_data()
    {
        memset((void*)this, 0, sizeof(*this));
        VertexBufferSize = 5000;
        IndexBufferSize = 10000;
    }
};

struct dx11_backup_state
{
    UINT ScissorRectsCount, ViewportsCount;
    D3D11_RECT ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    D3D11_VIEWPORT Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    ID3D11RasterizerState *RS;
    ID3D11BlendState *BlendState;
    FLOAT BlendFactor[4];
    UINT SampleMask;
    UINT StencilRef;
    ID3D11DepthStencilState *DepthStencilState;
    ID3D11ShaderResourceView *PSShaderResource;
    ID3D11SamplerState *PSSampler;
    ID3D11PixelShader *PS;
    ID3D11VertexShader *VS;
    ID3D11GeometryShader *GS;
    UINT PSInstancesCount, VSInstancesCount, GSInstancesCount;
    // 256 is max according to PSSetShader documentation
    ID3D11ClassInstance *PSInstances[256], *VSInstances[256], *GSInstances[256];
    D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology;
    ID3D11Buffer *IndexBuffer, *VertexBuffer, *VSConstantBuffer;
    UINT IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
    DXGI_FORMAT IndexBufferFormat;
    ID3D11InputLayout *InputLayout;
};

struct dx11_vertex_constant_buffer
{
    f32 mvp[4][4];
};