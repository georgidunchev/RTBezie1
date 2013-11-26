#ifndef VERTEX_H
#define VERTEX_H

#include <vector3df.h>

class CVertex
{
public:
    CVertex();
    CVertex(CVector3DF vPos, int nId);
    const CVector3DF &GetPos() const;

    void Normal_AddNormal(const CVector3DF& vNormal);

    void Normal_Normalize();

    const CVector3DF &Normal_Get() const;
    bool IsSmoothGeometryOn();

private:
    void ConstructorInit();
    CVector3DF m_vPos;
    bool m_bSmoothGeometry;
    CVector3DF m_vNormal;
    int m_nId;
};

#endif // VERTEX_H
