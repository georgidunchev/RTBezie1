#include "light.h"

ILight::ILight()
{
}

void ILight::SetPosition(const QVector3D &vPosition)
{
    m_vPosition = vPosition;
}
