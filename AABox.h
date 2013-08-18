#ifndef AABOX_H
#define AABOX_H

#include <QVector3D>
#include <Utils.h>
#include <vertex.h>
#include <intersactioninfo.h>

class CRay;

class CAABox
{
public:
    explicit CAABox();
    explicit CAABox(const QVector3D & vMinVertex,const QVector3D & vMaxVertex);

    void AddPoint(const CVertex &vPoint);
    void AddPoint(const QVector3D& vPoint);

    void Set(const QVector3D & vMinVertex,const QVector3D & vMaxVertex);
    void Reset();

    bool IsInside(const QVector3D& vPoint) const;
    bool IsInside(const CVertex& vPoint) const;

    bool Intersect(const CRay& ray, bool bDebug = false) const;

	bool Intersect(const CRay& ray, CIntersactionInfo& io_IntersectionInfo, bool bDebug = false) const;
	
	bool Intersect_Test(const CRay& ray, CIntersactionInfo& io_IntersectionInfo, bool bDebug = false) const;

    const QVector3D& GetMinVertex() const;
    const QVector3D& GetMaxVertex() const;

    void Split(float fWhere, CAABox& LeftBBox, CAABox& RightBBox, EDimiensions eSplitDimension) const;

signals:
    
public slots:

private:
    QVector3D m_vMaxVertex;
    QVector3D m_vMinVertex;

};

#endif // AABOX_H
