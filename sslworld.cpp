#include "sslworld.h"

#include <QtGlobal>
#include <QtNetwork>

#include <QDebug>

#include "logger.h"

#define ROBOT_GRAY 0.4

SSLWorld* _w;

dReal fric(float f)
{
    if (f==-1) return dInfinity;
    return f;
}

int robotIndex(int robot,int team)
{
    return robot + team*ROBOT_COUNT;
}

bool wheelCallBack(dGeomID o1,dGeomID o2,PSurface* s)
{
    //s->id2 is ground
    const dReal* r; //wheels rotation matrix
    const dReal* p; //wheels rotation matrix
    if ((o1==s->id1) && (o2==s->id2))
    {
        r=dBodyGetRotation(dGeomGetBody(o1));
        p=dGeomGetPosition(o1);
    }
    if ((o1==s->id2) && (o2==s->id1))
    {
        r=dBodyGetRotation(dGeomGetBody(o2));
        p=dGeomGetPosition(o2);
    }

    s->surface.mode = dContactFDir1 | dContactMu2  | dContactApprox1 | dContactSoftCFM;
    s->surface.mu = fric(_w->cfg->wheelperpendicularfriction());
    s->surface.mu2 = fric(_w->cfg->wheeltangentfriction());
    s->surface.soft_cfm = 0.002;

    dVector3 v={0,0,1,1};
    dVector3 axis;
    dMultiply0(axis,r,v,4,3,1);
    float l = sqrt(axis[0]*axis[0] + axis[1]*axis[1]);
    s->fdir1[0] = axis[0]/l;
    s->fdir1[1] = axis[1]/l;
    s->fdir1[2] = 0;
    s->fdir1[3] = 0;
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
    if (obj==_w->ground->geom)
    {
        _w->cursor_x = s->contactPos[0];
        _w->cursor_y = s->contactPos[1];
        _w->cursor_z = s->contactPos[2];
    }
    return false;
}

bool ballCallBack(dGeomID o1,dGeomID o2,PSurface* s)
{
    if (_w->ball->tag!=-1) //spinner adjusting
    {
        float x,y,z;
        _w->robots[_w->ball->tag]->chassis->getBodyDirection(x,y,z);
        s->fdir1[0] = x;
        s->fdir1[1] = y;
        s->fdir1[2] = 0;
        s->fdir1[3] = 0;
        s->usefdir1 = true;
        s->surface.mode = dContactMu2 | dContactFDir1 | dContactSoftCFM;
        s->surface.mu = _w->cfg->ballfriction();
        s->surface.mu2 = 0;
        s->surface.soft_cfm = 0.002;
    }
    return true;
}

SSLWorld::SSLWorld(QGLWidget* parent,ConfigWidget* _cfg,RobotsFomation *form1,RobotsFomation *form2)
        : QObject(parent)
{    
    _w = this;
    cfg = _cfg;
    m_parent = parent;
    show3DCursor = false;
    framenum = 0;
    last_dt = -1;    
    g = new CGraphics(parent);
    g->setSphereQuality(1);
    g->setViewpoint(0,-(cfg->_SSL_FIELD_WIDTH()+cfg->_SSL_FIELD_MARGIN()*2.0f)/2000.0f,3,90,-45,0);
    p = new PWorld(0.05,9.81f,g);
    ball = new PBall (0,0,0.5,cfg->BALLRADIUS(),cfg->BALLMASS(), 1,0.7,0);

    ground = new PGround(cfg->_SSL_FIELD_RAD(),cfg->_SSL_FIELD_LENGTH(),cfg->_SSL_FIELD_WIDTH(),cfg->_SSL_FIELD_PENALTY_RADIUS(),cfg->_SSL_FIELD_PENALTY_LINE(),cfg->_SSL_FIELD_PENALTY_POINT(),0);
    ray = new PRay(50);
    walls[0] = new PFixedBox(0.0,((cfg->_SSL_FIELD_WIDTH() + cfg->_SSL_FIELD_MARGIN()) / 2000.0) + (cfg->_SSL_WALL_THICKNESS() / 2000.0),0.0
                             ,(cfg->_SSL_FIELD_LENGTH() + cfg->_SSL_FIELD_MARGIN()) / 1000.0, cfg->_SSL_WALL_THICKNESS() / 1000.0, 0.4,
                             0.7, 0.7, 0.7);
    walls[1] = new PFixedBox(0.0,((cfg->_SSL_FIELD_WIDTH() + cfg->_SSL_FIELD_MARGIN()) / -2000.0) - (cfg->_SSL_WALL_THICKNESS() / 2000.0),0.0,
            (cfg->_SSL_FIELD_LENGTH() + cfg->_SSL_FIELD_MARGIN()) / 1000.0, cfg->_SSL_WALL_THICKNESS() / 1000.0, 0.4,
            0.7, 0.7, 0.7);

    walls[2] = new PFixedBox(((cfg->_SSL_FIELD_LENGTH() + cfg->_SSL_FIELD_MARGIN()) / 2000.0) + (cfg->_SSL_WALL_THICKNESS() / 2000.0),0.0 ,0.0,
        cfg->_SSL_WALL_THICKNESS() / 1000.0 ,(cfg->_SSL_FIELD_WIDTH() + cfg->_SSL_FIELD_MARGIN()) / 1000.0, 0.4,
        0.7, 0.7, 0.7);

    walls[3] = new PFixedBox(((cfg->_SSL_FIELD_LENGTH() + cfg->_SSL_FIELD_MARGIN()) / -2000.0) - (cfg->_SSL_WALL_THICKNESS() / 2000.0),0.0 ,0.0,
        cfg->_SSL_WALL_THICKNESS() / 1000.0 , (cfg->_SSL_FIELD_WIDTH() + cfg->_SSL_FIELD_MARGIN()) / 1000.0, 0.4,
        0.7, 0.7, 0.7);

    walls[4] = new PFixedBox(( cfg->_SSL_FIELD_LENGTH() / 2000.0) + cfg->_SSL_GOAL_DEPTH()*0.001f + cfg->_SSL_GOAL_THICKNESS()*0.5f*0.001f ,0.0, 0.0
        , cfg->_SSL_GOAL_THICKNESS()*0.001f, cfg->_SSL_GOAL_WIDTH()*0.001f + cfg->_SSL_GOAL_THICKNESS()*2.0f*0.001f, cfg->_SSL_GOAL_HEIGHT()*0.001f , 0.1, 0.9, 0.4);
    walls[5] = new PFixedBox(( cfg->_SSL_FIELD_LENGTH() / 2000.0) + cfg->_SSL_GOAL_DEPTH()*0.5*0.001f,-cfg->_SSL_GOAL_WIDTH()*0.5f*0.001f - cfg->_SSL_GOAL_THICKNESS()*0.5f*0.001f,0.0
        , cfg->_SSL_GOAL_DEPTH()*0.001f, cfg->_SSL_GOAL_THICKNESS()*0.001f, cfg->_SSL_GOAL_HEIGHT()*0.001f , 0.1, 0.9, 0.4);
    walls[6] = new PFixedBox(( cfg->_SSL_FIELD_LENGTH() / 2000.0) + cfg->_SSL_GOAL_DEPTH()*0.5*0.001f,cfg->_SSL_GOAL_WIDTH()*0.5f*0.001f+ cfg->_SSL_GOAL_THICKNESS()*0.5f*0.001f, 0.0
        , cfg->_SSL_GOAL_DEPTH()*0.001f, cfg->_SSL_GOAL_THICKNESS()*0.001f, cfg->_SSL_GOAL_HEIGHT()*0.001f , 0.1, 0.9, 0.4);

    walls[7] = new PFixedBox(- (( cfg->_SSL_FIELD_LENGTH() / 2000.0) + cfg->_SSL_GOAL_DEPTH()*0.001f + cfg->_SSL_GOAL_THICKNESS()*0.5f*0.001f) ,0.0, 0.0
        , cfg->_SSL_GOAL_THICKNESS()*0.001f, cfg->_SSL_GOAL_WIDTH()*0.001f + cfg->_SSL_GOAL_THICKNESS()*2.0f*0.001f, cfg->_SSL_GOAL_HEIGHT()*0.001f , 0.1, 0.9, 0.4);
    walls[8] = new PFixedBox(- (( cfg->_SSL_FIELD_LENGTH() / 2000.0) + cfg->_SSL_GOAL_DEPTH()*0.5*0.001f),-cfg->_SSL_GOAL_WIDTH()*0.5f*0.001f - cfg->_SSL_GOAL_THICKNESS()*0.5f*0.001f,0.0
        , cfg->_SSL_GOAL_DEPTH()*0.001f, cfg->_SSL_GOAL_THICKNESS()*0.001f, cfg->_SSL_GOAL_HEIGHT()*0.001f , 0.1, 0.9, 0.4);
    walls[9] = new PFixedBox( -(( cfg->_SSL_FIELD_LENGTH() / 2000.0) + cfg->_SSL_GOAL_DEPTH()*0.5*0.001f),cfg->_SSL_GOAL_WIDTH()*0.5f*0.001f+ cfg->_SSL_GOAL_THICKNESS()*0.5f*0.001f, 0.0
        , cfg->_SSL_GOAL_DEPTH()*0.001f, cfg->_SSL_GOAL_THICKNESS()*0.001f, cfg->_SSL_GOAL_HEIGHT()*0.001f , 0.1, 0.9, 0.4);
    p->addObject(ground);
    p->addObject(ball);
    p->addObject(ray);
    for (int i=0;i<10;i++)
        p->addObject(walls[i]);
    const int wheeltexid = 22;


    for (int k=0;k<5;k++)
       robots[k] = new Robot(p,ball,cfg,-form1->x[k],form1->y[k],ROBOT_START_Z(cfg),ROBOT_GRAY,ROBOT_GRAY,ROBOT_GRAY,k+1,wheeltexid,1);
    for (int k=0;k<5;k++)
       robots[k+5] = new Robot(p,ball,cfg,form2->x[k],form2->y[k],ROBOT_START_Z(cfg),ROBOT_GRAY,ROBOT_GRAY,ROBOT_GRAY,k+6,wheeltexid,-1);

    dBodySetLinearDampingThreshold(ball->body,0.001);
    dBodySetLinearDamping(ball->body,cfg->balllineardamp());
    dBodySetAngularDampingThreshold(ball->body,0.001);
    dBodySetAngularDamping(ball->body,cfg->ballangulardamp());


    p->createSurface(ray,ground)->callback = rayCallback;
    p->createSurface(ray,ball)->callback = rayCallback;
    for (int k=0;k<10;k++)
    {
       p->createSurface(ray,robots[k]->chassis)->callback = rayCallback;
       p->createSurface(ray,robots[k]->dummy)->callback = rayCallback;
    }
    PSurface ballwithwall;
    ballwithwall.surface.mode = dContactBounce | dContactApprox1 | dContactSlip1;
    ballwithwall.surface.mu = fric(cfg->ballfriction());
    ballwithwall.surface.bounce = cfg->ballbounce();
    ballwithwall.surface.bounce_vel = cfg->ballbouncevel();
    ballwithwall.surface.slip1 = cfg->ballslip();

    PSurface wheelswithground;
    PSurface* ball_ground = p->createSurface(ball,ground);
    ball_ground->surface = ballwithwall.surface;
    ball_ground->callback = ballCallBack;

    PSurface ballwithkicker;
    ballwithkicker.surface.mode = dContactApprox1;
    ballwithkicker.surface.mu = fric(cfg->Kicker_Friction());
//    ballwithkicker.surface.bounce = 0.2;
//    ballwithkicker.surface.bounce_vel = 0.1;
    ballwithkicker.surface.slip1 = 5;
    for (int i=0;i<10;i++)
        p->createSurface(ball,walls[i])->surface = ballwithwall.surface;
    for (int k=0;k<10;k++)
    {
        p->createSurface(robots[k]->chassis,ground);
        for (int j=0;j<10;j++)
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
            }
        }
    }

  visionServer = NULL;
  reconnectVisionSocket();
  blueSocket = NULL;
  reconnectBlueCommandSocket();
  yellowSocket = NULL;
  reconnectYellowCommandSocket();
}

void SSLWorld::reconnectBlueCommandSocket()
{
  if (blueSocket!=NULL)
  {
      QObject::disconnect(blueSocket,SIGNAL(readyRead()),this,SLOT(recvFromBlue()));
      delete blueSocket;
  }
  blueSocket = new QUdpSocket(this);
  if (blueSocket->bind(QHostAddress::Any,cfg->BlueCommandListenPort()))
    logStatus(QString("Command listen port binded for Blue Team on: %1").arg(cfg->BlueCommandListenPort()),QColor("green"));
  QObject::connect(blueSocket,SIGNAL(readyRead()),this,SLOT(recvFromBlue()));
}


void SSLWorld::reconnectYellowCommandSocket()
{
  if (yellowSocket!=NULL)
  {
      QObject::disconnect(yellowSocket,SIGNAL(readyRead()),this,SLOT(recvFromYellow()));
      delete yellowSocket;
  }
  yellowSocket = new QUdpSocket(this);
  if (yellowSocket->bind(QHostAddress::Any,cfg->YellowCommandListenPort()))
    logStatus(QString("Command listen port binded for Yellow Team on: %1").arg(cfg->YellowCommandListenPort()),QColor("green"));
  QObject::connect(yellowSocket,SIGNAL(readyRead()),this,SLOT(recvFromYellow()));
}


void SSLWorld::reconnectVisionSocket()
{
  if (visionServer!=NULL)
      delete visionServer;
  visionServer = new RoboCupSSLServer(
#ifdef Q_OS_WIN32
          m_parent,
#endif
          cfg->VisionMulticastPort(),cfg->VisionMulticastAddr());
  visionServer->open();
}


SSLWorld::~SSLWorld()
{
    delete visionServer;
    yellowSocket->close();
    blueSocket->close();
    delete yellowSocket;
    delete blueSocket;
}

QImage* createBlob(char yb,int i,QImage** res)
{
    QImage* img = new QImage(QString(":/Graphics/%1%2").arg(yb).arg(i+1)+QString(".bmp"));
/*  QPainter *p = new QPainter();
    p->begin(img);
    QPen pen;
    pen.setStyle(Qt::DashDotLine);
    pen.setWidth(3);
    pen.setBrush(Qt::gray);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p->setPen(pen);
    QFont f;
    f.setPointSize(35);
    p->setFont(f);
    p->drawText(img->width()/2-15,img->height()/2-15,30,30,Qt::AlignCenter,QString("%1").arg(i));
    p->end();
    delete p;*/
    (*res) = img;
    return img;
}

QImage* createNumber(int i)
{
    QImage* img = new QImage(32,32,QImage::Format_RGB32);
    QPainter *p = new QPainter();
    QBrush br;
    p->begin(img);
    br.setColor(QColor("black"));
    br.setStyle(Qt::SolidPattern);
    p->fillRect(0,0,32,32,br);
    QPen pen;
    pen.setStyle(Qt::DashDotLine);
    pen.setWidth(3);
    pen.setBrush(Qt::white);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p->setPen(pen);
    QFont f;
    f.setPointSize(35);
    p->setFont(f);
    p->drawText(img->width()/2-15,img->height()/2-15,30,30,Qt::AlignCenter,QString("%1").arg(i));
    p->end();
    delete p;
    return img;
}


void SSLWorld::glinit()
{
    g->loadTexture(new QImage(":/Graphics/grass001.bmp"));
    g->loadTexture(createBlob('b',0,&robots[0]->img));
    g->loadTexture(createBlob('b',1,&robots[1]->img));
    g->loadTexture(createBlob('b',2,&robots[2]->img));
    g->loadTexture(createBlob('b',3,&robots[3]->img));
    g->loadTexture(createBlob('b',4,&robots[4]->img));
    g->loadTexture(createBlob('y',0,&robots[5]->img));
    g->loadTexture(createBlob('y',1,&robots[6]->img));
    g->loadTexture(createBlob('y',2,&robots[7]->img));
    g->loadTexture(createBlob('y',3,&robots[8]->img));
    g->loadTexture(createBlob('y',4,&robots[9]->img));
    g->loadTexture(createNumber(0));
    g->loadTexture(createNumber(1));
    g->loadTexture(createNumber(2));
    g->loadTexture(createNumber(3));
    g->loadTexture(createNumber(4));
    g->loadTexture(new QImage("../Graphics/sky/neg_x.bmp"));
    g->loadTexture(new QImage("../Graphics/sky/pos_x.bmp"));
    g->loadTexture(new QImage("../Graphics/sky/neg_y.bmp"));
    g->loadTexture(new QImage("../Graphics/sky/pos_y.bmp"));
    g->loadTexture(new QImage("../Graphics/sky/neg_z.bmp"));
    g->loadTexture(new QImage("../Graphics/sky/pos_z.bmp"));
    g->loadTexture(new QImage("../Graphics/Wheel.png"));
    //pos_y neg_x neg_y pos_x pos_z neg_z
    p->glinit();
}

void SSLWorld::step(float dt)
{    
    if (dt==0) dt=last_dt;
    else last_dt = dt;
    g->initScene(m_parent->width(),m_parent->height(),0,0.7,1);//true,0.7,0.7,0.7,0.8);
    selected = -1;
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
    if (best_k>=0) robots[best_k]->chassis->setColor(ROBOT_GRAY*2,ROBOT_GRAY*1.5,ROBOT_GRAY*1.5);
    selected = best_k;
    ball->tag = -1;
    for (int k=0;k<10;k++)
    {
        robots[k]->step();
        robots[k]->selected = false;
    }

    p->draw();
    g->drawSkybox(16,17,18,19,20,21);

    dMatrix3 R;
    if (show3DCursor)
    {
        dRFromAxisAndAngle(R,0,0,1,0);
        g->setColor(1,0.9,0.2,0.5);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        g->drawCircle(cursor_x,cursor_y,0.001,cursor_radius);
        glDisable(GL_BLEND);
    }

    for (int k=0;k<10;k++) robots[k]->drawLabel();

    g->finalizeScene();


    sendVisionBuffer();
    framenum ++;
}

void SSLWorld::recvFromBlue()
{
    recvActions(blueSocket,0);
}


void SSLWorld::recvFromYellow()
{
    recvActions(yellowSocket,1);
}

void SSLWorld::recvActions(QUdpSocket* commandSocket,int team)
{
    unsigned char action;
    int nID;
    // Data received from Comm Thread
    QHostAddress sender;
    quint16 port;
    while (commandSocket->hasPendingDatagrams())
    {
        char *c = new char [5];
        commandSocket->readDatagram(c,5,&sender,&port);
        if (c[0]!=99) continue;
        action = c[1];
        nID = action & 0x07;
        if (nID>=ROBOT_COUNT) continue;
        nID = robotIndex(nID,team);
        commands[nID].shootPower = (action & 0x70) >> 4;
        commands[nID].chip = (action & 0x80);
        commands[nID].spin = action & 0x08;

        commands[nID].m1 = c[2];
        commands[nID].m2 = c[3];
        commands[nID].m3 = c[4];
        char sm1, sm2, sm3, sm4;

        sm1 =  ( (commands[nID].m1 >> 2) & 0x3F);
        sm2 = (((commands[nID].m1 << 4) & 0x30 ) | (( commands[nID].m2 >> 4) & 0x0F ) );
        sm3 = (((commands[nID].m2 << 2) & 0x3C ) | (( commands[nID].m3 >> 6) & 0x03 ) ) ;
        sm4 = ( (commands[nID].m3 & 0x3f)) ;
        if (sm1 >= 32) sm1 = sm1 | 0xC0;
        if (sm2 >= 32) sm2 = sm2 | 0xC0;
        if (sm3 >= 32) sm3 = sm3 | 0xC0;
        if (sm4 >= 32) sm4 = sm4 | 0xC0;

        robots[nID]->setSpeed(0,sm1);
        robots[nID]->setSpeed(1,sm2);
        robots[nID]->setSpeed(2,sm3);
        robots[nID]->setSpeed(3,sm4);
        // Applying Shoot and spinner
       if ((commands[nID].shootPower > 0))
           robots[nID]->kicker->kick(((double) commands[nID].shootPower*cfg->shootfactor()));
       else if ((commands[nID].chip == true))
           robots[nID]->kicker->kick(((double) commands[nID].shootPower*cfg->shootfactor()),true);
       robots[nID]->kicker->setRoller(commands[nID].spin);
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
}

void RobotsFomation::setAll(float* xx,float *yy)
{
    for (int i=0;i<ROBOT_COUNT;i++)
    {
        x[i] = xx[i];
        y[i] = yy[i];
    }
}

RobotsFomation::RobotsFomation(int type)
{
    if (type==0)
    {
        float teamPosX[ROBOT_COUNT] = {2.2, 1.0 , 1.0, 1.0, 0.33};
        float teamPosY[ROBOT_COUNT] = {0.0, -0.75 , 0.0, 0.75, 0.25};
        setAll(teamPosX,teamPosY);
    }
    if (type==1)
    {
        float teamPosX[ROBOT_COUNT] = {1.0, 1.0, 1.0, 0.33, 1.7};
        float teamPosY[ROBOT_COUNT] = {0.75, 0.0, -0.75, -0.25, 0.0};
        setAll(teamPosX,teamPosY);
    }
    if (type==2)
    {
        float teamPosX[ROBOT_COUNT] = {2.8, 2.5, 2.5, 0.8, 0.8};
        float teamPosY[ROBOT_COUNT] = {0.0, -0.3, 0.3, 0.0, 1.5};
        setAll(teamPosX,teamPosY);
    }
    if (type==-1)
    {
        float teamPosX[ROBOT_COUNT] = {6, 6, 6, 6, 6};
        float teamPosY[ROBOT_COUNT] = {-0.8, -0.4, 0, 0.4, 0.8};
        setAll(teamPosX,teamPosY);
    }
/*
        float teamPosX[5] = {-2.0, -2.0 , 0.0, -2.2, -0.33};
        float teamPosY[5] = {0.0, 1.0 , -0.7, 0.7, 1.25};

        float teamPosX[5] = {-2.2, -1.0 , -1.0, 0.0, -0.33};
        float teamPosY[5] = {5.0, 5.75 , -5.75, 0.0, 5.25};

        float teamPosX[5] = {-2.2, -1.0 , -1.0, -1.0, -0.70};
        float teamPosY[5] = {0.0, -0.75 , -0.40, 0.75, 0.75};

        float teamPosX[5] = {0.0, -1.0 , -1.0, -1.0, -0.70};
        float teamPosY[5] = {0.0, -0.75 , -0.40, 0.75, 0.75};

        float teamPosX[5] = {2.8, 2.5, 2.5, 0.8, 0.8};
        float teamPosY[5] = {0.0, -0.3, 0.3, 0.0, 1.5};

        float teamPosX[5] = {1.0, 1.0, 1.0, 0.33, 1.7};
        float teamPosY[5] = {0.75, 0.0, -0.75, -0.25, 0.0};

        float teamPosX[5] = {1.0, 1.0, 1.0, 0.0, 0.0};
        float teamPosY[5] = {0.35, 0.0, -0.35, -0.50, 0.50};*/

}

void RobotsFomation::loadFromFile(const QString& filename)
{
     QFile file(filename);
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return;
     QTextStream in(&file);
     int k;
     for (k=0;k<ROBOT_COUNT;k++) x[k]=y[k]=0;
     k=0;
     while (!in.atEnd()) {
         QString line = in.readLine();
         QStringList list = line.split(",");         
         if (list.count()>=2)
         {             
            x[k]=list[0].toFloat();
            y[k]=list[1].toFloat();
         }
         else if (list.count()==1)
         {
            x[k]=list[0].toFloat();
         }
         if (k==ROBOT_COUNT-1) break;
         k++;
     }     
}

void RobotsFomation::resetRobots(Robot** r,int team)
{
    float dir=-1;
    if (team==1) dir = 1;
    for (int k=0;k<ROBOT_COUNT;k++)
    {
        r[robotIndex(k,team)]->setXY(x[k]*dir,y[k]);
    }
}


