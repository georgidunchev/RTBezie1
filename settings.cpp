#include "settings.h"

CSettings::CSettings(QObject *parent) :
    QObject(parent)
  , m_bINTERSECT_BEZIER(false)
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


