#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QTime>
#include <QMenu>
#include "sslworld.h"

#include "configwidget.h"

#define _RENDER_INTERVAL 10

class GLWidget : public QGLWidget {

    Q_OBJECT
public:
    GLWidget(QWidget *parent,ConfigWidget* _cfg);
    ~GLWidget();
    QString getFPS();
    ConfigWidget* cfg;
    SSLWorld* ssl;
    QMenu* robpopup,*ballpopup;
    QAction* moveRobotAct;
    QAction* selectRobotAct;
    QAction* resetRobotAct;
    QAction* moveBallAct;
    int Current_robot,Current_team,cammode;
    void update3DCursor(int mouse_x,int mouse_y);
public slots:
    void moveRobot();
    void selectRobot();
    void resetRobot();
    void moveBall();
signals:
    void clicked();
    void selectedRobot();
    void closeSignal(bool);
protected:
    void paintGL ();
    void initializeGL ();    

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);    
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);
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
