#include "settings.h"

CSettings::CSettings(QObject *parent) :
    QObject(parent)
  , m_bINTERSECT_BEZIER(false)
  , m_bNormalSmoothing(false)

{
}

void CSettings::SetIntersectBezier(bool bIntersect)
{
    m_bINTERSECT_BEZIER = bIntersect;
}

bool CSettings::GetIntersectBezier()
{
    return m_bINTERSECT_BEZIER;
}

void CSettings::SetNormalSmoothing(bool bSmooth)
{
    m_bNormalSmoothing = bSmooth;
}

bool CSettings::GetNormalSmoothing()
{
    return m_bNormalSmoothing;
}

void CSettings::SetNofSubdivisions(uint nValue)
{
    m_nNofSubdivisions = nValue;
}

uint CSettings::GetNofSubdivisions()
{
    return m_nNofSubdivisions;
}


