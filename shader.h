#ifndef SHADER_H
#define SHADER_H

#include <QRgb>

#include "cmesh.h"
#include "intersactioninfo.h"
#include "lightsscene.h"

class CShader
{
public:
    CShader();
    QRgb Shade(const CRay& ray, CIntersactionInfo& intersectionInfo);
private:
    CMesh &GetMesh();
    CLightsScene &GetLightScene();
    CColor ShadeLambert(const CRay& ray, CIntersactionInfo& intersectionInfo);
    CColor ShadeGloss(const CRay& ray, CIntersactionInfo& intersectionInfo);
    CVector3DF vLightPos;
};

#endif // SHADER_H
