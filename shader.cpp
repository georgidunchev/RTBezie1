#include "shader.h"
#include <main.h>
#include <raytracer.h>


CShader::CShader()
{
}

QRgb CShader::Shade(CIntersactionInfo &intersectionInfo)
{
    return ShadeLambert(intersectionInfo);
}

CMesh &CShader::GetMesh()
{
    return GetRaytracer()->GetMesh();
}

QRgb CShader::ShadeLambert(CIntersactionInfo &intersectionInfo)
{
    return qRgb(189, 149, 39);
}
