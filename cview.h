#ifndef CVIEW_H
#define CVIEW_H

#include <QGraphicsView>

class CView : public QGraphicsView
{
public:
    CView();
    CView(QWidget* widget);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QPoint lastPos;
};

#endif // CVIEW_H
