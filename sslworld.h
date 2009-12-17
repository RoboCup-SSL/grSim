#ifndef SSLWORLD_H
#define SSLWORLD_H


#include <QGLWidget>

#include "Graphics/graphics.h"
#include "Physics/pworld.h"
#include "Physics/pball.h"
#include "Physics/pground.h"
#include "Physics/pfixedbox.h"
#include "Physics/pray.h"

#include "proto/robocup_ssl_server.h"

#include "robot.h"

#include "configwidget.h"

#include <QObject>

#include <QUdpSocket>

#define ROBOT_COUNT 5

class RobotsFomation;

class SSLWorld : public QObject
{
    Q_OBJECT
private:
    QGLWidget* m_parent;
    int framenum;
    float last_dt;
public:

    SSLWorld(QGLWidget* parent,ConfigWidget* _cfg,RobotsFomation *form1,RobotsFomation *form2);
    virtual ~SSLWorld();
    void glinit();
    void step(float dt=-1);
    void sendVisionBuffer();
    struct cmdBuffer {
        bool isNew;
        char m1, m2, m3;
        int shootPower;
        bool spin;
        bool chip;
    } commands[ROBOT_COUNT*2];
    ConfigWidget* cfg;
    CGraphics* g;
    PWorld* p;
    PBall* ball;
    PGround* ground;
    PRay* ray;
    PFixedBox* walls[10];
    int selected;
    bool show3DCursor;
    float cursor_x,cursor_y,cursor_z;
    float cursor_radius;
    RoboCupSSLServer *visionServer;
    QUdpSocket *blueSocket,*yellowSocket;

    Robot* robots[ROBOT_COUNT*2];
    void recvActions(QUdpSocket* commandSocket,int team);
public slots:
    void recvFromBlue();
    void recvFromYellow();
    void reconnectBlueCommandSocket();
    void reconnectYellowCommandSocket();
    void reconnectVisionSocket();
};

class RobotsFomation {
    public:
        float x[ROBOT_COUNT];
        float y[ROBOT_COUNT];
        RobotsFomation(int type);
        void setAll(float *xx,float *yy);
        void loadFromFile(const QString& filename);
        void resetRobots(Robot** r,int team);
};

dReal fric(float f);
int robotIndex(int robot,int team);

#endif // SSLWORLD_H
