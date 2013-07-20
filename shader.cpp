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
    return ShadeLambert(ray, intersectionInfo);
}

CMesh &CShader::GetMesh()
{
    return GetRaytracer()->GetMesh();
}

CLightsScene &CShader::GetLightScene()
{
    return GetRaytracer()->GetLightScene();
}

QRgb CShader::ShadeLambert(const CRay &ray, CIntersactionInfo &intersectionInfo)
{
    float fLightColor = 1.f;

    for (int i = 0; i < GetLightScene().GetLightsNumber(); ++i)
    {
	QVector3D vIntersection = intersectionInfo.m_vIntersectionPoint;
	QVector3D vLIghtDirection = GetLightScene().GetLight(i).GetPosition() - vIntersection;
	float fCos = QVector3D::dotProduct(vLIghtDirection.normalized(), intersectionInfo.m_vNormal);

	CColor colorForLight(0.5f, 0.5f, 0.5f);
	colorForLight *= qAbs(fCos);
	colorForLight *= fLightColor;
	colorForLight *= 1.f/GetLightScene().GetLightsNumber();
	intersectionInfo.color+= colorForLight;

	//add specular for each affecting light
//	if(false)
//	{

//	    QVector3D vLIghtDirection = vIntersection - GetLightScene().GetLight(i);
//	    CRay LightRay(GetLightScene().GetLight(i), vLIghtDirection);
//	    f32 fDistToLight = vLIghtDirection.length();

//	    CIntersactionInfo &LightIntersectionInfo;
//	    if (GetMesh().Intersect(LightRay, LightIntersectionInfo))
//	    {
//		//if the first intersection is the intersection point
//		if (LightIntersectionInfo.m_fDistance > fDistToLight - k_fSMALL)
//		{

//		}
//	    }
//	}
    }
    return intersectionInfo.color.GetRGB();
}
