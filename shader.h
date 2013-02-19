#ifndef SHADER_H
#define SHADER_H

#include <QRgb>

#include <cmesh.h>
#include <intersactioninfo.h>

class CShader
{
public:
    CShader();
    QRgb Shade(CIntersactionInfo& intersectionInfo);
private:
    CMesh &GetMesh();
    QRgb ShadeLambert(CIntersactionInfo& intersectionInfo);
};

#endif // SHADER_H
