#include "vertex.h"
#include <Utils.h>
#include <settings.h>

CVertex::CVertex()
    :m_vPos(QVector3D(0.f,0.f,0.f))
{
    ConstructorInit();
}

CVertex::CVertex(QVector3D vPos)
    :m_vPos(vPos)
{
    ConstructorInit();
}

void CVertex::ConstructorInit()
{
    m_bSmoothGeometry = false;
    m_vNormal = QVector3D(0.f, 0.f, 0.f);
}

const QVector3D &CVertex::GetPos() const
{
    return m_vPos;
}

void CVertex::Normal_AddNormal(const QVector3D &vNormal)
{
    if (CUtils::Cos(m_vNormal, vNormal) < 0)
    {
	m_vNormal -= vNormal;
    }
    else
    {
	m_vNormal += vNormal;
    }
}

void CVertex::Normal_Normalize()
{
    m_vNormal.normalize();
}

const QVector3D &CVertex::Normal_Get() const
{
    return m_vNormal;
}

bool CVertex::IsSmoothGeometryOn()
{
    return m_bSmoothGeometry;
}


