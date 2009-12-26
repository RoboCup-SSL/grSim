#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QGraphicsView>
#include <QTime>
#include <QMenu>
#include "sslworld.h"

#include "configwidget.h"

class GLWidgetGraphicsView;
class GLWidget : public QGLWidget {

    Q_OBJECT
public:
    GLWidget(QWidget *parent,ConfigWidget* _cfg);
    ~GLWidget();
    float getFPS();
    ConfigWidget* cfg;
    SSLWorld* ssl;
    RobotsFomation* forms[4];
    QMenu* robpopup,*ballpopup,*mainpopup;
    QMenu *blueRobotsMenu,*yellowRobotsMenu;
    QAction* moveRobotAct;
    QAction* selectRobotAct;
    QAction* resetRobotAct;
    QAction* moveBallAct;        
    QAction* lockToBallAct;
    QAction* onOffRobotAct;
    QAction* lockToRobotAct;
    QAction* moveBallHereAct;
    QAction* moveRobotHereAct;
    QAction* changeCamModeAct;
    QMenu *cameraMenu;
    int Current_robot,Current_team,cammode;
    int lockedIndex;
    bool ctrl,alt;
    bool fullScreen;
    void update3DCursor(int mouse_x,int mouse_y);
    void putBall(float x,float y);
    void reform(int team,const QString& act);
public slots:
    void moveRobot();
    void resetRobot();
    void selectRobot();
    void moveCurrentRobot();
    void resetCurrentRobot();
    void moveBall();
    void changeCameraMode();
    void yellowRobotsMenuTriggered(QAction* act);
    void blueRobotsMenuTriggered(QAction* act);
    void switchRobotOnOff();
    void moveRobotHere();
    void moveBallHere();
    void lockCameraToRobot();
    void lockCameraToBall();
signals:
    void clicked();
    void selectedRobot();
    void closeSignal(bool);
    void toggleFullScreen(bool);
    void robotTurnedOnOff(int,bool);
protected:
    void paintGL ();
    void initializeGL ();    

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);    
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent *event);    
private:
    int state;
    int moving_robot_id,clicked_robot;
    int frames;
    bool first_time;
    QTime time,rendertimer;
    float m_fps;
    QPoint lastPos;
friend class GLWidgetGraphicsView;
};

class GLWidgetGraphicsView : public QGraphicsView        
{
    private:
        GLWidget *glwidget;
    public:
        GLWidgetGraphicsView(QGraphicsScene *scene,GLWidget* _glwidget);
    protected:
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);
        void keyPressEvent(QKeyEvent *event);
        void closeEvent(QCloseEvent *event);
};

#endif // WIDGET_H
