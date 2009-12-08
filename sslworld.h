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


class SSLWorld : public QObject
{
    Q_OBJECT
private:
    QGLWidget* m_parent;
    int framenum;
    bool initing;
    float last_dt;
public:

    SSLWorld(QGLWidget* parent,ConfigWidget* _cfg);
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
    } commands[5];
    ConfigWidget* cfg;
    CGraphics* g;
    PWorld* p;
    PBall* ball;
    PGround* ground;
    PRay* ray;
    PFixedBox* walls[6];
    int selected;
    bool show3DCursor;
    float cursor_x,cursor_y,cursor_z;
//    QUdpSocket* visionSocket, *commandSocket;
//    CCommThread *commThread;
    //Socket *visionSocket;
    RoboCupSSLServer *visionServer;
    QUdpSocket* commandSocket;

    Robot* robots[10];
public slots:
    void recvActions();
    void reconnectCommandSocket();
    void reconnectVisionSocket();
};

#endif // SSLWORLD_H
