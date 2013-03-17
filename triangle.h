#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QVector3D>
#include <QVector>
#include <AABox.h>
#include <primitive.h>

class CRay;
class CIntersactionInfo;

class CTriangle :public CPrimitive
{
public:
    CTriangle() {}

    CTriangle( const QVector<QVector3D> & aVertecis, int v1, int v2, int v3);
    virtual bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo ) const;
    virtual bool Intersect(const QVector3D& vStart, const QVector3D& vEnd) const;
protected:

};


#endif // TRIANGLE_H
