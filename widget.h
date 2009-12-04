#ifndef WIDGET_H
#define WIDGET_H

#include <QGLWidget>
#include <QTime>
#include <QMenu>
#include "sslworld.h"

#define _RENDER_INTERVAL 10

class Widget : public QGLWidget {

    Q_OBJECT
public:
    Widget(QWidget *parent = 0);
    ~Widget();
    QString getFPS();
    SSLWorld* ssl;
    QMenu* robpopup,*ballpopup;
    QAction* moveRobotAct;
    QAction* selectRobotAct;
    QAction* resetRobotAct;
    QAction* moveBallAct;
    int Current_robot,Current_team,cammode;
    void update3DCursur(int mouse_x,int mouse_y);
public slots:
    void moveRobot();
    void selectRobot();
    void resetRobot();
    void moveBall();
signals:
    void clicked();
    void selectedRobot();
protected:
    void paintGL ();
    void initializeGL ();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);    
    void wheelEvent(QWheelEvent *event);
private:
    int state;
    int moving_robot_id,clicked_robot;
    int frames;
    bool first_time;
    QTime time,rendertimer;
    QString m_fps;
    QPoint lastPos;
};

#endif // WIDGET_H
