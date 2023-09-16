// -------------------------------------
//  rotating pyramid with directional light source
// -------------------------------------

//IDirect3D9 *D3DInterface;
//IDirect3DDevice9 *D3DDevice;
//IDirect3DVertexBuffer9 *VertexBuffer;
//IDirect3DIndexBuffer9 *IndexBuffer;
//
//typedef struct
//{
//    float X, Y, Z;
//    float NX, NY, NZ;
//} vertex;
//#define MYFVF (D3DFVF_XYZ | D3DFVF_NORMAL)
//
//
//void SetupPipeline()
//{
//    // enable lighting
//    D3DDevice->SetRenderState(D3DRS_LIGHTING, 1);
//
//    // put a pyramid in the vertex buffer, each side has 3 vertices,
//    // we'll not share vertices because each face has a different normal
//    D3DDevice->CreateVertexBuffer(12 * sizeof(vertex), D3DUSAGE_WRITEONLY,
//                                  MYFVF, D3DPOOL_MANAGED, &VertexBuffer, 0);
//    vertex *VPtr;
//    VertexBuffer->Lock(0, 0, (void **) &VPtr, 0);
//    // front face (no vertices sharing)
//    VPtr[0] = {-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f};
//    VPtr[1] = {0.0f, 1.0f, 0.0f, 0.0f, 0.707f, -0.707f};
//    VPtr[2] = {1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f};
//    // left face    
//    VPtr[3] = {-1.0f, 0.0f, 1.0f, -0.707f, 0.707f, 0.0f};
//    VPtr[4] = {0.0f, 1.0f, 0.0f, -0.707f, 0.707f, 0.0f};
//    VPtr[5] = {-1.0f, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f};
//    // right face
//    VPtr[6] = {1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0};
//    VPtr[7] = {0.0f, 1.0f, 0.0f, 0.707f, 0.707f, 0.0f};
//    VPtr[8] = {1.0f, 0.0f, 1.0f, 0.707f, 0.707f, 0.0f};
//    // back face
//    VPtr[9] = {1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f};
//    VPtr[10] = {0.0f, 1.0f, 0.0f, 0.0f, 0.707f, 0.707f};
//    VPtr[11] = {-1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f};
//    VertexBuffer->Unlock();
//
//    // create the pyramid material
//    D3DMATERIAL9 PyramidMaterial;
//    PyramidMaterial.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//    PyramidMaterial.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//    PyramidMaterial.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//    PyramidMaterial.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
//    PyramidMaterial.Power = 5.0f;
//    D3DDevice->SetMaterial(&PyramidMaterial);
//
//    // create a directional light source
//    D3DLIGHT9 DirectionalLight;
//    ZeroMemory(&DirectionalLight, sizeof(D3DLIGHT9));
//    DirectionalLight.Type = D3DLIGHT_DIRECTIONAL;
//    DirectionalLight.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//    DirectionalLight.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) * 0.3f;
//    DirectionalLight.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) * 0.6f;
//    DirectionalLight.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
//
//    D3DDevice->SetLight(0, &DirectionalLight);
//    D3DDevice->LightEnable(0, 1);
//
//    // re-normalize vertex normals after transformations
//    D3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, 1);
//    // enable specular lighting
//    D3DDevice->SetRenderState(D3DRS_SPECULARENABLE, 1);
//
//    // view (camera) matrix
//    D3DXVECTOR3 position(0.0f, 0.0f, -5.0f); // eye
//    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f); // at
//    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f); // up
//    D3DXMATRIX ViewMat;
//    D3DXMatrixLookAtLH(&ViewMat, &position, &target, &up);
//    D3DDevice->SetTransform(D3DTS_VIEW, &ViewMat);
//
//    // projection matrix
//    D3DXMATRIX ProjectionMat;
//    D3DXMatrixPerspectiveFovLH(&ProjectionMat, 0.5f * D3DX_PI,
//                               (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
//                               1.0f, 1000.0f);
//    D3DDevice->SetTransform(D3DTS_PROJECTION, &ProjectionMat);
//}
//
//
//int
//UpdateAndRender(float TimeDelta)
//{
//    if(D3DDevice)
//    {
//        // 
//        // Rotate the pyramid.
//        //
//        D3DXMATRIX YRotMat;
//        static float YAngle = 0.0f;
//        D3DXMatrixRotationY(&YRotMat, YAngle);
//        YAngle += TimeDelta;
//
//        if(YAngle >= 6.28f)
//            YAngle = 0.0f;
//
//        D3DDevice->SetTransform(D3DTS_WORLD, &YRotMat);
//
//        // clear the screen and draw the pyramid
//        D3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
//        D3DDevice->BeginScene();
//
//        D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(vertex));
//        D3DDevice->SetFVF(MYFVF);
//
//        D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);
//
//        D3DDevice->EndScene();
//        D3DDevice->Present(0, 0, 0, 0);
//    }
//    return 1;
//}





//// -------------------------------------
////  directional light example with 4 mesh objects
//// -------------------------------------
//// 4 mesh objects with 4 materials and 4 world transforms
//ID3DXMesh *Objects[4] = {0, 0, 0, 0};
//D3DXMATRIX  WorldTransforms[4];
//D3DMATERIAL9 Materials[4];
//
//
//// the vertex format
//typedef struct
//{
//    float X, Y, Z;
//    float NX, NY, NZ;
//} vertex;
//#define MYFVF (D3DFVF_XYZ | D3DFVF_NORMAL)
//
//void SetupPipeline()
//{
//    //
//    // Create 4 mesh objects
//    //
//    D3DXCreateTeapot(D3DDevice, &Objects[0], 0);
//    D3DXCreateSphere(D3DDevice, 1.0f, 20, 20, &Objects[1], 0);
//    D3DXCreateTorus(D3DDevice, 0.5f, 1.0f, 20, 20, &Objects[2], 0);
//    D3DXCreateCylinder(D3DDevice, 0.5f, 0.5f, 2.0f, 20, 20, &Objects[3], 0);
//
//    //
//    // world transforms for these objects
//    //
//    D3DXMatrixTranslation(&WorldTransforms[0], 0.0f, 2.0f, 0.0f);
//    D3DXMatrixTranslation(&WorldTransforms[1], 0.0f, -2.0f, 0.0f);
//    D3DXMatrixTranslation(&WorldTransforms[2], -3.0f, 0.0f, 0.0f);
//    D3DXMatrixTranslation(&WorldTransforms[3], 3.0f, 0.0f, 0.0f);
//
//    //
//    // assign object materials
//    //
//    Materials[0] = RED_MTRL;
//    Materials[1] = BLUE_MTRL;
//    Materials[2] = GREEN_MTRL;
//    Materials[3] = YELLOW_MTRL;
//
//    //
//    // Setup a directional light
//    //
//    D3DXVECTOR3 Direction = {1.0f, -0.0f, 0.25f};
//    D3DXCOLOR c = WHITE;
//    D3DLIGHT9 DirectionalLight = InitDirectionalLight(&Direction, &c);
//
//    //
//    // Set and Enable the light.
//    //
//    D3DDevice->SetLight(0, &DirectionalLight);
//    D3DDevice->LightEnable(0, 1);
//    D3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
//    D3DDevice->SetRenderState(D3DRS_SPECULARENABLE, false);
//
//    //
//    // Set the projection matrix.
//    //
//    D3DXMATRIX ProjMat;
//    D3DXMatrixPerspectiveFovLH(&ProjMat,
//                               D3DX_PI * 0.25f, // 45 - degree
//                               (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
//                               1.0f, 1000.0f);
//    D3DDevice->SetTransform(D3DTS_PROJECTION, &ProjMat);
//}
//
//
//int
//UpdateAndRender(float TimeDelta)
//{
//    if(D3DDevice)
//    {
//        // 
//        // Update the scene: update camera position.
//        //
//        static float CameraAngle = (3.0f * D3DX_PI) / 2.0f;
//        static float CameraElevation = 5.0f;
//
//        // check keyboard inputs and change camera positions with it
//        if(GetAsyncKeyState(VK_LEFT) & 0x8000f)
//            CameraAngle -= 0.5f * TimeDelta;
//
//        if(GetAsyncKeyState(VK_RIGHT) & 0x8000f)
//            CameraAngle += 0.5f * TimeDelta;
//
//        if(GetAsyncKeyState(VK_UP) & 0x8000f)
//            CameraElevation += 5.0f * TimeDelta;
//
//        if(GetAsyncKeyState(VK_DOWN) & 0x8000f)
//            CameraElevation -= 5.0f * TimeDelta;
//
//
//        // create a view transform
//        D3DXVECTOR3 CameraPosition = {cosf(CameraAngle) * 7.0f, CameraElevation, sinf(CameraAngle) * 7.0f};
//        D3DXVECTOR3 target = {0.0f, 0.0f, 0.0f};
//        D3DXVECTOR3 up = {0.0f, 1.0f, 0.0f};
//        D3DXMATRIX ViewTrans;
//        D3DXMatrixLookAtLH(&ViewTrans, &CameraPosition, &target, &up);
//        D3DDevice->SetTransform(D3DTS_VIEW, &ViewTrans);
//
//        //
//        // Draw the scene
//        //
//        D3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
//        D3DDevice->BeginScene();
//
//        for(int i = 0; i < 4; i++)
//        {
//            D3DDevice->SetMaterial(&Materials[i]);
//            D3DDevice->SetTransform(D3DTS_WORLD, &WorldTransforms[i]);
//            Objects[i]->DrawSubset(0);
//        }
//
//        D3DDevice->EndScene();
//        D3DDevice->Present(0, 0, 0, 0);
//    }
//    return 1;
//}
//
//
//void
//CleanUp()
//{
//    D3DDevice->Release();
//    D3DInterface->Release();
//    VertexBuffer->Release();
//    for(int i = 0;
//        i < ARRAYSIZE(Objects);
//        i++)
//    {
//        Objects[i]->Release();
//    }
//}