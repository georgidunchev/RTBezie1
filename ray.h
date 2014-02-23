#ifndef CRAY_H
#define CRAY_H

#include "vector3df.h"

class CRay
{
public:
    CRay(CVector3DF v3dOrigin, CVector3DF v3dDirection);
    ~CRay(){}

    CVector3DF StartPoint() const;

    CVector3DF Direction() const;

    CVector3DF GetPointAtDistance(float fDistance) const;
private:
    CVector3DF m_v3dOrigin;
    //normalized
    CVector3DF m_v3dDirection;
};

#endif // CRAY_H
