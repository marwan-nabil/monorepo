// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static dx11_backend_data *Dx11_GetBackendData()
{
    if (ImGui::GetCurrentContext())
    {
        dx11_backend_data *Result = (dx11_backend_data *)ImGui::GetIO().BackendRendererUserData;
        return Result;
    }
    else
    {
        return NULL;
    }
}

static void Dx11_InvalidateDeviceObjects()
{
    dx11_backend_data *BackendData = Dx11_GetBackendData();
    if (!BackendData->pd3dDevice)
    {
        return;
    }

    if (BackendData->pFontSampler)
    {
        BackendData->pFontSampler->Release();
        BackendData->pFontSampler = NULL;
    }

    // We copied data->pFontTextureView to io.Fonts->TexID so let's clear that as well.
    if (BackendData->pFontTextureView)
    {
        BackendData->pFontTextureView->Release();
        BackendData->pFontTextureView = NULL;
        ImGui::GetIO().Fonts->SetTexID(0);
    }

    if (BackendData->pIB)
    {
        BackendData->pIB->Release();
        BackendData->pIB = NULL;
    }

    if (BackendData->pVB)
    {
        BackendData->pVB->Release();
        BackendData->pVB = NULL;
    }

    if (BackendData->pBlendState)
    {
        BackendData->pBlendState->Release();
        BackendData->pBlendState = NULL;
    }

    if (BackendData->pDepthStencilState)
    {
        BackendData->pDepthStencilState->Release();
        BackendData->pDepthStencilState = NULL;
    }

    if (BackendData->pRasterizerState)
    {
        BackendData->pRasterizerState->Release();
        BackendData->pRasterizerState = NULL;
    }

    if (BackendData->pPixelShader)
    {
        BackendData->pPixelShader->Release();
        BackendData->pPixelShader = NULL;
    }

    if (BackendData->pVertexConstantBuffer)
    {
        BackendData->pVertexConstantBuffer->Release();
        BackendData->pVertexConstantBuffer = NULL;
    }

    if (BackendData->pInputLayout)
    {
        BackendData->pInputLayout->Release();
        BackendData->pInputLayout = NULL;
    }

    if (BackendData->pVertexShader)
    {
        BackendData->pVertexShader->Release();
        BackendData->pVertexShader = NULL;
    }
}

static void Dx11_SetupRenderState(ImDrawData *DrawData, ID3D11DeviceContext *D3dDeviceContext)
{
    dx11_backend_data *BackendData = Dx11_GetBackendData();

    D3D11_VIEWPORT D3dViewPort;
    ZeroMemory(&D3dViewPort, sizeof(D3D11_VIEWPORT));
    D3dViewPort.Width = DrawData->DisplaySize.x;
    D3dViewPort.Height = DrawData->DisplaySize.y;
    D3dViewPort.MinDepth = 0.0f;
    D3dViewPort.MaxDepth = 1.0f;
    D3dViewPort.TopLeftX = 0;
    D3dViewPort.TopLeftY = 0;
    D3dDeviceContext->RSSetViewports(1, &D3dViewPort);

    // Setup shader and vertex buffers
    u32 Stride = sizeof(ImDrawVert);
    u32 Offset = 0;
    D3dDeviceContext->IASetInputLayout(BackendData->pInputLayout);
    D3dDeviceContext->IASetVertexBuffers(0, 1, &BackendData->pVB, &Stride, &Offset);
    D3dDeviceContext->IASetIndexBuffer(BackendData->pIB, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
    D3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    D3dDeviceContext->VSSetShader(BackendData->pVertexShader, NULL, 0);
    D3dDeviceContext->VSSetConstantBuffers(0, 1, &BackendData->pVertexConstantBuffer);
    D3dDeviceContext->PSSetShader(BackendData->pPixelShader, NULL, 0);
    D3dDeviceContext->PSSetSamplers(0, 1, &BackendData->pFontSampler);
    D3dDeviceContext->GSSetShader(NULL, NULL, 0);
    D3dDeviceContext->HSSetShader(NULL, NULL, 0); // In theory we should backup and restore this as well.. very infrequently used..
    D3dDeviceContext->DSSetShader(NULL, NULL, 0); // In theory we should backup and restore this as well.. very infrequently used..
    D3dDeviceContext->CSSetShader(NULL, NULL, 0); // In theory we should backup and restore this as well.. very infrequently used..

    // Setup blend state
    f32 BlendFactor[4] = {0.f, 0.f, 0.f, 0.f};
    D3dDeviceContext->OMSetBlendState(BackendData->pBlendState, BlendFactor, 0xffffffff);
    D3dDeviceContext->OMSetDepthStencilState(BackendData->pDepthStencilState, 0);
    D3dDeviceContext->RSSetState(BackendData->pRasterizerState);
}

void Dx11_RenderDrawData(ImDrawData *DrawData)
{
    // Avoid rendering when minimized
    if ((DrawData->DisplaySize.x <= 0.0f) || (DrawData->DisplaySize.y <= 0.0f))
    {
        return;
    }

    dx11_backend_data *BackendData = Dx11_GetBackendData();
    ID3D11DeviceContext *D3dDeviceContext = BackendData->pd3dDeviceContext;

    // Create and grow vertex/index buffers if needed
    if
    (
        !BackendData->pVB || 
        (BackendData->VertexBufferSize < DrawData->TotalVtxCount)
    )
    {
        if (BackendData->pVB)
        {
            BackendData->pVB->Release();
            BackendData->pVB = NULL;
        }
        BackendData->VertexBufferSize = DrawData->TotalVtxCount + 5000;

        D3D11_BUFFER_DESC BufferDescriptor;
        ZeroMemory(&BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        BufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
        BufferDescriptor.ByteWidth = BackendData->VertexBufferSize * sizeof(ImDrawVert);
        BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        BufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        BufferDescriptor.MiscFlags = 0;
        if (BackendData->pd3dDevice->CreateBuffer(&BufferDescriptor, NULL, &BackendData->pVB) < 0)
        {
            return;
        }
    }

    if
    (
        !BackendData->pIB || 
        (BackendData->IndexBufferSize < DrawData->TotalIdxCount)
    )
    {
        if (BackendData->pIB)
        {
            BackendData->pIB->Release();
            BackendData->pIB = NULL;
        }
        BackendData->IndexBufferSize = DrawData->TotalIdxCount + 10000;

        D3D11_BUFFER_DESC BufferDescriptor;
        ZeroMemory(&BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        BufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
        BufferDescriptor.ByteWidth = BackendData->IndexBufferSize * sizeof(ImDrawIdx);
        BufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
        BufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        if (BackendData->pd3dDevice->CreateBuffer(&BufferDescriptor, NULL, &BackendData->pIB) < 0)
        {
            return;
        }
    }

    // Upload vertex/index data into a single contiguous GPU buffer
    D3D11_MAPPED_SUBRESOURCE VertexResource, IndexResource;
    if (D3dDeviceContext->Map(BackendData->pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &VertexResource) != S_OK)
    {
        return;
    }

    if (D3dDeviceContext->Map(BackendData->pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &IndexResource) != S_OK)
    {
        return;
    }

    ImDrawVert *VertexDestination = (ImDrawVert *)VertexResource.pData;
    ImDrawIdx *IndexDestination = (ImDrawIdx *)IndexResource.pData;

    for (u32 CommandListIndex = 0; CommandListIndex < DrawData->CmdListsCount; CommandListIndex++)
    {
        ImDrawList *CommandList = DrawData->CmdLists[CommandListIndex];
        memcpy(VertexDestination, CommandList->VtxBuffer.Data, CommandList->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(IndexDestination, CommandList->IdxBuffer.Data, CommandList->IdxBuffer.Size * sizeof(ImDrawIdx));
        VertexDestination += CommandList->VtxBuffer.Size;
        IndexDestination += CommandList->IdxBuffer.Size;
    }
    D3dDeviceContext->Unmap(BackendData->pVB, 0);
    D3dDeviceContext->Unmap(BackendData->pIB, 0);

    // Setup orthographic projection matrix into our constant buffer
    // Our visible imgui space lies from DrawData->DisplayPos (top left) to DrawData->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    D3D11_MAPPED_SUBRESOURCE MappedResource;

    HRESULT Result = D3dDeviceContext->Map(BackendData->pVertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    if (Result != S_OK)
    {
        return;
    }

    dx11_vertex_constant_buffer *ConstantBuffer = (dx11_vertex_constant_buffer *)MappedResource.pData;
    f32 L = DrawData->DisplayPos.x;
    f32 R = DrawData->DisplayPos.x + DrawData->DisplaySize.x;
    f32 T = DrawData->DisplayPos.y;
    f32 B = DrawData->DisplayPos.y + DrawData->DisplaySize.y;

    f32 MVP[4][4] =
    {
        {2.0f / (R - L), 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f / (T - B), 0.0f, 0.0f},
        {0.0f, 0.0f, 0.5f, 0.0f},
        {(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f},
    };
    memcpy(&ConstantBuffer->mvp, MVP, sizeof(MVP));
    D3dDeviceContext->Unmap(BackendData->pVertexConstantBuffer, 0);

    // Backup DX state that will be modified to restore it afterwards (unfortunately this is very ugly looking and verbose. Close your eyes!)
    dx11_backup_state OldDx11State = {};
    OldDx11State.ScissorRectsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    OldDx11State.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    D3dDeviceContext->RSGetScissorRects(&OldDx11State.ScissorRectsCount, OldDx11State.ScissorRects);
    D3dDeviceContext->RSGetViewports(&OldDx11State.ViewportsCount, OldDx11State.Viewports);
    D3dDeviceContext->RSGetState(&OldDx11State.RS);
    D3dDeviceContext->OMGetBlendState(&OldDx11State.BlendState, OldDx11State.BlendFactor, &OldDx11State.SampleMask);
    D3dDeviceContext->OMGetDepthStencilState(&OldDx11State.DepthStencilState, &OldDx11State.StencilRef);
    D3dDeviceContext->PSGetShaderResources(0, 1, &OldDx11State.PSShaderResource);
    D3dDeviceContext->PSGetSamplers(0, 1, &OldDx11State.PSSampler);
    OldDx11State.PSInstancesCount = OldDx11State.VSInstancesCount = OldDx11State.GSInstancesCount = 256;
    D3dDeviceContext->PSGetShader(&OldDx11State.PS, OldDx11State.PSInstances, &OldDx11State.PSInstancesCount);
    D3dDeviceContext->VSGetShader(&OldDx11State.VS, OldDx11State.VSInstances, &OldDx11State.VSInstancesCount);
    D3dDeviceContext->VSGetConstantBuffers(0, 1, &OldDx11State.VSConstantBuffer);
    D3dDeviceContext->GSGetShader(&OldDx11State.GS, OldDx11State.GSInstances, &OldDx11State.GSInstancesCount);
    D3dDeviceContext->IAGetPrimitiveTopology(&OldDx11State.PrimitiveTopology);
    D3dDeviceContext->IAGetIndexBuffer(&OldDx11State.IndexBuffer, &OldDx11State.IndexBufferFormat, &OldDx11State.IndexBufferOffset);
    D3dDeviceContext->IAGetVertexBuffers(0, 1, &OldDx11State.VertexBuffer, &OldDx11State.VertexBufferStride, &OldDx11State.VertexBufferOffset);
    D3dDeviceContext->IAGetInputLayout(&OldDx11State.InputLayout);

    // Setup desired DX state
    Dx11_SetupRenderState(DrawData, D3dDeviceContext);

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own Offset into them)
    i32 GlobalIndexOffset = 0;
    i32 GlobalVertexOffset = 0;
    ImVec2 ClipOffset = DrawData->DisplayPos;

    for (i32 CommandListIndex = 0; CommandListIndex < DrawData->CmdListsCount; CommandListIndex++)
    {
        ImDrawList *CommandList = DrawData->CmdLists[CommandListIndex];
        for (i32 CommandIndex = 0; CommandIndex < CommandList->CmdBuffer.Size; CommandIndex++)
        {
            ImDrawCmd *Command = &CommandList->CmdBuffer[CommandIndex];
            if (Command->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (Command->UserCallback == ImDrawCallback_ResetRenderState)
                {
                    Dx11_SetupRenderState(DrawData, D3dDeviceContext);
                }
                else
                {
                    Command->UserCallback(CommandList, Command);
                }
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 ClipMinimum(Command->ClipRect.x - ClipOffset.x, Command->ClipRect.y - ClipOffset.y);
                ImVec2 ClipMaximum(Command->ClipRect.z - ClipOffset.x, Command->ClipRect.w - ClipOffset.y);
                if ((ClipMaximum.x <= ClipMinimum.x) || (ClipMaximum.y <= ClipMinimum.y))
                {
                    continue;
                }

                // Apply scissor/clipping rectangle
                D3D11_RECT Rectangle =
                {
                    (LONG)ClipMinimum.x,
                    (LONG)ClipMinimum.y,
                    (LONG)ClipMaximum.x,
                    (LONG)ClipMaximum.y
                };
                D3dDeviceContext->RSSetScissorRects(1, &Rectangle);

                // Bind texture, Draw
                ID3D11ShaderResourceView *TextureShaderResourceView = (ID3D11ShaderResourceView *)Command->GetTexID();
                D3dDeviceContext->PSSetShaderResources(0, 1, &TextureShaderResourceView);
                D3dDeviceContext->DrawIndexed(Command->ElemCount, Command->IdxOffset + GlobalIndexOffset, Command->VtxOffset + GlobalVertexOffset);
            }
        }
        GlobalIndexOffset += CommandList->IdxBuffer.Size;
        GlobalVertexOffset += CommandList->VtxBuffer.Size;
    }

    // Restore modified DX state
    D3dDeviceContext->RSSetScissorRects(OldDx11State.ScissorRectsCount, OldDx11State.ScissorRects);
    D3dDeviceContext->RSSetViewports(OldDx11State.ViewportsCount, OldDx11State.Viewports);
    D3dDeviceContext->RSSetState(OldDx11State.RS); if (OldDx11State.RS) OldDx11State.RS->Release();
    D3dDeviceContext->OMSetBlendState(OldDx11State.BlendState, OldDx11State.BlendFactor, OldDx11State.SampleMask); if (OldDx11State.BlendState) OldDx11State.BlendState->Release();
    D3dDeviceContext->OMSetDepthStencilState(OldDx11State.DepthStencilState, OldDx11State.StencilRef); if (OldDx11State.DepthStencilState) OldDx11State.DepthStencilState->Release();
    D3dDeviceContext->PSSetShaderResources(0, 1, &OldDx11State.PSShaderResource); if (OldDx11State.PSShaderResource) OldDx11State.PSShaderResource->Release();
    D3dDeviceContext->PSSetSamplers(0, 1, &OldDx11State.PSSampler); if (OldDx11State.PSSampler) OldDx11State.PSSampler->Release();
    D3dDeviceContext->PSSetShader(OldDx11State.PS, OldDx11State.PSInstances, OldDx11State.PSInstancesCount); if (OldDx11State.PS) OldDx11State.PS->Release();

    for (UINT i = 0; i < OldDx11State.PSInstancesCount; i++)
    {
        if (OldDx11State.PSInstances[i])
        {
            OldDx11State.PSInstances[i]->Release();
        }
    }

    D3dDeviceContext->VSSetShader(OldDx11State.VS, OldDx11State.VSInstances, OldDx11State.VSInstancesCount); if (OldDx11State.VS) OldDx11State.VS->Release();
    D3dDeviceContext->VSSetConstantBuffers(0, 1, &OldDx11State.VSConstantBuffer); if (OldDx11State.VSConstantBuffer) OldDx11State.VSConstantBuffer->Release();
    D3dDeviceContext->GSSetShader(OldDx11State.GS, OldDx11State.GSInstances, OldDx11State.GSInstancesCount); if (OldDx11State.GS) OldDx11State.GS->Release();

    for (UINT i = 0; i < OldDx11State.VSInstancesCount; i++)
    {
        if (OldDx11State.VSInstances[i])
        {
            OldDx11State.VSInstances[i]->Release();
        }
    }

    D3dDeviceContext->IASetPrimitiveTopology(OldDx11State.PrimitiveTopology);
    D3dDeviceContext->IASetIndexBuffer(OldDx11State.IndexBuffer, OldDx11State.IndexBufferFormat, OldDx11State.IndexBufferOffset); if (OldDx11State.IndexBuffer) OldDx11State.IndexBuffer->Release();
    D3dDeviceContext->IASetVertexBuffers(0, 1, &OldDx11State.VertexBuffer, &OldDx11State.VertexBufferStride, &OldDx11State.VertexBufferOffset); if (OldDx11State.VertexBuffer) OldDx11State.VertexBuffer->Release();
    D3dDeviceContext->IASetInputLayout(OldDx11State.InputLayout); if (OldDx11State.InputLayout) OldDx11State.InputLayout->Release();
}

static void Dx11_CreateFontsTexture()
{
    // NOTE: stopped here
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    dx11_backend_data *BackendData = Dx11_GetBackendData();
    unsigned char *pixels;
    i32 width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to graphics system
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ID3D11Texture2D *pTexture = NULL;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = pixels;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        BackendData->pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
        IM_ASSERT(pTexture != NULL);

        // Create texture view
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        BackendData->pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &BackendData->pFontTextureView);
        pTexture->Release();
    }

    // Store our identifier
    io.Fonts->SetTexID((ImTextureID)BackendData->pFontTextureView);

    // Create texture sampler
    // (Bilinear sampling is required by default. Set 'io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines' or 'style.AntiAliasedLinesUseTex = false' to allow point/nearest sampling)
    {
        D3D11_SAMPLER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MipLODBias = 0.f;
        desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        desc.MinLOD = 0.f;
        desc.MaxLOD = 0.f;
        BackendData->pd3dDevice->CreateSamplerState(&desc, &BackendData->pFontSampler);
    }
}

bool    Dx11_CreateDeviceObjects()
{
    dx11_backend_data *BackendData = Dx11_GetBackendData();
    if (!BackendData->pd3dDevice)
        return false;
    if (BackendData->pFontSampler)
        Dx11_InvalidateDeviceObjects();

    // By using D3DCompile() from <d3dcompiler.h> / d3dcompiler.lib, we introduce a dependency to a given version of d3dcompiler_XX.dll (see D3DCOMPILER_DLL_A)
    // If you would like to use this DX11 sample code but remove this dependency you can:
    //  1) compile once, save the compiled shader blobs into a file or source code and pass them to CreateVertexShader()/CreatePixelShader() [preferred solution]
    //  2) use code to detect any version of the DLL and grab a pointer to D3DCompile from the DLL.
    // See https://github.com/ocornut/imgui/pull/638 for sources and details.

    // Create the vertex shader
    {
        static const char *vertexShader =
            "cbuffer vertexBuffer : register(b0) \
            {\
              float4x4 ProjectionMatrix; \
            };\
            struct VS_INPUT\
            {\
              float2 pos : POSITION;\
              float4 col : COLOR0;\
              float2 uv  : TEXCOORD0;\
            };\
            \
            struct PS_INPUT\
            {\
              float4 pos : SV_POSITION;\
              float4 col : COLOR0;\
              float2 uv  : TEXCOORD0;\
            };\
            \
            PS_INPUT main(VS_INPUT input)\
            {\
              PS_INPUT output;\
              output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
              output.col = input.col;\
              output.uv  = input.uv;\
              return output;\
            }";

        ID3DBlob *vertexShaderBlob;
        if (FAILED(D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &vertexShaderBlob, NULL)))
            return false; // NB: Pass ID3DBlob *pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
        if (BackendData->pd3dDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &BackendData->pVertexShader) != S_OK)
        {
            vertexShaderBlob->Release();
            return false;
        }

        // Create the input layout
        D3D11_INPUT_ELEMENT_DESC local_layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)IM_OFFSETOF(ImDrawVert, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)IM_OFFSETOF(ImDrawVert, uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (UINT)IM_OFFSETOF(ImDrawVert, col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        if (BackendData->pd3dDevice->CreateInputLayout(local_layout, 3, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &BackendData->pInputLayout) != S_OK)
        {
            vertexShaderBlob->Release();
            return false;
        }
        vertexShaderBlob->Release();

        // Create the constant buffer
        {
            D3D11_BUFFER_DESC desc;
            desc.ByteWidth = sizeof(dx11_vertex_constant_buffer);
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;
            BackendData->pd3dDevice->CreateBuffer(&desc, NULL, &BackendData->pVertexConstantBuffer);
        }
    }

    // Create the pixel shader
    {
        static const char *pixelShader =
            "struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            sampler sampler0;\
            Texture2D texture0;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
            float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
            return out_col; \
            }";

        ID3DBlob *pixelShaderBlob;
        if (FAILED(D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &pixelShaderBlob, NULL)))
            return false; // NB: Pass ID3DBlob *pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
        if (BackendData->pd3dDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &BackendData->pPixelShader) != S_OK)
        {
            pixelShaderBlob->Release();
            return false;
        }
        pixelShaderBlob->Release();
    }

    // Create the blending setup
    {
        D3D11_BLEND_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.AlphaToCoverageEnable = false;
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        BackendData->pd3dDevice->CreateBlendState(&desc, &BackendData->pBlendState);
    }

    // Create the rasterizer state
    {
        D3D11_RASTERIZER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.ScissorEnable = true;
        desc.DepthClipEnable = true;
        BackendData->pd3dDevice->CreateRasterizerState(&desc, &BackendData->pRasterizerState);
    }

    // Create depth-stencil State
    {
        D3D11_DEPTH_STENCIL_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.DepthEnable = false;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        desc.StencilEnable = false;
        desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc.BackFace = desc.FrontFace;
        BackendData->pd3dDevice->CreateDepthStencilState(&desc, &BackendData->pDepthStencilState);
    }

    Dx11_CreateFontsTexture();

    return true;
}

bool    Dx11_Initialize(ID3D11Device *device, ID3D11DeviceContext *device_context)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendRendererUserData == NULL && "Already initialized a renderer backend!");

    // Setup backend capabilities flags
    dx11_backend_data *BackendData = IM_NEW(dx11_backend_data)();
    io.BackendRendererUserData = (void*)BackendData;
    io.BackendRendererName = "imgui_impl_dx11";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.

    // Get factory from device
    IDXGIDevice *pDXGIDevice = NULL;
    IDXGIAdapter *pDXGIAdapter = NULL;
    IDXGIFactory *pFactory = NULL;

    if (device->QueryInterface(IID_PPV_ARGS(&pDXGIDevice)) == S_OK)
        if (pDXGIDevice->GetParent(IID_PPV_ARGS(&pDXGIAdapter)) == S_OK)
            if (pDXGIAdapter->GetParent(IID_PPV_ARGS(&pFactory)) == S_OK)
            {
                BackendData->pd3dDevice = device;
                BackendData->pd3dDeviceContext = device_context;
                BackendData->pFactory = pFactory;
            }
    if (pDXGIDevice) pDXGIDevice->Release();
    if (pDXGIAdapter) pDXGIAdapter->Release();
    BackendData->pd3dDevice->AddRef();
    BackendData->pd3dDeviceContext->AddRef();

    return true;
}

void Dx11_Shutdown()
{
    dx11_backend_data *BackendData = Dx11_GetBackendData();
    IM_ASSERT(BackendData != NULL && "No renderer backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    Dx11_InvalidateDeviceObjects();
    if (BackendData->pFactory)             { BackendData->pFactory->Release(); }
    if (BackendData->pd3dDevice)           { BackendData->pd3dDevice->Release(); }
    if (BackendData->pd3dDeviceContext)    { BackendData->pd3dDeviceContext->Release(); }
    io.BackendRendererName = NULL;
    io.BackendRendererUserData = NULL;
    io.BackendFlags &= ~ImGuiBackendFlags_RendererHasVtxOffset;
    IM_DELETE(BackendData);
}

void Dx11_NewFrame()
{
    dx11_backend_data *BackendData = Dx11_GetBackendData();
    IM_ASSERT(BackendData != NULL && "Did you call Dx11_Initialize()?");

    if (!BackendData->pFontSampler)
        Dx11_CreateDeviceObjects();
}