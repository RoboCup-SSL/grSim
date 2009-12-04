#include "sslworld.h"

#include <QtGlobal>
#include <QtNetwork>

#include <QDebug>


#define ROBOT_GRAY 0.4

SSLWorld* _w;
bool wheelCallBack(dGeomID o1,dGeomID o2,PSurface* s)
{
    //s->id2 is ground
    const dReal* r; //wheels rotation matrix
    const dReal* p; //wheels rotation matrix
    //dQuaternion q;
    if ((o1==s->id1) && (o2==s->id2))
    {
        r=dBodyGetRotation(dGeomGetBody(o1));
        p=dGeomGetPosition(o1);
        //dGeomGetQuaternion(o1,q);
    }
    if ((o1==s->id2) && (o2==s->id1))
    {
        //r=dGeomGetRotation(o2);
        r=dBodyGetRotation(dGeomGetBody(o2));
        //dGeomGetQuaternion(o2,q);
        p=dGeomGetPosition(o2);
    }
    //s->surface.mode = dContactFDir1 | dContactSlip1 | dContactSlip2 | dContactApprox1 | dContactSoftCFM;//dContactMu2 | dContactFDir1;
    s->surface.mode = dContactFDir1 | dContactMu2 | dContactApprox1 | dContactSoftCFM;
/*    s->surface.mu = 5;
    s->surface.mu2 = 10;*/

    /* Ideal Wheel + Carpet */
    s->surface.mu = 0.01;  //perpendicular
    s->surface.mu2 = 0.8;

    /* Bad Wheel - 1 */

    //s->surface.mu = 0.5;  //perpendicular
    //s->surface.mu2 = 0.8;

    /* Bad Wheel - 2 */

//    s->surface.mu = 0.1;  //perpendicular
//    s->surface.mu2 = 0.1;


    s->surface.slip1 = 0.0;
    s->surface.slip2 = 0.0;



    s->surface.soft_cfm = 0.002;

    dVector3 v={0,0,1,1};
    dVector3 axis;
    dMultiply0(axis,r,v,4,3,1);
    float l = sqrt(axis[0]*axis[0] + axis[1]*axis[1]);
//    s->surface.slip1 = 1;//fabs(axis[0]/l);
//    s->surface.slip2 = 1;//fabs(axis[1]/l);
    s->fdir1[0] = axis[0]/l;
    s->fdir1[1] = axis[1]/l;
    s->fdir1[2] = 0;
    s->fdir1[3] = 0;

//    float c = sqrt(q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
/*    glBegin(GL_LINES);
        glColor3f(1,0,0);
        glLineWidth(2);
        glVertex3f(p[0],p[1],p[2]);
        glVertex3f(p[0]+axis[0]*0.2,p[1]+axis[1]*0.2,p[2]+axis[2]*0.2);
    glEnd();
*/
    s->usefdir1 = true;
    return true;
}

bool rayCallback(dGeomID o1,dGeomID o2,PSurface* s)
{
    dGeomID obj;
    if (o1==_w->ray->geom) obj = o2;
    else obj = o1;
    for (int i=0;i<10;i++)
    {
        if (_w->robots[i]->chassis->geom==obj || _w->robots[i]->dummy->geom==obj)
        {
             _w->robots[i]->selected = true;
             _w->robots[i]->select_x = s->contactPos[0];
             _w->robots[i]->select_y = s->contactPos[1];
             _w->robots[i]->select_z = s->contactPos[2];
        }
    }
    if (_w->ball->geom==obj)
    {
        _w->selected = -2;
    }
    if (_w->show3DCursur && obj==_w->ground->geom)
    {
        dMatrix3 R;
        dRFromAxisAndAngle(R,s->contactNormal[0],s->contactNormal[1],s->contactNormal[2],0);
        _w->g->setColor(1,0.9,0.2,0.5);
        _w->g->drawCylinder(s->contactPos,R,0.1,0.02);
        _w->cursur_x = s->contactPos[0];
        _w->cursur_y = s->contactPos[1];
        _w->cursur_z = s->contactPos[2];
    }
    //if ((s->id1==o1) && (s->id2==o2))
    /*{
    }*/
    return false;
}

SSLWorld::SSLWorld(QGLWidget* parent) : QObject(parent)
{
    _w = this;
    show3DCursur = false;
    framenum = 0;
    last_dt = -1;
    m_parent = parent;
    g = new CGraphics(parent);
    g->setSphereQuality(1);
    p = new PWorld(0.05,9.8,g);
    ball = new PBall (0,0,0.5,BALLRADIUS,BALLMASS, 1,0.7,0);

    ground = new PGround(_SSL_FIELD_RAD,_SSL_FIELD_LENGTH,_SSL_FIELD_WIDTH,0);
    ray = new PRay(50);
    walls[0] = new PFixedBox(0.0,((_SSL_FIELD_WIDTH + _SSL_FIELD_MARGIN) / 2000.0) + (_SSL_WALL_THICKNESS / 2000.0),0.0
                             ,(_SSL_FIELD_LENGTH + _SSL_FIELD_MARGIN) / 1000.0, _SSL_WALL_THICKNESS / 1000.0, 0.4,
                             0.7, 0.7, 0.7);
    walls[1] = new PFixedBox(0.0,((_SSL_FIELD_WIDTH + _SSL_FIELD_MARGIN) / -2000.0) - (_SSL_WALL_THICKNESS / 2000.0),0.0,
            (_SSL_FIELD_LENGTH + _SSL_FIELD_MARGIN) / 1000.0, _SSL_WALL_THICKNESS / 1000.0, 0.4,
            0.7, 0.7, 0.7);

    walls[2] = new PFixedBox(((_SSL_FIELD_LENGTH + _SSL_FIELD_MARGIN) / 2000.0) + (_SSL_WALL_THICKNESS / 2000.0),0.0 ,0.0,
        _SSL_WALL_THICKNESS / 1000.0 ,(_SSL_FIELD_WIDTH + _SSL_FIELD_MARGIN) / 1000.0, 0.4,
        0.7, 0.7, 0.7);

    walls[3] = new PFixedBox(((_SSL_FIELD_LENGTH + _SSL_FIELD_MARGIN) / -2000.0) - (_SSL_WALL_THICKNESS / 2000.0),0.0 ,0.0,
        _SSL_WALL_THICKNESS / 1000.0 , (_SSL_FIELD_WIDTH + _SSL_FIELD_MARGIN) / 1000.0, 0.4,
        0.7, 0.7, 0.7);

    walls[4] = new PFixedBox(( (_SSL_FIELD_LENGTH / -2000.0)) - 0.025 ,0.0, 0.0,
        0.05, 0.7, 0.4,
        0.1, 0.9, 0.4);
    walls[5] = new PFixedBox(( (_SSL_FIELD_LENGTH / 2000.0)) + 0.025 ,0.0, 0.0,
        0.05, 0.7, 0.4,
        0.1, 0.9, 0.4);


    p->addObject(ground);
    p->addObject(ball);
    p->addObject(ray);
    for (int i=0;i<6;i++)
        p->addObject(walls[i]);



        // Best Start
        dReal OurTeamPosX[5] = {-2.2, -1.0 , -1.0, -1.0, -0.33};
        dReal OurTeamPosY[5] = {0.0, -0.75 , 0.0, 0.75, 0.25};

/*	// Test
        dReal OurTeamPosX[5] = {-2.0, -2.0 , 0.0, -2.2, -0.33};
        dReal OurTeamPosY[5] = {0.0, 1.0 , -0.7, 0.7, 1.25};

        dReal OurTeamPosX[5] = {-2.2, -1.0 , -1.0, 0.0, -0.33};
        dReal OurTeamPosY[5] = {5.0, 5.75 , -5.75, 0.0, 5.25};

        dReal OurTeamPosX[5] = {-2.2, -1.0 , -1.0, -1.0, -0.70};
        dReal OurTeamPosY[5] = {0.0, -0.75 , -0.40, 0.75, 0.75};

        dReal OurTeamPosX[5] = {0.0, -1.0 , -1.0, -1.0, -0.70};
        dReal OurTeamPosY[5] = {0.0, -0.75 , -0.40, 0.75, 0.75};
*/
        for (int k=0;k<5;k++)
        {
                robots[k] = new Robot(p,ball,OurTeamPosX[k],OurTeamPosY[k],STARTZ,ROBOT_GRAY,ROBOT_GRAY,ROBOT_GRAY,k+1);
        }
        //Defend
//	dReal OppTeamPosX[5] = {2.8, 2.5, 2.5, 0.8, 0.8};
//	dReal OppTeamPosY[5] = {0.0, -0.3, 0.3, 0.0, 1.5};//{0.0, -0.3, 0.3, 0.0, 1.5};

        //Test
        //dReal OppTeamPosX[5] = {-1.5, 1.0, 1.0, 0.3, 0.8};
        //dReal OppTeamPosY[5] = {-6.0, -5.8, 5.8, 5.0, 5.0};

        dReal OppTeamPosX[5] = {1.0, 1.0, 1.0, 0.33, 1.7};
        dReal OppTeamPosY[5] = {0.75, 0.0, -0.75, -0.25, 0.0};

        //dReal OppTeamPosX[5] = {1.0, 1.0, 1.0, 0.0, 0.0};
        //dReal OppTeamPosY[5] = {0.35, 0.0, -0.35, -0.50, 0.50};
        for (int k=0;k<5;k++)
        {
                robots[k+5] = new Robot(p,ball,OppTeamPosX[k],OppTeamPosY[k],STARTZ,ROBOT_GRAY,ROBOT_GRAY,ROBOT_GRAY,k+6);
               // robots[k+5]->chassis->setBodyRotation(0,0,1,M_PI);
                //robots[k+5]->kicker->box->setBodyRotation(0,0,1,M_PI);
        }
        initing = true;


    dBodySetLinearDampingThreshold(ball->body,0.001);
    dBodySetLinearDamping(ball->body,0.004);
    dBodySetAngularDampingThreshold(ball->body,0.001);
    dBodySetAngularDamping(ball->body,0.004);


    p->createSurface(ray,ground)->callback = rayCallback;
    p->createSurface(ray,ball)->callback = rayCallback;
    for (int k=0;k<10;k++)
    {
       p->createSurface(ray,robots[k]->chassis)->callback = rayCallback;
       p->createSurface(ray,robots[k]->dummy)->callback = rayCallback;
    }
    PSurface ballwithwall;
    ballwithwall.surface.mode = dContactBounce | dContactApprox1 | dContactSlip1;
    ballwithwall.surface.mu = 1;
    ballwithwall.surface.bounce = 0.7;
    ballwithwall.surface.bounce_vel = 0.1;
    ballwithwall.surface.slip1 = 1;    

    PSurface wheelswithground;
    p->createSurface(ball,ground)->surface = ballwithwall.surface;

    PSurface ballwithkicker;
    ballwithkicker.surface.mode = dContactApprox1 | dContactMu2;// | dContactSlip1;
    ballwithkicker.surface.mu = dInfinity;
    ballwithkicker.surface.mu2 = 0.01;
//    ballwithkicker.surface.bounce = 0.2;
//    ballwithkicker.surface.bounce_vel = 0.1;
    ballwithkicker.surface.slip1 = 5;
    for (int i=0;i<6;i++)
        p->createSurface(ball,walls[i])->surface = ballwithwall.surface;
    for (int k=0;k<10;k++)
    {
        p->createSurface(robots[k]->chassis,ground);
        for (int j=0;j<6;j++)
            p->createSurface(robots[k]->chassis,walls[j]);
        p->createSurface(robots[k]->chassis,ball);
        p->createSurface(robots[k]->kicker->box,ball)->surface = ballwithkicker.surface;
        for (int j=0;j<4;j++)
        {
            PSurface* w_g = p->createSurface(robots[k]->wheels[j]->cyl,ground);
            w_g->surface=wheelswithground.surface;
            w_g->usefdir1=true;
            w_g->callback=wheelCallBack;
        }
        for (int j=k+1;j<10;j++)
        {            
            if (k!=j)
            {
p->createSurface(robots[k]->dummy,robots[j]->dummy); //seams ode doesn't understand cylinder-cylinder contacts, so I used spheres
                p->createSurface(robots[k]->chassis,robots[j]->kicker->box);
/*                p->createSurface(robots[k]->chassis,robots[j]->boxes[0]);
                p->createSurface(robots[k]->chassis,robots[j]->boxes[1]);
                p->createSurface(robots[k]->chassis,robots[j]->boxes[2]);*/
            }
        }
    }

//    visionSocket = new QUdpSocket(m_parent);


  visionServer = new RoboCupSSLServer(
#ifdef Q_OS_WIN32
          m_parent,
#endif
<<<<<<< .mine
          10004,"127.0.0.1");//"224.5.23.2");
=======
          10002,"127.0.0.1");//"224.5.23.2");
>>>>>>> .r168

  visionServer->open();


  commandSocket = new QUdpSocket(this);
  if (commandSocket->bind(QHostAddress::Any,20011))
      qDebug() << "binded on port:" << 20011;

  connect(commandSocket,SIGNAL(readyRead()),this,SLOT(recvActions()));
}

SSLWorld::~SSLWorld()
{
    //visionSocket->close();
    //delete visionSocket;
    //delete commThread;
    delete visionServer;
    commandSocket->close();
    delete commandSocket;
}

void SSLWorld::step(float dt)
{    
    if (dt==0) dt=last_dt;
    else last_dt = dt;
    g->initScene(m_parent->width(),m_parent->height(),0,0.7,1);//true,0.7,0.7,0.7,0.8);

    if (initing)
    {
        if (robots[5]->getDir()>=179)
        {
            initing = false;
            for (int k=0;k<5;k++)
            {
                robots[k+5]->setSpeed(0,0);
                robots[k+5]->setSpeed(1,0);
                robots[k+5]->setSpeed(2,0);
                robots[k+5]->setSpeed(3,0);
            }
        }
        else
        {
            for (int k=0;k<5;k++)
            {
                robots[k+5]->setSpeed(0,1);
                robots[k+5]->setSpeed(1,1);
                robots[k+5]->setSpeed(2,1);
                robots[k+5]->setSpeed(3,1);
            }
        }
    }

    selected = -1;
    for (int k=0;k<10;k++)
    {
        robots[k]->step();
        robots[k]->selected = false;
    }

    p->step(dt);

    int best_k=-1;
    float best_dist = 1e8;
    float xyz[3],hpr[3];
    if (selected==-2) {
        best_k=-2;
        float bx,by,bz;
        ball->getBodyPosition(bx,by,bz);
        g->getViewpoint(xyz,hpr);
        best_dist  =(bx-xyz[0])*(bx-xyz[0])
                   +(by-xyz[1])*(by-xyz[1])
                   +(bz-xyz[2])*(bz-xyz[2]);
    }
    for (int k=0;k<10;k++)
    {
        if (robots[k]->selected)
        {
            g->getViewpoint(xyz,hpr);
            float dist= (robots[k]->select_x-xyz[0])*(robots[k]->select_x-xyz[0])
                       +(robots[k]->select_y-xyz[1])*(robots[k]->select_y-xyz[1])
                       +(robots[k]->select_z-xyz[2])*(robots[k]->select_z-xyz[2]);
            if (dist<best_dist) {
                best_dist = dist;
                best_k = k;
            }
        }
        robots[k]->chassis->setColor(ROBOT_GRAY,ROBOT_GRAY,ROBOT_GRAY);
    }
    if (best_k!=-1) robots[best_k]->chassis->setColor(ROBOT_GRAY*2,ROBOT_GRAY*1.5,ROBOT_GRAY*1.5);
    selected = best_k;

    p->draw();



    g->drawSkybox(11,12,13,14,15,16);

    g->finalizeScene();


    sendVisionBuffer();
    framenum ++;
}


#define _VISION_BUF_LEN 512
#define _IMG_FPS_DELAY 0.01667 //100 fps
#define _WHEEL_NUM 4
const float motorfactor = 8;
const float shootfactor = 1;
void SSLWorld::recvActions()
{
        char ch,  m1, m2, m3;
        unsigned char action;
        int nID, shootPower;
        bool spin;
        // Data received from Comm Thread
        QHostAddress sender;
        quint16 port;
        qDebug() << "Recieved";
        while (commandSocket->hasPendingDatagrams())
            {
                char *c = new char [5];
                commandSocket->readDatagram(c,5,&sender,&port);
                if (c[0]!=99) continue;


                action = c[1];
                nID = action & 0x07;
                qDebug() << "Recieved: " << nID;
                commands[nID].isNew = true;
                commands[nID].shootPower = (action & 0x70) >> 4;
                commands[nID].chip = (action & 0x80);
                commands[nID].spin = action & 0x08;

                commands[nID].m1 = c[2];
                commands[nID].m2 = c[3];
                commands[nID].m3 = c[4];


        for (int nID = 0; nID < 5; nID++)
                if (commands[nID].isNew) {
                        if (_WHEEL_NUM == 3) {
                                //vel[nID][0] = commands[nID].m1 * MOTOR_TO_FORCE;
                                //vel[nID][1] = commands[nID].m2 * MOTOR_TO_FORCE;
                                //vel[nID][2] = commands[nID].m3 * MOTOR_TO_FORCE;
                        } else if (_WHEEL_NUM == 4) {
                        char sm1, sm2, sm3, sm4;

                        sm1 =  ( (commands[nID].m1 >> 2) & 0x3F);
                        sm2 = (((commands[nID].m1 << 4) & 0x30 ) | (( commands[nID].m2 >> 4) & 0x0F ) );
                        sm3 = (((commands[nID].m2 << 2) & 0x3C ) | (( commands[nID].m3 >> 6) & 0x03 ) ) ;
                        sm4 = ( (commands[nID].m3 & 0x3f)) ;
                        if (sm1 >= 32) sm1 = sm1 | 0xC0;
                        if (sm2 >= 32) sm2 = sm2 | 0xC0;
                        if (sm3 >= 32) sm3 = sm3 | 0xC0;
                        if (sm4 >= 32) sm4 = sm4 | 0xC0;

                        double dw1 = ((double) sm1 / 31.0) * 1.73;
                        double dw2 = ((double) sm2 / 31.0) * 1.73;
                        double dw3 = ((double) sm3 / 31.0) * 1.73;
                        double dw4 = ((double) sm4 / 31.0) * 1.73;

                        robots[nID]->setSpeed(0,dw1*motorfactor);// 3 1
                        robots[nID]->setSpeed(1,dw2*motorfactor);// 1 3
                        robots[nID]->setSpeed(2,dw3*motorfactor);// 4 2
                        robots[nID]->setSpeed(3,dw4*motorfactor);// 2 4
                        commands[nID].isNew = false;
                        }
                }

                // Applying Shoot and spinner
                for(int nID = 0; nID < 5; nID++) {
                       if ((commands[nID].shootPower > 0))
                           robots[nID]->kicker->kick(((double) commands[nID].shootPower*shootfactor));
                       else if ((commands[nID].chip == true))
                           robots[nID]->kicker->kick(((double) commands[nID].shootPower*shootfactor),true);
                       robots[nID]->kicker->setRoller(commands[nID].spin);
                }

            }
}

void SSLWorld::sendVisionBuffer()
{
    SSL_WrapperPacket packet;
    SSL_DetectionBall* vball = packet.mutable_detection()->add_balls();
    float x,y,z,dir;
    ball->getBodyPosition(x,y,z);
    packet.mutable_detection()->set_camera_id(0);
    packet.mutable_detection()->set_frame_number(framenum);
    packet.mutable_detection()->set_t_capture(0.0f);
    packet.mutable_detection()->set_t_sent(0.0f);
    vball->set_x(x*1000.0f);
    vball->set_y(y*1000.0f);
    vball->set_z(z*1000.0f);
    vball->set_pixel_x(x*1000.0f);
    vball->set_pixel_y(y*1000.0f);
    vball->set_confidence(1);
    for(int i = 0; i < 5; i++){
        SSL_DetectionRobot* rob = packet.mutable_detection()->add_robots_blue();
        robots[i]->getXY(x,y);
        dir = robots[i]->getDir();
        rob->set_robot_id(i);
        rob->set_pixel_x(x*1000.0f);
        rob->set_pixel_y(x*1000.0f);
        rob->set_confidence(1);
        rob->set_x(x*1000.0f);
        rob->set_y(y*1000.0f);
        rob->set_orientation(dir*M_PI/180.0f);
    }
    for(int i = 5; i < 10; i++){
        SSL_DetectionRobot* rob = packet.mutable_detection()->add_robots_yellow();
        robots[i]->getXY(x,y);
        dir = robots[i]->getDir();
        rob->set_robot_id(i-5);
        rob->set_pixel_x(x*1000.0f);
        rob->set_pixel_y(x*1000.0f);
        rob->set_confidence(1);
        rob->set_x(x*1000.0f);
        rob->set_y(y*1000.0f);
        rob->set_orientation(dir*M_PI/180.0f);
   }
    visionServer->send(packet);
#if 0
                // Simulating Vision Frame Rate
                char buf[1000];
                int nID;
/*                float dt = getElapsedTime(false);
                if (dt > _IMG_FPS_DELAY) { // is XXX fps? send via UDP
*/

                        // constructing vision buf
                        sprintf(buf,"[VISION],");
                        // Our team data
                        for(nID = 0; nID < 5; nID++){
                                float x,y,dir;
                                robots[nID]->getXY(x,y);
                                dir = robots[nID]->getDir();
                                sprintf(buf,"%s%d %d %d %d,",buf,
                                        (int) (x*1000.0f),
                                        (int) (y*1000.0f),
                                        (int) dir,
                                        1);

                        }

                        //opp Team data
                        for(nID = 5; nID < 10; nID++){
                                float x,y,dir;
                                robots[nID]->getXY(x,y);
                                dir = robots[nID]->getDir();
                                sprintf(buf,"%s%d %d %d %d,",buf,
                                        (int) (x*1000.0f),
                                        (int) (y*1000.0f),
                                        (int) dir,
                                        1);

                        }

                        //Ball
                        float x,y,z;
                        ball->getBodyPosition(x,y,z);
                        sprintf(buf,"%s%d %d %d,",buf,
                                (int) (x*1000.0f),
                                (int) (y*1000.0f),
                                1);
//                        QByteArray a(buf);
                        //udpSocket->writeDatagram(a,QHostAddress::Any, VISION_PORT);
/*    udpSocket->writeDatagram(datagram.data(), datagram.size(),
                             QHostAddress::Broadcast, 45454);                                */
//printf("%s\n",buf);
                        visionSocket->send(buf,strlen(buf)+1);


                        //getElapsedTime(true);
/*
                        try {
                                char visionBuffer[_VISION_BUF_LEN];
                                sock->sendTo(buf,_VISION_BUF_LEN,_UDP_BRDCAST,_OUR_PORT_VISION);
                        }
                        catch (SocketException &e) {
                                cout << e.what() << endl;
                                exit(1);
                        } */

#endif
}
