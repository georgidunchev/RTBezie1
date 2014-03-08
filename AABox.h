#ifndef AABOX_H
#define AABOX_H

#include "vector3df.h"
#include "Utils.h"
#include "vertex.h"
#include "intersactioninfo.h"

class CRay;

class CAABox
{
public:
    explicit CAABox();
    explicit CAABox(const CVector3DF & vMinVertex,const CVector3DF & vMaxVertex);

    void AddPoint(const CVertex &vPoint);
    void AddPoint(const CVector3DF& vPoint);

    void Set(const CVector3DF & vMinVertex,const CVector3DF & vMaxVertex);
    void Reset();

    bool IsInside(const CVector3DF& vPoint) const;
    bool IsInside(const CVertex& vPoint) const;

    bool Intersect(const CRay& ray, bool bDebug = false) const;

	bool Intersect(const CRay& ray, CIntersactionInfo& io_IntersectionInfo, bool bDebug = false) const;
	
	bool Intersect_Test(const CRay& ray, CIntersactionInfo& io_IntersectionInfo, bool bDebug = false) const;

    const CVector3DF& GetMinVertex() const;
    const CVector3DF& GetMaxVertex() const;

    void Split(float fWhere, CAABox& LeftBBox, CAABox& RightBBox, CVector3DF::EDimiensions eSplitDimension) const;

    CVector3DF GetCentre() const;

signals:
    
public slots:

private:
    CVector3DF m_vMaxVertex;
    CVector3DF m_vMinVertex;

};

#endif // AABOX_H
