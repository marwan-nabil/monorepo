#pragma once

struct cube {
    IDirect3DDevice9 *Device;
    IDirect3DVertexBuffer9 *VBuffer;
    IDirect3DIndexBuffer9 *IBuffer;
};

cube MakeCube(IDirect3DDevice9 *Dev);
int DrawCube(cube *Cube, D3DXMATRIX *WorldMat, D3DMATERIAL9 *Material, IDirect3DTexture9 *Texture);