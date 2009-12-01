#ifndef SSLWORLD_H
#define SSLWORLD_H


#include <QGLWidget>

#include "Graphics/graphics.h"
#include "Physics/pworld.h"
#include "Physics/pball.h"
#include "Physics/pground.h"
#include "Physics/pfixedbox.h"

#include "proto/robocup_ssl_server.h"

#include "robot.h"
#include <QObject>

#include <QUdpSocket>


class SSLWorld : public QObject
{
    Q_OBJECT
private:
    QGLWidget* m_parent;
    int framenum;
    bool initing;
public:
    SSLWorld(QGLWidget* parent = 0);
    virtual ~SSLWorld();
    void step();
    void sendVisionBuffer();
    struct cmdBuffer {
        bool isNew;
        char m1, m2, m3;
        int shootPower;
        bool spin;
        bool chip;
    } commands[5];
    CGraphics* g;
    PWorld* p;
    PBall* ball;
    PGround* ground;
    PFixedBox* walls[6];
//    QUdpSocket* visionSocket, *commandSocket;
//    CCommThread *commThread;
    //Socket *visionSocket;
    RoboCupSSLServer *visionServer;
    QUdpSocket* commandSocket;

    Robot* robots[10];
public slots:
    void recvActions();
};

#endif // SSLWORLD_H
