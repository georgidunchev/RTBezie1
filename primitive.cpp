#include "primitive.h"
#include <intersactioninfo.h>
#include <ray.h>
#include <QVector3D>
#include <QVector>
#include <qmath.h>
#include <QDebug>
#include <Utils.h>

CPrimitive::CPrimitive(const QVector<QVector3D> &aVertecis)
    :m_aVertecis(aVertecis)
    , m_bHasBoundingBox(false)
{
}

const QVector<int> &CPrimitive::Vertices() const
{
    return m_aVertIndices;
}

const QVector3D &CPrimitive::AB() const
{
    return m_vAB;
}

const QVector3D &CPrimitive::AC() const
{
    return m_vAC;
}

const QVector3D &CPrimitive::Normal() const
{
    return m_vNormal;
}

const QVector3D &CPrimitive::A() const
{
    return m_aVertecis[m_aVertIndices[0]];
}

const QVector3D &CPrimitive::B() const
{
    return m_aVertecis[m_aVertIndices[1]];
}

const QVector3D &CPrimitive::C() const
{
    return m_aVertecis[m_aVertIndices[2]];
}

void CPrimitive::MakeBoundingBox()
{
    for(int i = 0; i < m_aVertIndices.size(); ++i)
    {
	m_BoundingBox.AddPoint(m_aVertecis[m_aVertIndices[i]]);
//	qDebug()<<m_aVertecis[m_aVertIndices[i]];
    }
//    qDebug()<<"min"<<m_BoundingBox.GetMinVertex()<<"max"<<m_BoundingBox.GetMaxVertex();
    m_bHasBoundingBox = true;
}

const CAABox& CPrimitive::GetBoundingBox()
{
    return m_BoundingBox;
}
