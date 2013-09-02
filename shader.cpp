#include "shader.h"
#include <main.h>
#include <raytracer.h>
#include <ray.h>
#include <settings.h>
#include <color.h>

CShader::CShader()
{
}

QRgb CShader::Shade(const CRay &ray, CIntersactionInfo &intersectionInfo)
{
    const CColor cLamber = ShadeLambert(ray, intersectionInfo);

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

		if (fCos > 0.0f)
		{
		    const float fLength = vLIghtDirection.length();
		    fCos /= fLength;

		    colorForLight *= fCos;
		    fLightColor *= fCos;
		    //		colorForLight *= intersectionInfo.color;
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
		const QVector3D& vIntersection = intersectionInfo.m_vIntersectionPoint;
		const QVector3D vLIghtDirection = GetLightScene().GetLight(i).GetPosition() - vIntersection;
		float fCos = QVector3D::dotProduct(vLIghtDirection.normalized(), intersectionInfo.GetFacingNormal(ray));

		CColor colorForLight(1.0f, 1.0f, 1.0f);

		if (fCos > 0.0f)
		{
		    const float fLength = vLIghtDirection.length();
		    fCos /= fLength;

		    colorForLight *= fCos;
		    fLightColor *= fCos;
		    //		colorForLight *= intersectionInfo.color;
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
