#include "raytracer.h"
#include <cmesh.h>
#include <triangle.h>
#include <ray.h>
#include <camera.h>

RayTracer::RayTracer(QObject *parent)
    : QObject(parent)
    , m_Camera(this)
    , m_Mesh(NULL)
{
}


RayTracer::~RayTracer()
{
    delete m_pImage;
}

void RayTracer::SetCanvas(qreal fWidth, qreal fHeight)
{
    m_fWidth = fWidth;
    m_fHeight = fHeight;

    m_Mesh = new CMesh();
//    m_Mesh->Load("Triangle.obj");
    m_Mesh->Load("bunny_1k.obj");

    GetCamera().SetCameraResolution(m_fWidth, m_fHeight);
    GetCamera().BeginFrame();

    m_pImage = new QImage(m_fWidth, m_fHeight, QImage::Format_ARGB32);
}

void RayTracer::Render()
{
    for( uint j = 0; j < m_fHeight; ++j)
	for (uint i = 0; i < m_fWidth; ++i )
	{
	    if (i == 1 && j == 5)
	    {
		bool b = true;
		b = false;
	    }
	    QRgb value = qRgb(0,0,0);
	    if ( m_Mesh )
	    {
		CIntersactionInfo intersectionInfo;
//		GetCamera().getCameraToViewportRay(.getCameraToViewportRay(i, j);
		if ( m_Mesh->Intersect(GetCamera().GetScreenRay(i, j), intersectionInfo) )
		{
		    value = qRgb(189, 149, 39);
		}
	    }

	    m_pImage->setPixel(i, j, value);
	}
}

const QImage& RayTracer::GetImage() const
{
    return *m_pImage;
}

Camera &RayTracer::GetCamera()
{
    return m_Camera;
}
