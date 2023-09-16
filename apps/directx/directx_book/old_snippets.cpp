// -------------------------------------
//  paint a surface
// -------------------------------------
//void
//PaintSurfaceRed()
//{
//    D3DSURFACE_DESC SurfaceDesc;
//    Surface->GetDesc(&SurfaceDesc);
//    int SurfaceHeight = SurfaceDesc.Height;
//    int SurfaceWidth = SurfaceDesc.Width;
//
//    // lock the surface and get a pointer to it
//    D3DLOCKED_RECT LockedRect;
//    Surface->LockRect(&LockedRect, 0, 0);
//
//    DWORD *Pixel = (DWORD *) LockedRect.pBits;
//    int Pitch = LockedRect.Pitch;
//    for(int i = 0;
//        i < SurfaceHeight;
//        i++)
//    {
//        for(int j = 0;
//            j < SurfaceWidth;
//            j++)
//        {
//            int dword_index = i * Pitch / 4 + j;
//            Pixel[dword_index] = 0xffff0000; // red
//        }
//    }
//
//    Surface->UnlockRect();
//}


// -------------------------------------
//  a simple triangle
// -------------------------------------
//void SetupPipeline()
//{
//    // fill a vertex buffer with a triangle
//    D3DDevice->CreateVertexBuffer(3 * sizeof(struct vertex),
//                                  D3DUSAGE_WRITEONLY,
//                                  MYFVF,
//                                  D3DPOOL_MANAGED,
//                                  &VertexBuffer, 0);
//    vertex *Vertices;
//    VertexBuffer->Lock(0, 0, (void **) &Vertices, 0);
//    Vertices[0] = {-1.0f, 0.0f, 2.0f};
//    Vertices[1] = {0.0f, 1.0f, 2.0f};
//    Vertices[2] = {1.0f, 0.0f, 2.0f};
//    VertexBuffer->Unlock();
//
//    // setup the projection matrix, no world and view transformations
//    D3DXMATRIX ProjMat;
//    D3DXMatrixPerspectiveFovLH(&ProjMat, D3DX_PI * 0.5f,
//                               (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
//                               1.0f, 1000.0f);
//    D3DDevice->SetTransform(D3DTS_PROJECTION, &ProjMat);
//
//    // set the render mode as wireframe
//    D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
//}
//
//
//int
//UpdateAndRender(float TimeDelta)
//{
//    if(D3DDevice)
//    {
//        D3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
//        D3DDevice->BeginScene();
//
//        D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(struct vertex));
//        D3DDevice->SetFVF(MYFVF);
//
//        // Draw one triangle.
//        D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
//
//        D3DDevice->EndScene();
//        D3DDevice->Present(0, 0, 0, 0);
//    }
//    return 1;
//}




// -------------------------------------
//  a rotating cube
// -------------------------------------
//
//void SetupPipeline()
//{
//    // fill a vertex buffer with a cube
//    D3DDevice->CreateVertexBuffer(8 * sizeof(struct vertex),
//                                  D3DUSAGE_WRITEONLY,
//                                  MYFVF,
//                                  D3DPOOL_MANAGED,
//                                  &VertexBuffer, 0);
//    vertex *Vertices;
//    VertexBuffer->Lock(0, 0, (void **) &Vertices, 0);
//    Vertices[0] = {-1.0f, -1.0f, -1.0f};
//    Vertices[1] = {-1.0f, 1.0f, -1.0f};
//    Vertices[2] = {1.0f, 1.0f, -1.0f};
//    Vertices[3] = {1.0f, -1.0f, -1.0f};
//    Vertices[4] = {-1.0f, -1.0f, 1.0f};
//    Vertices[5] = {-1.0f, 1.0f, 1.0f};
//    Vertices[6] = {1.0f, 1.0f, 1.0f};
//    Vertices[7] = {1.0f, -1.0f, 1.0f};
//    VertexBuffer->Unlock();
//
//    // create an index buffer for that triangle and fill it
//    D3DDevice->CreateIndexBuffer(36 * sizeof(WORD),
//                                 D3DUSAGE_WRITEONLY,
//                                 D3DFMT_INDEX16,
//                                 D3DPOOL_MANAGED,
//                                 &IndexBuffer,
//                                 0);
//    WORD *indices = 0;
//    IndexBuffer->Lock(0, 0, (void **) &indices, 0);
//    // front side
//    indices[0] = 0; indices[1] = 1; indices[2] = 2;
//    indices[3] = 0; indices[4] = 2; indices[5] = 3;
//    // back side
//    indices[6] = 4; indices[7] = 6; indices[8] = 5;
//    indices[9] = 4; indices[10] = 7; indices[11] = 6;
//    // left side
//    indices[12] = 4; indices[13] = 5; indices[14] = 1;
//    indices[15] = 4; indices[16] = 1; indices[17] = 0;
//    // right side
//    indices[18] = 3; indices[19] = 2; indices[20] = 6;
//    indices[21] = 3; indices[22] = 6; indices[23] = 7;
//    // top
//    indices[24] = 1; indices[25] = 5; indices[26] = 6;
//    indices[27] = 1; indices[28] = 6; indices[29] = 2;
//    // bottom
//    indices[30] = 4; indices[31] = 0; indices[32] = 3;
//    indices[33] = 4; indices[34] = 3; indices[35] = 7;
//    IndexBuffer->Unlock();
//
//    // view (camera) transformation
//    D3DXVECTOR3 position(0.0f, 0.0f, -5.0f); // eye
//    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f); // at
//    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f); // up
//    D3DXMATRIX ViewTrans;
//    D3DXMatrixLookAtLH(&ViewTrans, &position, &target, &up);
//    D3DDevice->SetTransform(D3DTS_VIEW, &ViewTrans);
//
//    // setup the projection matrix
//    D3DXMATRIX ProjMat;
//    D3DXMatrixPerspectiveFovLH(&ProjMat, D3DX_PI * 0.5f,
//                               (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
//                               1.0f, 1000.0f);
//    D3DDevice->SetTransform(D3DTS_PROJECTION, &ProjMat);
//
//    // set the render mode as wireframe
//    D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
//}
//
//
//int
//UpdateAndRender(float TimeDelta)
//{
//    if(D3DDevice)
//    {
//        // setup the world transform (rotating with time)
//        D3DXMATRIX XRotation, YRotation;
//
//        // rotate on x-axis with time
//        static float XAngle = 0.0f;
//        D3DXMatrixRotationX(&XRotation, 3.14f / 4.0f * XAngle);
//        XAngle += TimeDelta;
//        if(XAngle >= 6.28f)
//            XAngle = 0.0f;
//
//        // incremement Y-rotation angle with time
//        static float YAngle = 0.0f;
//        D3DXMatrixRotationY(&YRotation, YAngle);
//        YAngle += TimeDelta;
//
//        // reset angle to zero when angle reaches 2*PI
//        if(YAngle >= 6.28f)
//            YAngle = 0.0f;
//
//        // calculate the final world transform
//        D3DXMATRIX WorldTrans = XRotation * YRotation;
//        D3DDevice->SetTransform(D3DTS_WORLD, &WorldTrans);
//
//        D3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
//        D3DDevice->BeginScene();
//
//        D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(struct vertex));
//        D3DDevice->SetIndices(IndexBuffer);
//        D3DDevice->SetFVF(MYFVF);
//
//        // draw a cube
//        D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
//
//        D3DDevice->EndScene();
//        D3DDevice->Present(0, 0, 0, 0);
//    }
//    return 1;
//}



// ------------------------------------
//  various meshes using D3DXCreateXXXX
// ------------------------------------
//
//void SetupPipeline()
//{
//    D3DXCreateTeapot(
//        D3DDevice,
//        &Objects[0],
//        0);
//
//    D3DXCreateBox(
//        D3DDevice,
//        2.0f, // width
//        2.0f, // height
//        2.0f, // depth
//        &Objects[1],
//        0);
//
//    // cylinder is built aligned on z-axis
//    D3DXCreateCylinder(
//        D3DDevice,
//        1.0f, // radius at negative z end
//        1.0f, // radius at positive z end
//        3.0f, // length of cylinder
//        10,   // slices
//        10,   // stacks
//        &Objects[2],
//        0);
//
//    D3DXCreateTorus(
//        D3DDevice,
//        1.0f, // inner radius
//        3.0f, // outer radius
//        10,   // sides
//        10,   // rings
//        &Objects[3],
//        0);
//
//    D3DXCreateSphere(
//        D3DDevice,
//        1.0f, // radius
//        10,   // slices
//        10,   // stacks
//        &Objects[4],
//        0);
//
//    // Eye objects at different world locations
//    D3DXMatrixTranslation(&WorldTransMats[0], 0.0f, 0.0f, 0.0f);
//    D3DXMatrixTranslation(&WorldTransMats[1], -5.0f, 0.0f, 5.0f);
//    D3DXMatrixTranslation(&WorldTransMats[2], 5.0f, 0.0f, 5.0f);
//    D3DXMatrixTranslation(&WorldTransMats[3], -5.0f, 0.0f, -5.0f);
//    D3DXMatrixTranslation(&WorldTransMats[4], 5.0f, 0.0f, -5.0f);
//
//    // Set the projection matrix.
//    D3DXMATRIX proj;
//    D3DXMatrixPerspectiveFovLH(
//        &proj,
//        D3DX_PI * 0.5f, // 90 - degree
//        (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
//        1.0f,
//        1000.0f);
//    D3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
//
//    // Switch to wireframe mode.
//    D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
//}
//
//
//int
//UpdateAndRender(float TimeDelta)
//{
//    if(D3DDevice)
//    {
//        // The camera will circle around the center of the scene.  We use the
//        // sin and cos functions to generate points on the circle, then scale them
//        // by 10 to further the radius.  In addition the camera will move up and down
//        // as it circles about the scene.
//        static float angle = (3.0f * D3DX_PI) / 2.0f;
//        static float cameraHeight = 0.0f;
//        static float cameraHeightDirection = 5.0f;
//
//        D3DXVECTOR3 Eye(cosf(angle) * 10.0f, cameraHeight, sinf(angle) * 10.0f);
//
//        // the camera is targetted at the origin of the world
//        D3DXVECTOR3 At(0.0f, 0.0f, 0.0f);
//
//        // the worlds up vector
//        D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
//
//        D3DXMATRIX ViewTrans;
//        D3DXMatrixLookAtLH(&ViewTrans, &Eye, &At, &up);
//        D3DDevice->SetTransform(D3DTS_VIEW, &ViewTrans);
//
//        // compute the Eye for the next frame
//        angle += TimeDelta;
//        if(angle >= 6.28f)
//            angle = 0.0f;
//
//        // compute the height of the camera for the next frame
//        cameraHeight += cameraHeightDirection * TimeDelta;
//        if(cameraHeight >= 10.0f)
//            cameraHeightDirection = -5.0f;
//
//        if(cameraHeight <= -10.0f)
//            cameraHeightDirection = 5.0f;
//
//        // Draw the Scene
//        D3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
//        D3DDevice->BeginScene();
//
//        for(int i = 0; i < 5; i++)
//        {
//            // Set the world matrix that positions the object.
//            D3DDevice->SetTransform(D3DTS_WORLD, &WorldTransMats[i]);
//
//            // Draw the object using the previously set world matrix.
//            Objects[i]->DrawSubset(0);
//        }
//
//        D3DDevice->EndScene();
//        D3DDevice->Present(0, 0, 0, 0);
//    }
//    return 1;
//}

// -------------------------------------
//  flat and smooth (gourad) shading
// -------------------------------------
//void SetupPipeline()
//{
//    D3DDevice->CreateVertexBuffer(3 * sizeof(colored_vertex), D3DUSAGE_WRITEONLY,
//                                  FVF_COLORED_VERTEX, D3DPOOL_MANAGED,
//                                  &VertexBuffer, 0);
//    colored_vertex *VBufferPtr;
//    VertexBuffer->Lock(0, 0, (void **) &VBufferPtr, 0);
//    // add the vertices of a triangle to the buffer
//    VBufferPtr[0] = {-1.0f, 0.0f, 2.0f, RED};
//    VBufferPtr[1] = {0.0f, 1.0f, 2.0f, GREEN};
//    VBufferPtr[2] = {1.0f, 0.0f, 2.0f, BLUE};
//    VertexBuffer->Unlock();
//
//    // setup a projection matrix
//    D3DXMATRIX ProjectionMat;
//    D3DXMatrixPerspectiveFovLH(&ProjectionMat, 0.5f * D3DX_PI,
//                               (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
//                               1.0f, 1000.0f);
//    D3DDevice->SetTransform(D3DTS_PROJECTION, &ProjectionMat);
//    D3DDevice->SetRenderState(D3DRS_LIGHTING, 0);
//
//    // set Gouraud shading mode
//    //D3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
//}
//
//
//void
//UpdateAndRender(float TimeDelta)
//{
//    if(D3DDevice)
//    {
//        D3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
//                         0xffffffff, 1.0f, 0);
//        D3DDevice->BeginScene();
//
//        D3DDevice->SetFVF(FVF_COLORED_VERTEX);
//        D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(colored_vertex));
//        // first triangle a bit to the left, flat shaded
//        D3DXMatrixTranslation(&WorldTransform, -1.25f, 0.0f, 0.0f);
//        D3DDevice->SetTransform(D3DTS_WORLD, &WorldTransform);
//        D3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
//        D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
//
//        // second triangle to the right, gourad (smooth) shaded
//        D3DXMatrixTranslation(&WorldTransform, 1.25f, 0.0f, 0.0f);
//        D3DDevice->SetTransform(D3DTS_WORLD, &WorldTransform);
//        D3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
//        D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
//
//        D3DDevice->EndScene();
//        D3DDevice->Present(0, 0, 0, 0);
//    }
//}




//
//// define a colored and normalized vertex format
//typedef struct
//{
//    float X, Y, Z;
//    float NX, NY, NZ;
//} color_normal_vertex;
//#define FVF_COLORED_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL)
//
//
//// buffers
//IDirect3DVertexBuffer9 *VertexBuffer;
//D3DXMATRIX WorldTransform;
//
//
//// materials
//D3DMATERIAL9
//InitializeMaterial(D3DXCOLOR AmbientLight, D3DXCOLOR DiffuseLight,
//                   D3DXCOLOR SpecularLight, D3DXCOLOR EmissiveLight, float SpecularPower)
//{
//    D3DMATERIAL9 Result;
//    ZeroMemory(&Result, sizeof(Result));
//    Result.Diffuse = DiffuseLight;
//    Result.Ambient = AmbientLight;
//    Result.Specular = SpecularLight;
//    Result.Emissive = EmissiveLight;
//    Result.Power = SpecularPower;
//}
//
//
//D3DMATERIAL9 WHITE_MATERIAL;
//D3DMATERIAL9 RED_MATERIAL;
//D3DMATERIAL9 GREEN_MATERIAL;
//D3DMATERIAL9 BLUE_MATERIAL;
//D3DMATERIAL9 YELLOW_MATERIAL;
//
//
//void ComputeNormalFromTriangle(D3DXVECTOR3 *P0,
//                               D3DXVECTOR3 *P1,
//                               D3DXVECTOR3 *P2,
//                               D3DXVECTOR3 *Out)
//{
//    D3DXVECTOR3 U = *P1 - *P0;
//    D3DXVECTOR3 V = *P2 - *P0;
//    D3DXVec3Cross(Out, &U, &V);
//    D3DXVec3Normalize(Out, Out);
//}
//
//
//D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3 *direction,
//                               D3DXCOLOR *color)
//{
//    D3DLIGHT9 light;
//    ZeroMemory(&light, sizeof(light));
//    light.Type = D3DLIGHT_DIRECTIONAL;
//    light.Ambient = *color * 0.4f;
//    light.Diffuse = *color;
//    light.Specular = *color * 0.6f;
//    light.Direction = *direction;
//    return light;
//}
//
//
//void SetupPipeline()
//{
//    // initialize materials
//    WHITE_MATERIAL = InitializeMaterial(WHITE, WHITE, WHITE, BLACK, 8.0f);
//    RED_MATERIAL = InitializeMaterial(RED, RED, RED, BLACK, 8.0f);
//    GREEN_MATERIAL = InitializeMaterial(GREEN, GREEN, GREEN, BLACK, 8.0f);
//    BLUE_MATERIAL = InitializeMaterial(BLUE, BLUE, BLUE, BLACK, 8.0f);
//    YELLOW_MATERIAL = InitializeMaterial(YELLOW, YELLOW, YELLOW, BLACK, 8.0f);
//
//    // make some lights
//    D3DDevice->CreateVertexBuffer(3 * sizeof(color_normal_vertex), D3DUSAGE_WRITEONLY,
//                                  FVF_COLORED_VERTEX, D3DPOOL_MANAGED,
//                                  &VertexBuffer, 0);
//    color_normal_vertex *VBufferPtr;
//    VertexBuffer->Lock(0, 0, (void **) &VBufferPtr, 0);
//    // add the vertices of a triangle to the buffer
//    VBufferPtr[0] = {-1.0f, 0.0f, 2.0f, RED};
//    VBufferPtr[1] = {0.0f, 1.0f, 2.0f, GREEN};
//    VBufferPtr[2] = {1.0f, 0.0f, 2.0f, BLUE};
//    VertexBuffer->Unlock();
//
//    // setup a projection matrix
//    D3DXMATRIX ProjectionMat;
//    D3DXMatrixPerspectiveFovLH(&ProjectionMat, 0.5f * D3DX_PI,
//                               (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
//                               1.0f, 1000.0f);
//    D3DDevice->SetTransform(D3DTS_PROJECTION, &ProjectionMat);
//    D3DDevice->SetRenderState(D3DRS_LIGHTING, 0);
//
//    // set Gouraud shading mode
//    //D3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
//}
//
//
//void
//UpdateAndRender(float TimeDelta)
//{
//    if(D3DDevice)
//    {
//        D3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
//                         0xffffffff, 1.0f, 0);
//        D3DDevice->BeginScene();
//
//        D3DDevice->SetFVF(FVF_COLORED_VERTEX);
//        D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(color_normal_vertex));
//        // first triangle a bit to the left, flat shaded
//        D3DXMatrixTranslation(&WorldTransform, -1.25f, 0.0f, 0.0f);
//        D3DDevice->SetTransform(D3DTS_WORLD, &WorldTransform);
//        D3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
//        D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
//
//        // second triangle to the right, gourad (smooth) shaded
//        D3DXMatrixTranslation(&WorldTransform, 1.25f, 0.0f, 0.0f);
//        D3DDevice->SetTransform(D3DTS_WORLD, &WorldTransform);
//        D3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
//        D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
//
//        D3DDevice->EndScene();
//        D3DDevice->Present(0, 0, 0, 0);
//    }
//}