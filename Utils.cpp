#include "Utils.h"

CUtils::CUtils()
{
}

float CUtils::GetDimension(const QVector3D& pVector, EDimiensions eDimension)
{
    switch(eDimension)
    {
    case e_Dimension_X:
    {
	return pVector.x();
    }
    case e_Dimension_Y:
    {
	return pVector.y();
    }
    case e_Dimension_Z:
    {
	return pVector.z();
    }
    }
    return 0.f;
}
