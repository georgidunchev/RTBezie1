#include "shader.h"
#include <main.h>
#include <raytracer.h>
#include <ray.h>
#include <settings.h>
#include <color.h>
#include <qmath.h>

CShader::CShader()
{
}

QRgb CShader::Shade(const CRay &ray, CIntersactionInfo &intersectionInfo)
{
    CColor cLamber;
    const float fLow = 0.05f;
    const float fHigh = 1.0f - fLow;
    bool bWireframe = false;
    if (!bWireframe ||
	    intersectionInfo.m_vBarCoordsLocal.x() < fLow || intersectionInfo.m_vBarCoordsLocal.x() > fHigh ||
	    intersectionInfo.m_vBarCoordsLocal.y() < fLow || intersectionInfo.m_vBarCoordsLocal.y() > fHigh ||
	    intersectionInfo.m_vBarCoordsLocal.z() < fLow || intersectionInfo.m_vBarCoordsLocal.z() > fHigh)
    {
	cLamber = ShadeLambert(ray, intersectionInfo);
	//cLamber += ShadeGloss(ray, intersectionInfo);
    }

    return cLamber.GetRGB();
}

CMesh &CShader::GetMesh()
{
	return GetRaytracer()->GetMesh();
}

CLightsScene &CShader::GetLightScene()
{
	return GetRaytracer()->GetLightScene();
}

CColor CShader::ShadeLambert(const CRay &ray, CIntersactionInfo &intersectionInfo)
{
    CColor cLamber;
	float fLightColor = 1.f;

	for (int i = 0; i < GetLightScene().GetLightsNumber(); ++i)
	{
		const QVector3D& vIntersection = intersectionInfo.m_vIntersectionPoint;
		const QVector3D vLIghtDirection = GetLightScene().GetLight(i).GetPosition() - vIntersection;
		float fCos = QVector3D::dotProduct(vLIghtDirection.normalized(), intersectionInfo.GetFacingNormal(ray));

		CColor colorForLight(1.0f, 1.0f, 1.0f);
		colorForLight = intersectionInfo.color;

		if (fCos > 0.0f)
		{
		    const float fLength = vLIghtDirection.length();
		    fCos /= fLength;

		    colorForLight *= fCos;
		    fLightColor *= fCos;
//				colorForLight *= intersectionInfo.color;
		    //		colorForLight *= fLightColor;
		    //		colorForLight *= 1.f/GetLightScene().GetLightsNumber();
		}
		else
		{
		     colorForLight *= 0.0f;
		     fLightColor *= 0.0f;
		}

//		intersectionInfo.color += colorForLight;
		cLamber = colorForLight;// * fLightColor;
	}
	return cLamber;
}

CColor CShader::ShadeGloss(const CRay &ray, CIntersactionInfo &intersectionInfo)
{
    CColor cLamber;
	float fLightColor = 1.f;

	for (int i = 0; i < GetLightScene().GetLightsNumber(); ++i)
	{
	    const  QVector3D vNormal = intersectionInfo.GetFacingNormal(ray);
		const QVector3D& vIntersection = intersectionInfo.m_vIntersectionPoint;
		const QVector3D vLIghtDirection = GetLightScene().GetLight(i).GetPosition() - vIntersection;
		const QVector3D vLightDirNorm = vLIghtDirection.normalized();
		float fCos = QVector3D::dotProduct(vLightDirNorm, vNormal);

		CColor colorForLight(1.0f, 1.0f, 1.0f);

		if (fCos > 0.0f)
		{
		    const float fLength = vLIghtDirection.length();
		    fCos /= fLength;

		    const QVector3D vFromLightNorm = -vLightDirNorm;

		    const QVector3D vReflection = CUtils::Reflect(vFromLightNorm, vNormal);
		    float fCosRefl = QVector3D::dotProduct(vReflection, vNormal);

		    if (fCosRefl > 0.0f)
		    {
			fCosRefl = qPow(fCosRefl, 8);
			colorForLight *= fCosRefl;
			fLightColor *= fCosRefl;
		    }
		    else
		    {
			colorForLight *= 0.0f;
			fLightColor *= 0.0f;
		    }
		}
		else
		{
		     colorForLight *= 0.0f;
		     fLightColor *= 0.0f;
		}

//		intersectionInfo.color += colorForLight;
		cLamber = colorForLight;// * fLightColor;
	}
	return cLamber;
}
