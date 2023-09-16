#include <Windows.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>


int
main(int argc, char **argv)
{
#define OUT_STRING_MAX 100
    char OutString[OUT_STRING_MAX];
#define NL ".\n"
#define VectorFmtStr " %f, %f, %f "
#define MatFmtStr "\n" \
                  "| %f, %f, %f |\n" \
                  "| %f, %f, %f |\n" \
                  "| %f, %f, %f |"
#define VecComponents(V) V.x, V.y, V.z
#define MatComponents(M) M(0, 0), M(0, 1), M(0, 2), \
                         M(1, 0), M(1, 1), M(1, 2), \
                         M(2, 0), M(2, 1), M(2, 2)

    D3DXVECTOR3 ResultPoint;
    D3DXMATRIX TransMat;
    D3DXMatrixTranslation(&TransMat, 10.0f, 10.0f, 0.0f);
    D3DXVECTOR3 Point1(1, 1, 1);
    D3DXVec3TransformCoord(&ResultPoint, &Point1, &TransMat);
    printf_s("ResultPoint is: " VectorFmtStr NL, VecComponents(ResultPoint));
    return(0);
}