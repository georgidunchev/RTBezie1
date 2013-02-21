#ifndef SHADER_H
#define SHADER_H

#include <QRgb>

#include <cmesh.h>
#include <intersactioninfo.h>
#include <lightsscene.h>

class CShader
{
public:
    CShader();
    QRgb Shade(const CRay& ray, CIntersactionInfo& intersectionInfo);
private:
    CMesh &GetMesh();
    CLightsScene &GetLightScene();
    QRgb ShadeLambert(const CRay& ray, CIntersactionInfo& intersectionInfo);
};

#endif // SHADER_H
