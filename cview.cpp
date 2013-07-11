#include "cview.h"
#include "stdio.h"
#include <QMouseEvent>
#include <qdebug.h>
#include "main.h"
#include "raytracer.h"

CView::CView()
{
}

CView::CView(QWidget *widget)
    : QLabel(widget)
{

}

void CView::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
	if (event->buttons() & Qt::MiddleButton)
	{
		GetRaytracer()->RenderPixel(lastPos.x(), lastPos.y(), true);
	}
}

void CView::mouseMoveEvent(QMouseEvent *event)
{
    float dx = event->x() - lastPos.x();
    float dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton)
    {
		GetRaytracer()->GetCamera().Rotate(dx, dy);
    }
	else if (event->buttons() & Qt::RightButton)
	{
		GetRaytracer()->GetCamera().MoveTarget(dx, dy);
	}
    lastPos = event->pos();
}

void CView::wheelEvent(QWheelEvent *event)
{
    float fZoom = static_cast<float>(event->delta());
	//qDebug() << "ZOOM " << fZoom;
	GetRaytracer()->GetCamera().Zoom(fZoom);
}
