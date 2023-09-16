#include <d3dx9.h>
#include "window.h"
#include "cube.h"


cube MakeCube(IDirect3DDevice9 *Dev)
{
	cube Result;
	// save a ptr to the device
	Result.Device = Dev;

	Result.Device->CreateVertexBuffer(24 * sizeof(vertex),
									  D3DUSAGE_WRITEONLY,
									  MYFVF,
									  D3DPOOL_MANAGED,
									  &Result.VBuffer,
									  0);

	vertex *v;
	Result.VBuffer->Lock(0, 0, (void **) &v, 0);

	// build box

	// fill in the front face vertex data
	v[0] = {-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
	v[1] = {-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f};
	v[2] = {1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f};
	v[3] = {1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f};

	// fill in the back face vertex data
	v[4] = {-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
	v[5] = {1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f};
	v[6] = {1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};
	v[7] = {-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f};

	// fill in the top face vertex data
	v[8] = {-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
	v[9] = {-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
	v[10] = {1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f};
	v[11] = {1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

	// fill in the bottom face vertex data
	v[12] = {-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f};
	v[13] = {1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f};
	v[14] = {1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f};
	v[15] = {-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f};

	// fill in the left face vertex data
	v[16] = {-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	v[17] = {-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	v[18] = {-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f};
	v[19] = {-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

	// fill in the right face vertex data
	v[20] = {1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	v[21] = {1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	v[22] = {1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f};
	v[23] = {1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

	Result.VBuffer->Unlock();

	Result.Device->CreateIndexBuffer(36 * sizeof(WORD),
									 D3DUSAGE_WRITEONLY,
									 D3DFMT_INDEX16,
									 D3DPOOL_MANAGED,
									 &Result.IBuffer,
									 0);

	WORD *i = 0;
	Result.IBuffer->Lock(0, 0, (void **) &i, 0);

	// fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	Result.IBuffer->Unlock();
	return Result;
}


int DrawCube(cube *Cube, D3DXMATRIX *WorldMat, D3DMATERIAL9 *Material, IDirect3DTexture9 *Texture)
{
	if(WorldMat)
	{
		Cube->Device->SetTransform(D3DTS_WORLD, WorldMat);
	}

	if(Material)
	{
		Cube->Device->SetMaterial(Material);
	}

	if(Texture)
	{
		Cube->Device->SetTexture(0, Texture);
	}

	Cube->Device->SetStreamSource(0, Cube->VBuffer, 0, sizeof(vertex));
	Cube->Device->SetIndices(Cube->IBuffer);
	Cube->Device->SetFVF(MYFVF);
	Cube->Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);
	return 1;
}

