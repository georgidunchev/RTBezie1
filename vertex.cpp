#include "vertex.h"
#include "Utils.h"
#include "settings.h"

CVertex::CVertex()
    :m_vPos(0.f,  0.f, 0.f)
    ,m_nId(-1)
{
    ConstructorInit();
}

CVertex::CVertex(CVector3DF vPos, int nId)
    :m_vPos(vPos)
    ,m_nId(nId)
{
    ConstructorInit();
}

void CVertex::ConstructorInit()
{
    m_bSmoothGeometry = false;
    m_vNormal = CVector3DF(0.f, 0.f, 0.f);
}

const CVector3DF &CVertex::GetPos() const
{
    return m_vPos;
}

void CVertex::Normal_AddNormal(const CVector3DF &vNormal)
{
    if (m_vNormal.Cos(vNormal) < 0)
    {
	m_vNormal = m_vNormal - vNormal;
    }
    else
    {
	m_vNormal = m_vNormal + vNormal;
    }
}

void CVertex::Normal_Normalize()
{
    m_vNormal.Normalize();
}

const CVector3DF &CVertex::Normal_Get() const
{
    return m_vNormal;
}

bool CVertex::IsSmoothGeometryOn()
{
    return m_bSmoothGeometry;
}


