#include <d3dx9.h>
#include "utils.h"

//
// some colors
//
D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));

//
// some materials
//
D3DMATERIAL9 WHITE_MTRL = InitializeMaterial(WHITE, WHITE, WHITE, BLACK, 2.0f);
D3DMATERIAL9 RED_MTRL = InitializeMaterial(RED, RED, RED, BLACK, 2.0f);
D3DMATERIAL9 GREEN_MTRL = InitializeMaterial(GREEN, GREEN, GREEN, BLACK, 2.0f);
D3DMATERIAL9 BLUE_MTRL = InitializeMaterial(BLUE, BLUE, BLUE, BLACK, 2.0f);
D3DMATERIAL9 YELLOW_MTRL = InitializeMaterial(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);


// make a colored material
D3DMATERIAL9 InitializeMaterial(D3DXCOLOR AmbientColor, D3DXCOLOR DiffuseColor, D3DXCOLOR SpecularColor, 
                                D3DXCOLOR EmissiveColor, float EmisivePower)
{
    D3DMATERIAL9 Result;
    Result.Ambient = AmbientColor;
    Result.Diffuse = DiffuseColor;
    Result.Specular = SpecularColor;
    Result.Emissive = EmissiveColor;
    Result.Power = EmisivePower;
    return Result;
}


// make a spotlight
D3DLIGHT9 InitializeSpotLight(D3DXVECTOR3 *Position, D3DXVECTOR3 *Direction, D3DXCOLOR *Color)
{
    D3DLIGHT9 Result;
    ZeroMemory(&Result, sizeof(Result));

    Result.Type = D3DLIGHT_SPOT;
    Result.Ambient = *Color * 0.0f;
    Result.Diffuse = *Color;
    Result.Specular = *Color * 0.6f;
    Result.Position = *Position;
    Result.Direction = *Direction;
    Result.Range = 1000.0f;
    Result.Falloff = 1.0f;
    Result.Attenuation0 = 1.0f;
    Result.Attenuation1 = 0.0f;
    Result.Attenuation2 = 0.0f;
    Result.Theta = 0.4f;
    Result.Phi = 0.9f;

    return Result;
}


// make a directional light
D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3 *Direction, D3DXCOLOR *Color)
{
    D3DLIGHT9 Result;
    ZeroMemory(&Result, sizeof(Result));

    Result.Type = D3DLIGHT_DIRECTIONAL;
    Result.Ambient = *Color * 0.6f;
    Result.Diffuse = *Color;
    Result.Specular = *Color * 0.6f;
    Result.Direction = *Direction;

    return Result;
}


// make a point light
D3DLIGHT9 InitPointLight(D3DXVECTOR3 *Position, D3DXCOLOR *Color)
{
    D3DLIGHT9 Result;
    ZeroMemory(&Result, sizeof(Result));

    Result.Type = D3DLIGHT_POINT;
    Result.Ambient = *Color * 0.6f;
    Result.Diffuse = *Color;
    Result.Specular = *Color * 0.6f;
    Result.Position = *Position;
    Result.Range = 1000.0f;
    Result.Falloff = 1.0f;
    Result.Attenuation0 = 1.0f;
    Result.Attenuation1 = 0.0f;
    Result.Attenuation2 = 0.0f;

    return Result;
}