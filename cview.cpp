#include "cview.h"
#include "stdio.h"
#include <QMouseEvent>
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
}

void CView::mouseMoveEvent(QMouseEvent *event)
{
    float dx = event->x() - lastPos.x();
    float dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton)
    {
		GetRaytracer()->GetCamera().Rotate(dx, dy);
    }
	//else if (event->buttons() & Qt::LeftButton)
	//{
	//	GetRaytracer()->GetCamera().MoveTarget(dx, dy);
	//}
    lastPos = event->pos();
}

void CView::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    numDegrees /= 15;
//    zoom -=numDegrees/5.0;
//    if(zoom < 1.0)
//        zoom = 1.0;
//    if(zoom > 20.0)
//        zoom = 20.0;
//    xRot=yRot=0.0;
//    updateGL();
}
