#ifndef WIDGET_H
#define WIDGET_H

#include <QGLWidget>
#include <QTime>
#include "sslworld.h"

#define _RENDER_INTERVAL 10

class Widget : public QGLWidget {

    Q_OBJECT
public:
    Widget(QWidget *parent = 0);
    ~Widget();
    QString getFPS();
    SSLWorld* ssl;
    int Current_robot,Current_team,cammode;
//public slots:

signals:
    void clicked();
protected:
    void paintGL ();
    void initializeGL ();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);    
    void wheelEvent(QWheelEvent *event);
private:
    int frames;
    QTime time;
    QString m_fps;
    QPoint lastPos;
};

#endif // WIDGET_H
