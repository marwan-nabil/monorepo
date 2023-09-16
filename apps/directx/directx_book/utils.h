#pragma once

// materials
extern D3DMATERIAL9 WHITE_MTRL;
extern D3DMATERIAL9 RED_MTRL;
extern D3DMATERIAL9 GREEN_MTRL;
extern D3DMATERIAL9 BLUE_MTRL;
extern D3DMATERIAL9 YELLOW_MTRL;

// colors 
extern D3DXCOLOR      WHITE;
extern D3DXCOLOR      BLACK;
extern D3DXCOLOR        RED;
extern D3DXCOLOR      GREEN;
extern D3DXCOLOR       BLUE;
extern D3DXCOLOR     YELLOW;
extern D3DXCOLOR       CYAN;
extern D3DXCOLOR    MAGENTA;


// Lighting functionality
D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3 *direction, D3DXCOLOR *color);
D3DLIGHT9 InitPointLight(D3DXVECTOR3 *position, D3DXCOLOR *color);
D3DLIGHT9 InitializeSpotLight(D3DXVECTOR3 *position, D3DXVECTOR3 *direction, D3DXCOLOR *color);

// Materials functionality
D3DMATERIAL9 InitializeMaterial(D3DXCOLOR AmbientColor, D3DXCOLOR DiffuseColor, D3DXCOLOR SpecularColor,
                                D3DXCOLOR EmissiveColor, float EmisivePower);
