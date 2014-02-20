#include "shader.h"
#include <main.h>
#include <raytracer.h>
#include <ray.h>
#include <settings.h>
#include <color.h>
#include <qmath.h>
#include <vector3df.h>

CShader::CShader()
{
}

QRgb CShader::Shade(const CRay &ray, CIntersactionInfo &intersectionInfo)
{
    CColor cLamber(0.0f, 1.0f, 0.5f);
    const float fLow = 0.05f;
    const float fHigh = 1.0f - fLow;
    if (GetSettings()->m_bWireframe)
    {
        if (intersectionInfo.m_vBarCoordsLocal.X() < fLow || intersectionInfo.m_vBarCoordsLocal.X() > fHigh ||
            intersectionInfo.m_vBarCoordsLocal.Y() < fLow || intersectionInfo.m_vBarCoordsLocal.Y() > fHigh ||
            intersectionInfo.m_vBarCoordsLocal.Z() < fLow || intersectionInfo.m_vBarCoordsLocal.Z() > fHigh)
        {

        }
        else
        {
            cLamber = ShadeLambert(ray, intersectionInfo);
            //cLamber += ShadeGloss(ray, intersectionInfo);
        }
    }
    else
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
		const CVector3DF& vIntersection = intersectionInfo.m_vIntersectionPoint;
		const CVector3DF vLIghtDirection = GetLightScene().GetLight(i).GetPosition() - vIntersection;
		float fCos = vLIghtDirection.Normalized().Dot(intersectionInfo.GetFacingNormal(ray));

		CColor colorForLight(1.0f, 1.0f, 1.0f);
		colorForLight = intersectionInfo.color;

		if (fCos > 0.0f)
		{
		    const float fLength = vLIghtDirection.Length();
		    fCos /= fLength;

		    colorForLight *= fCos;
		    fLightColor *= fCos;
//				colorForLight *= intersectionInfo.color;
		    //		colorForLight *= fLightColor;
		    //		colorForLight *= 1.f/GetLightScene().GetLightsNumber();
		}
		else
		{
             colorForLight = 0.0f;
             fLightColor = 0.0f;
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
	    const  CVector3DF vNormal = intersectionInfo.GetFacingNormal(ray);
		const CVector3DF& vIntersection = intersectionInfo.m_vIntersectionPoint;
		const CVector3DF vLIghtDirection = GetLightScene().GetLight(i).GetPosition() - vIntersection;
		const CVector3DF vLightDirNorm = vLIghtDirection.Normalized();
		float fCos = vLightDirNorm.Dot(vNormal);

		CColor colorForLight(1.0f, 1.0f, 1.0f);

		if (fCos > 0.0f)
		{
		    const float fLength = vLIghtDirection.Length();
		    fCos /= fLength;

		    const CVector3DF vFromLightNorm = -vLightDirNorm;

		    const CVector3DF vReflection = CUtils::Reflect(vFromLightNorm, vNormal);
		    float fCosRefl = vReflection.Dot(vNormal);

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
