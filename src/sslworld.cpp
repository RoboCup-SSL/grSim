/*
grSim - RoboCup Small Size Soccer Robots Simulator
Copyright (C) 2011, Parsian Robotic Center (eew.aut.ac.ir/~parsian/grsim)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sslworld.h"

#include <QtGlobal>
#include <QtNetwork>

#include <QDebug>

#include "logger.h"

#include "grSim_Packet.pb.h"
#include "grSim_Commands.pb.h"
#include "grSim_Replacement.pb.h"
#include "messages_robocup_ssl_detection.pb.h"
#include "messages_robocup_ssl_geometry.pb.h"
#include "messages_robocup_ssl_refbox_log.pb.h"
#include "messages_robocup_ssl_wrapper.pb.h"


#define ROBOT_GRAY 0.4
#define WHEEL_COUNT 4

SSLWorld* _w;
dReal randn_notrig(dReal mu=0.0, dReal sigma=1.0);
dReal randn_trig(dReal mu=0.0, dReal sigma=1.0);
dReal rand0_1();

dReal fric(dReal f)
{
    if (f==-1) return dInfinity;
    return f;
}

int robotIndex(int robot,int team)
{
    if (robot >= ROBOT_COUNT) return -1;
    return robot + team*ROBOT_COUNT;
}

bool wheelCallBack(dGeomID o1,dGeomID o2,PSurface* s)
{
    //s->id2 is ground
    const dReal* r; //wheels rotation matrix
    //const dReal* p; //wheels rotation matrix
    if ((o1==s->id1) && (o2==s->id2)) {
        r=dBodyGetRotation(dGeomGetBody(o1));
        //p=dGeomGetPosition(o1);//never read
    } else if ((o1==s->id2) && (o2==s->id1)) {
        r=dBodyGetRotation(dGeomGetBody(o2));
        //p=dGeomGetPosition(o2);//never read
    } else {
        //XXX: in this case we dont have the rotation
        //     matrix, thus we must return
        return false;
    }

    s->surface.mode = dContactFDir1 | dContactMu2  | dContactApprox1 | dContactSoftCFM;
    s->surface.mu = fric(_w->cfg->robotSettings.WheelPerpendicularFriction);
    s->surface.mu2 = fric(_w->cfg->robotSettings.WheelTangentFriction);
    s->surface.soft_cfm = 0.002;

    dVector3 v={0,0,1,1};
    dVector3 axis;
    dMultiply0(axis,r,v,4,3,1);
    dReal l = sqrt(axis[0]*axis[0] + axis[1]*axis[1]);
    s->fdir1[0] = axis[0]/l;
    s->fdir1[1] = axis[1]/l;
    s->fdir1[2] = 0;
    s->fdir1[3] = 0;
    s->usefdir1 = true;
    return true;
}

bool rayCallback(dGeomID o1,dGeomID o2,PSurface* s)
{
    if (!_w->updatedCursor) return false;
    dGeomID obj;
    if (o1==_w->ray->geom) obj = o2;
    else obj = o1;
    for (int i=0;i<ROBOT_COUNT * 2;i++)
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
        dReal x,y,z;
        _w->robots[_w->ball->tag]->chassis->getBodyDirection(x,y,z);
        s->fdir1[0] = x;
        s->fdir1[1] = y;
        s->fdir1[2] = 0;
        s->fdir1[3] = 0;
        s->usefdir1 = true;
        s->surface.mode = dContactMu2 | dContactFDir1 | dContactSoftCFM;
        s->surface.mu = _w->cfg->BallFriction();
        s->surface.mu2 = 0.5;
        s->surface.soft_cfm = 0.002;
    }
    return true;
}

SSLWorld::SSLWorld(QGLWidget* parent,ConfigWidget* _cfg,RobotsFomation *form1,RobotsFomation *form2)
    : QObject(parent)
{    
    isGLEnabled = true;
    customDT = -1;    
    _w = this;
    cfg = _cfg;
    m_parent = parent;
    show3DCursor = false;
    updatedCursor = false;
    framenum = 0;
    last_dt = -1;    
    g = new CGraphics(parent);
    g->setSphereQuality(1);
    g->setViewpoint(0,-(cfg->Field_Width()+cfg->Field_Margin()*2.0f)/2.0f,3,90,-45,0);
    p = new PWorld(0.05,9.81f,g);
    ball = new PBall (0,0,0.5,cfg->BallRadius(),cfg->BallMass(), 1,0.7,0);

    ground = new PGround(cfg->Field_Rad(),cfg->Field_Length(),cfg->Field_Width(),cfg->Field_Penalty_Rad(),cfg->Field_Penalty_Line(),cfg->Field_Penalty_Point(),cfg->Field_Line_Width(),cfg->Field_Defense_Stretch(),cfg->Field_Defense_Rad(),0);
    ray = new PRay(50);
    
    // Bounding walls
    
    const double thick = cfg->Wall_Thickness();
    const double increment = cfg->Field_Margin() + cfg->Field_Referee_Margin() + thick / 2;
    const double pos_x = cfg->Field_Length() / 2.0 + increment;
    const double pos_y = cfg->Field_Width() / 2.0 + increment;
    const double pos_z = 0.0;
    const double siz_x = 2.0 * pos_x;
    const double siz_y = 2.0 * pos_y;
    const double siz_z = 0.4;
    const double tone = 1.0;
    
    walls[0] = new PFixedBox(thick/2, pos_y, pos_z,
                             siz_x, thick, siz_z,
                             tone, tone, tone);

    walls[1] = new PFixedBox(-thick/2, -pos_y, pos_z,
                             siz_x, thick, siz_z,
                             tone, tone, tone);
    
    walls[2] = new PFixedBox(pos_x, -thick/2, pos_z,
                             thick, siz_y, siz_z,
                             tone, tone, tone);

    walls[3] = new PFixedBox(-pos_x, thick/2, pos_z,
                             thick, siz_y, siz_z,
                             tone, tone, tone);
    
    // Goal walls
    
    const double gthick = cfg->Goal_Thickness();
    const double gpos_x = (cfg->Field_Length() + gthick) / 2.0 + cfg->Goal_Depth();
    const double gpos_y = (cfg->Goal_Width() + gthick) / 2.0;
    const double gpos_z = cfg->Goal_Height() / 2.0;
    const double gsiz_x = cfg->Goal_Depth() + gthick;
    const double gsiz_y = cfg->Goal_Width();
    const double gsiz_z = cfg->Goal_Height();
    const double gpos2_x = (cfg->Field_Length() + gsiz_x) / 2.0;

    walls[4] = new PFixedBox(gpos_x, 0.0, gpos_z,
                             gthick, gsiz_y, gsiz_z,
                             tone, tone, tone);
    
    walls[5] = new PFixedBox(gpos2_x, -gpos_y, gpos_z,
                             gsiz_x, gthick, gsiz_z,
                             tone, tone, tone);
    
    walls[6] = new PFixedBox(gpos2_x, gpos_y, gpos_z,
                             gsiz_x, gthick, gsiz_z,
                             tone, tone, tone);

    walls[7] = new PFixedBox(-gpos_x, 0.0, gpos_z,
                             gthick, gsiz_y, gsiz_z,
                             tone, tone, tone);
    
    walls[8] = new PFixedBox(-gpos2_x, -gpos_y, gpos_z,
                             gsiz_x, gthick, gsiz_z,
                             tone, tone, tone);
    
    walls[9] = new PFixedBox(-gpos2_x, gpos_y, gpos_z,
                             gsiz_x, gthick, gsiz_z,
                             tone, tone, tone);
    
    p->addObject(ground);
    p->addObject(ball);
    p->addObject(ray);
    for (int i=0;i<10;i++)
        p->addObject(walls[i]);
    const int wheeltexid = 4 * ROBOT_COUNT + 12 + 1 ; //37 for 6 robots


    cfg->robotSettings = cfg->blueSettings;
    for (int k=0;k<ROBOT_COUNT;k++)
        robots[k] = new Robot(p,ball,cfg,-form1->x[k],form1->y[k],ROBOT_START_Z(cfg),ROBOT_GRAY,ROBOT_GRAY,ROBOT_GRAY,k+1,wheeltexid,1);
    cfg->robotSettings = cfg->yellowSettings;
    for (int k=0;k<ROBOT_COUNT;k++)
        robots[k+ROBOT_COUNT] = new Robot(p,ball,cfg,form2->x[k],form2->y[k],ROBOT_START_Z(cfg),ROBOT_GRAY,ROBOT_GRAY,ROBOT_GRAY,k+ROBOT_COUNT+1,wheeltexid,-1);//XXX

    p->initAllObjects();

    //Surfaces

    p->createSurface(ray,ground)->callback = rayCallback;
    p->createSurface(ray,ball)->callback = rayCallback;
    for (int k=0;k<ROBOT_COUNT * 2;k++)
    {
        p->createSurface(ray,robots[k]->chassis)->callback = rayCallback;
        p->createSurface(ray,robots[k]->dummy)->callback = rayCallback;
    }
    PSurface ballwithwall;
    ballwithwall.surface.mode = dContactBounce | dContactApprox1;// | dContactSlip1;
    ballwithwall.surface.mu = 1;//fric(cfg->ballfriction());
    ballwithwall.surface.bounce = cfg->BallBounce();
    ballwithwall.surface.bounce_vel = cfg->BallBounceVel();
    ballwithwall.surface.slip1 = 0;//cfg->ballslip();

    PSurface wheelswithground;
    PSurface* ball_ground = p->createSurface(ball,ground);
    ball_ground->surface = ballwithwall.surface;
    ball_ground->callback = ballCallBack;

    PSurface ballwithkicker;
    ballwithkicker.surface.mode = dContactApprox1;
    ballwithkicker.surface.mu = fric(cfg->robotSettings.Kicker_Friction);
    ballwithkicker.surface.slip1 = 5;
    
    for (int i = 0; i < WALL_COUNT; i++)
        p->createSurface(ball, walls[i])->surface = ballwithwall.surface;
    
    for (int k = 0; k < 2 * ROBOT_COUNT; k++)
    {
        p->createSurface(robots[k]->chassis,ground);
        for (int j = 0; j < WALL_COUNT; j++)
            p->createSurface(robots[k]->chassis,walls[j]);
        p->createSurface(robots[k]->dummy,ball);
        //p->createSurface(robots[k]->chassis,ball);
        p->createSurface(robots[k]->kicker->box,ball)->surface = ballwithkicker.surface;
        for (int j = 0; j < WHEEL_COUNT; j++)
        {
            p->createSurface(robots[k]->wheels[j]->cyl,ball);
            PSurface* w_g = p->createSurface(robots[k]->wheels[j]->cyl,ground);
            w_g->surface=wheelswithground.surface;
            w_g->usefdir1=true;
            w_g->callback=wheelCallBack;
        }
        for (int j = k + 1; j < 2 * ROBOT_COUNT; j++)
        {            
            if (k != j)
            {
                p->createSurface(robots[k]->dummy,robots[j]->dummy); //seams ode doesn't understand cylinder-cylinder contacts, so I used spheres
                p->createSurface(robots[k]->chassis,robots[j]->kicker->box);
            }
        }
    }
    sendGeomCount = 0;
    timer = new QTime();
    timer->start();
    in_buffer = new char [65536];
}

SSLWorld::~SSLWorld()
{
    delete g;
    delete p;
}

QImage* createBlob(char yb,int i,QImage** res)
{
    *res = new QImage(QString(":/%1%2").arg(yb).arg(i)+QString(".png"));
    return *res;
}

QImage* createNumber(int i,int r,int g,int b,int a)
{
    QImage* img = new QImage(32,32,QImage::Format_ARGB32);
    QPainter *p = new QPainter();
    QBrush br;
    p->begin(img);
    QColor black(0,0,0,0);
    for (int i=0;i<img->width();i++)
        for (int j=0;j<img->height();j++)
        {
            img->setPixel(i,j,black.rgba());
        }
    QColor txtcolor(r,g,b,a);
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(3);
    pen.setBrush(txtcolor);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p->setPen(pen);
    QFont f;
    f.setBold(true);
    f.setPointSize(26);
    p->setFont(f);
    p->drawText(img->width()/2-15,img->height()/2-15,30,30,Qt::AlignCenter,QString("%1").arg(i));
    p->end();
    delete p;
    return img;
}


void SSLWorld::glinit()
{
    g->loadTexture(new QImage(":/grass.png"));

    // Loading Robot textures for each robot
    for (int i = 0; i < ROBOT_COUNT; i++)
        g->loadTexture(createBlob('b', i, &robots[i]->img));

    for (int i = 0; i < ROBOT_COUNT; i++)
        g->loadTexture(createBlob('y', i, &robots[ROBOT_COUNT + i]->img));

    // Creating number textures
    for (int i=0; i<ROBOT_COUNT;i++)
        g->loadTexture(createNumber(i,15,193,225,255));

    for (int i=0; i<ROBOT_COUNT;i++)
        g->loadTexture(createNumber(i,0xff,0xff,0,255));

    // Loading sky textures
    // XXX: for some reason they are loaded twice otherwise the wheel texture is wrong
    for (int i=0; i<6; i++) {
        g->loadTexture(new QImage(QString(":/sky/neg_%1").arg(i%3==0?'x':i%3==1?'y':'z')+QString(".png")));
        g->loadTexture(new QImage(QString(":/sky/pos_%1").arg(i%3==0?'x':i%3==1?'y':'z')+QString(".png")));
    }

    // The wheel texture
    g->loadTexture(new QImage(":/wheel.png"));

    // Init at last
    p->glinit();
}

void SSLWorld::step(dReal dt)
{
    if (!isGLEnabled) g->disableGraphics();
    else g->enableGraphics();

    if (customDT > 0)
        dt = customDT;
    g->initScene(m_parent->width(),m_parent->height(),0,0.7,1);//true,0.7,0.7,0.7,0.8);
    for (int kk=0;kk<5;kk++)
    {
        const dReal* ballvel = dBodyGetLinearVel(ball->body);
        dReal ballspeed = ballvel[0]*ballvel[0] + ballvel[1]*ballvel[1] + ballvel[2]*ballvel[2];
        ballspeed = sqrt(ballspeed);
        dReal ballfx=0,ballfy=0,ballfz=0;
        dReal balltx=0,ballty=0,balltz=0;
        if (ballspeed<0.01)
        {
            ;//const dReal* ballAngVel = dBodyGetAngularVel(ball->body);
            //TODO: what was supposed to be here?
        }
        else {
            dReal fk = cfg->BallFriction()*cfg->BallMass()*cfg->Gravity();
            ballfx = -fk*ballvel[0] / ballspeed;
            ballfy = -fk*ballvel[1] / ballspeed;
            ballfz = -fk*ballvel[2] / ballspeed;
            balltx = -ballfy*cfg->BallRadius();
            ballty = ballfx*cfg->BallRadius();
            balltz = 0;
            dBodyAddTorque(ball->body,balltx,ballty,balltz);
        }
        dBodyAddForce(ball->body,ballfx,ballfy,ballfz);
        if (dt==0) dt=last_dt;
        else last_dt = dt;

        selected = -1;
        p->step(dt*0.2);
    }


    int best_k=-1;
    dReal best_dist = 1e8;
    dReal xyz[3],hpr[3];
    if (selected==-2) {
        best_k=-2;
        dReal bx,by,bz;
        ball->getBodyPosition(bx,by,bz);
        g->getViewpoint(xyz,hpr);
        best_dist  =(bx-xyz[0])*(bx-xyz[0])
                +(by-xyz[1])*(by-xyz[1])
                +(bz-xyz[2])*(bz-xyz[2]);
    }
    for (int k=0;k<ROBOT_COUNT * 2;k++)
    {
        if (robots[k]->selected)
        {
            g->getViewpoint(xyz,hpr);
            dReal dist= (robots[k]->select_x-xyz[0])*(robots[k]->select_x-xyz[0])
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
    for (int k=0;k<ROBOT_COUNT * 2;k++)
    {
        robots[k]->step();
        robots[k]->selected = false;
    }
    p->draw();
    //g->drawSkybox(31,32,33,34,35,36);
    g->drawSkybox(4 * ROBOT_COUNT + 6 + 1, //31 for 6 robot
                  4 * ROBOT_COUNT + 6 + 2, //32 for 6 robot
                  4 * ROBOT_COUNT + 6 + 3, //33 for 6 robot
                  4 * ROBOT_COUNT + 6 + 4, //34 for 6 robot
                  4 * ROBOT_COUNT + 6 + 5, //31 for 6 robot
                  4 * ROBOT_COUNT + 6 + 6);//36 for 6 robot

    dMatrix3 R;

    if (g->isGraphicsEnabled())
        if (show3DCursor)
        {
            dRFromAxisAndAngle(R,0,0,1,0);
            g->setColor(1,0.9,0.2,0.5);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            g->drawCircle(cursor_x,cursor_y,0.001,cursor_radius);
            glDisable(GL_BLEND);
        }
    //for (int k=0;k<10;k++) robots[k]->drawLabel();

    g->finalizeScene();


    sendVisionBuffer();
    framenum ++;
}


void SSLWorld::recvActions()
{
    QHostAddress sender;
    quint16 port;
    grSim_Packet packet;
    while (commandSocket->hasPendingDatagrams())
    {
        int size = commandSocket->readDatagram(in_buffer, 65536, &sender, &port);
        if (size > 0)
        {
            packet.ParseFromArray(in_buffer, size);
            int team=0;
            if (packet.has_commands())
            {
                if (packet.commands().has_isteamyellow())
                {
                    if (packet.commands().isteamyellow()) team=1;
                }
                for (int i=0;i<packet.commands().robot_commands_size();i++)
                {
                    if (!packet.commands().robot_commands(i).has_id()) continue;
                    int k = packet.commands().robot_commands(i).id();
                    int id = robotIndex(k, team);
                    if ((id < 0) || (id >= ROBOT_COUNT*2)) continue;
                    bool wheels = false;
                    if (packet.commands().robot_commands(i).has_wheelsspeed())
                    {
                        if (packet.commands().robot_commands(i).wheelsspeed()==true)
                        {
                            if (packet.commands().robot_commands(i).has_wheel1()) robots[id]->setSpeed(0, packet.commands().robot_commands(i).wheel1());
                            if (packet.commands().robot_commands(i).has_wheel2()) robots[id]->setSpeed(1, packet.commands().robot_commands(i).wheel2());
                            if (packet.commands().robot_commands(i).has_wheel3()) robots[id]->setSpeed(2, packet.commands().robot_commands(i).wheel3());
                            if (packet.commands().robot_commands(i).has_wheel4()) robots[id]->setSpeed(3, packet.commands().robot_commands(i).wheel4());
                            wheels = true;
                        }
                    }
                    if (!wheels)
                    {
                        dReal vx = 0;if (packet.commands().robot_commands(i).has_veltangent()) vx = packet.commands().robot_commands(i).veltangent();
                        dReal vy = 0;if (packet.commands().robot_commands(i).has_velnormal())  vy = packet.commands().robot_commands(i).velnormal();
                        dReal vw = 0;if (packet.commands().robot_commands(i).has_velangular()) vw = packet.commands().robot_commands(i).velangular();
                        robots[id]->setSpeed(vx, vy, vw);
                    }
                    dReal kickx = 0 , kickz = 0;
                    bool kick = false;
                    if (packet.commands().robot_commands(i).has_kickspeedx())
                    {
                        kick = true;
                        kickx = packet.commands().robot_commands(i).kickspeedx();
                    }
                    if (packet.commands().robot_commands(i).has_kickspeedz())
                    {
                        kick = true;
                        kickz = packet.commands().robot_commands(i).kickspeedz();
                    }
                    if (kick && ((kickx>0.0001) || (kickz>0.0001)))
                        robots[id]->kicker->kick(kickx,kickz);
                    int rolling = 0;
                    if (packet.commands().robot_commands(i).has_spinner())
                    {
                        if (packet.commands().robot_commands(i).spinner()) rolling = 1;
                    }
                    robots[id]->kicker->setRoller(rolling);

                    char status = 0;
                    status = k;
                    if (robots[id]->kicker->isTouchingBall()) status = status | 8;
                    if (robots[id]->on) status = status | 240;
                    if (team == 0)
                        blueStatusSocket->writeDatagram(&status,1,sender,cfg->BlueStatusSendPort());
                    else
                        yellowStatusSocket->writeDatagram(&status,1,sender,cfg->YellowStatusSendPort());

                }
            }
            if (packet.has_replacement())
            {
                for (int i=0;i<packet.replacement().robots_size();i++)
                {
                    int team = 0;
                    if (packet.replacement().robots(i).has_yellowteam())
                    {
                        if (packet.replacement().robots(i).yellowteam())
                            team = 1;
                    }
                    if (!packet.replacement().robots(i).has_id()) continue;
                    int k = packet.replacement().robots(i).id();
                    dReal x = 0, y = 0, dir = 0;
                    bool turnon = true;
                    if (packet.replacement().robots(i).has_x()) x = packet.replacement().robots(i).x();
                    if (packet.replacement().robots(i).has_y()) y = packet.replacement().robots(i).y();
                    if (packet.replacement().robots(i).has_dir()) dir = packet.replacement().robots(i).dir();
                    if (packet.replacement().robots(i).has_turnon()) turnon = packet.replacement().robots(i).turnon();
                    int id = robotIndex(k, team);
                    if ((id < 0) || (id >= ROBOT_COUNT*2)) continue;
                    robots[id]->setXY(x,y);
                    robots[id]->resetRobot();
                    robots[id]->setDir(dir);
                    robots[id]->on = turnon;
                }
                if (packet.replacement().has_ball())
                {
                    dReal x = 0, y = 0, vx = 0, vy = 0;
                    if (packet.replacement().ball().has_x())  x  = packet.replacement().ball().x();
                    if (packet.replacement().ball().has_y())  y  = packet.replacement().ball().y();
                    if (packet.replacement().ball().has_vx()) vx = packet.replacement().ball().vx();
                    if (packet.replacement().ball().has_vy()) vy = packet.replacement().ball().vy();
                    ball->setBodyPosition(x,y,cfg->BallRadius()*1.2);
                    dBodySetLinearVel(ball->body,vx,vy,0);
                    dBodySetAngularVel(ball->body,0,0,0);
                }
            }
        }
    }
}

dReal normalizeAngle(dReal a)
{
    if (a>180) return -360+a;
    if (a<-180) return 360+a;
    return a;
}

bool SSLWorld::visibleInCam(int id, double x, double y)
{
    id %= 4;
    if (id==0)
    {
        if (x>-0.2 && y>-0.2) return true;
    }
    if (id==1)
    {
        if (x>-0.2 && y<0.2) return true;
    }
    if (id==2)
    {
        if (x<0.2 && y<0.2) return true;
    }
    if (id==3)
    {
        if (x<0.2 && y>-0.2) return true;
    }
    return false;
}

#define CONVUNIT(x) ((int)(1000*(x)))
SSL_WrapperPacket* SSLWorld::generatePacket(int cam_id)
{
    SSL_WrapperPacket* packet = new SSL_WrapperPacket;
    dReal x,y,z,dir;
    ball->getBodyPosition(x,y,z);    
    packet->mutable_detection()->set_camera_id(cam_id);
    packet->mutable_detection()->set_frame_number(framenum);    
    dReal t_elapsed = timer->elapsed()/1000.0;
    packet->mutable_detection()->set_t_capture(t_elapsed);
    packet->mutable_detection()->set_t_sent(t_elapsed);
    dReal dev_x = cfg->noiseDeviation_x();
    dReal dev_y = cfg->noiseDeviation_y();
    dReal dev_a = cfg->noiseDeviation_angle();
    if (sendGeomCount++ % cfg->sendGeometryEvery() == 0)
    {
        SSL_GeometryData* geom = packet->mutable_geometry();
        SSL_GeometryFieldSize* field = geom->mutable_field();


        // Old protocol
//        field->set_line_width(CONVUNIT(cfg->Field_Line_Width()));
//        field->set_referee_width(CONVUNIT(cfg->Field_Referee_Margin()));
//        field->set_goal_wall_width(CONVUNIT(cfg->Goal_Thickness()));
//        field->set_center_circle_radius(CONVUNIT(cfg->Field_Rad()));
//        field->set_defense_radius(CONVUNIT(cfg->Field_Defense_Rad()));
//        field->set_defense_stretch(CONVUNIT(cfg->Field_Defense_Stretch()));
//        field->set_free_kick_from_defense_dist(CONVUNIT(cfg->Field_Free_Kick()));
        //TODO: verify if these fields are correct:
//        field->set_penalty_line_from_spot_dist(CONVUNIT(cfg->Field_Penalty_Line()));
//        field->set_penalty_spot_from_field_line_dist(CONVUNIT(cfg->Field_Penalty_Point()));

        // Current protocol (2015+)
        // Field general info
        field->set_field_length(CONVUNIT(cfg->Field_Length()));
        field->set_field_width(CONVUNIT(cfg->Field_Width()));
        field->set_boundary_width(CONVUNIT(cfg->Field_Margin()));
        field->set_goal_width(CONVUNIT(cfg->Goal_Width()));
        field->set_goal_depth(CONVUNIT(cfg->Goal_Depth()));

        // Field lines and arcs
        addFieldLinesArcs(field);

    }
    if (cfg->noise()==false) {dev_x = 0;dev_y = 0;dev_a = 0;}
    if ((cfg->vanishing()==false) || (rand0_1() > cfg->ball_vanishing()))
    {
        if (visibleInCam(cam_id, x, y)) {
            SSL_DetectionBall* vball = packet->mutable_detection()->add_balls();
            vball->set_x(randn_notrig(x*1000.0f,dev_x));
            vball->set_y(randn_notrig(y*1000.0f,dev_y));
            vball->set_z(z*1000.0f);
            vball->set_pixel_x(x*1000.0f);
            vball->set_pixel_y(y*1000.0f);
            vball->set_confidence(0.9 + rand0_1()*0.1);
        }
    }
    for(int i = 0; i < ROBOT_COUNT; i++){
        if ((cfg->vanishing()==false) || (rand0_1() > cfg->blue_team_vanishing()))
        {
            if (!robots[i]->on) continue;
            robots[i]->getXY(x,y);
            dir = robots[i]->getDir();
            if (visibleInCam(cam_id, x, y)) {
                SSL_DetectionRobot* rob = packet->mutable_detection()->add_robots_blue();
                rob->set_robot_id(i);
                rob->set_pixel_x(x*1000.0f);
                rob->set_pixel_y(y*1000.0f);
                rob->set_confidence(1);
                rob->set_x(randn_notrig(x*1000.0f,dev_x));
                rob->set_y(randn_notrig(y*1000.0f,dev_y));
                rob->set_orientation(normalizeAngle(randn_notrig(dir,dev_a))*M_PI/180.0f);
            }
        }
    }
    for(int i = ROBOT_COUNT; i < ROBOT_COUNT*2; i++){
        if ((cfg->vanishing()==false) || (rand0_1() > cfg->yellow_team_vanishing()))
        {
            if (!robots[i]->on) continue;
            robots[i]->getXY(x,y);
            dir = robots[i]->getDir();
            if (visibleInCam(cam_id, x, y)) {
                SSL_DetectionRobot* rob = packet->mutable_detection()->add_robots_yellow();
                rob->set_robot_id(i-ROBOT_COUNT);
                rob->set_pixel_x(x*1000.0f);
                rob->set_pixel_y(y*1000.0f);
                rob->set_confidence(1);
                rob->set_x(randn_notrig(x*1000.0f,dev_x));
                rob->set_y(randn_notrig(y*1000.0f,dev_y));
                rob->set_orientation(normalizeAngle(randn_notrig(dir,dev_a))*M_PI/180.0f);
            }
        }
    }
    return packet;
}

void SSLWorld::addFieldLinesArcs(SSL_GeometryFieldSize *field) {
    const double kFieldLength = CONVUNIT(cfg->Field_Length());
    const double kFieldWidth = CONVUNIT(cfg->Field_Width());
    const double kGoalWidth = CONVUNIT(cfg->Goal_Width());
    const double kGoalDepth = CONVUNIT(cfg->Goal_Depth());
    const double kBoundaryWidth = CONVUNIT(cfg->Field_Referee_Margin());
    const double kCenterRadius = CONVUNIT(cfg->Field_Rad());
    const double kDefenseRadius = CONVUNIT(cfg->Field_Defense_Rad());
    const double kDefenseStretch = CONVUNIT(cfg->Field_Defense_Stretch());
    const double kLineThickness = CONVUNIT(cfg->Field_Line_Width());

    const double kXMax = (kFieldLength-2*kLineThickness)/2;
    const double kXMin = -kXMax;
    const double kYMax = (kFieldWidth-kLineThickness)/2;
    const double kYMin = -kYMax;

    // Field lines
    addFieldLine(field, "TopTouchLine", kXMin-kLineThickness/2, kYMax, kXMax+kLineThickness/2, kYMax, kLineThickness);
    addFieldLine(field, "BottomTouchLine", kXMin-kLineThickness/2, kYMin, kXMax+kLineThickness/2, kYMin, kLineThickness);
    addFieldLine(field, "LeftGoalLine", kXMin, kYMin, kXMin, kYMax, kLineThickness);
    addFieldLine(field, "RightGoalLine", kXMax, kYMin, kXMax, kYMax, kLineThickness);
    addFieldLine(field, "HalfwayLine", 0, kYMin, 0, kYMax, kLineThickness);
    addFieldLine(field, "CenterLine", kXMin, 0, kXMax, 0, kLineThickness);
    addFieldLine(field, "LeftPenaltyStretch", kXMin+kDefenseRadius-kLineThickness/2, -kDefenseStretch/2, kXMin+kDefenseRadius-kLineThickness/2, kDefenseStretch/2, kLineThickness);
    addFieldLine(field, "RightPenaltyStretch", kXMax-kDefenseRadius+kLineThickness/2, -kDefenseStretch/2, kXMax-kDefenseRadius+kLineThickness/2, kDefenseStretch/2, kLineThickness);

    addFieldLine(field, "RightGoalTopLine", kXMax, kGoalWidth/2, kXMax+kGoalDepth, kGoalWidth/2, kLineThickness);
    addFieldLine(field, "RightGoalBottomLine", kXMax, -kGoalWidth/2, kXMax+kGoalDepth, -kGoalWidth/2, kLineThickness);
    addFieldLine(field, "RightGoalDepthLine", kXMax+kGoalDepth-kLineThickness/2, -kGoalWidth/2, kXMax+kGoalDepth-kLineThickness/2, kGoalWidth/2, kLineThickness);

    addFieldLine(field, "LeftGoalTopLine", -kXMax, kGoalWidth/2, -kXMax-kGoalDepth, kGoalWidth/2, kLineThickness);
    addFieldLine(field, "LeftGoalBottomLine", -kXMax, -kGoalWidth/2, -kXMax-kGoalDepth, -kGoalWidth/2, kLineThickness);
    addFieldLine(field, "LeftGoalDepthLine", -kXMax-kGoalDepth+kLineThickness/2, -kGoalWidth/2, -kXMax-kGoalDepth+kLineThickness/2, kGoalWidth/2, kLineThickness);

    // Field arcs
    addFieldArc(field, "LeftFieldLeftPenaltyArc",   kXMin, kDefenseStretch/2,  kDefenseRadius-kLineThickness/2, 0,        M_PI/2,   kLineThickness);
    addFieldArc(field, "LeftFieldRightPenaltyArc",  kXMin, -kDefenseStretch/2, kDefenseRadius-kLineThickness/2, 1.5*M_PI, 2*M_PI,   kLineThickness);
    addFieldArc(field, "RightFieldLeftPenaltyArc",  kXMax, -kDefenseStretch/2, kDefenseRadius-kLineThickness/2, M_PI,     1.5*M_PI, kLineThickness);
    addFieldArc(field, "RightFieldRightPenaltyArc", kXMax, kDefenseStretch/2,  kDefenseRadius-kLineThickness/2, M_PI/2,   M_PI,     kLineThickness);
    addFieldArc(field, "CenterCircle",              0,     0,                  kCenterRadius-kLineThickness/2,  0,        2*M_PI,   kLineThickness);
}

Vector2f* SSLWorld::allocVector(float x, float y) {
    Vector2f *vec = new Vector2f();
    vec->set_x(x);
    vec->set_y(y);
    return vec;
}

void SSLWorld::addFieldLine(SSL_GeometryFieldSize *field, const std::string &name, float p1_x, float p1_y, float p2_x, float p2_y, float thickness) {
    SSL_FieldLineSegment *line = field->add_field_lines();
    line->set_name(name.c_str());
	line->mutable_p1()->set_x(p1_x);
	line->mutable_p1()->set_y(p1_y);
	line->mutable_p2()->set_x(p2_x);
	line->mutable_p2()->set_y(p2_y);
    line->set_thickness(thickness);
}

void SSLWorld::addFieldArc(SSL_GeometryFieldSize *field, const std::string &name, float c_x, float c_y, float radius, float a1, float a2, float thickness) {
    SSL_FieldCicularArc *arc = field->add_field_arcs();
	arc->set_name(name.c_str());
	arc->mutable_center()->set_x(c_x);
	arc->mutable_center()->set_y(c_y);
    arc->set_radius(radius);
    arc->set_a1(a1);
    arc->set_a2(a2);
    arc->set_thickness(thickness);
}

SendingPacket::SendingPacket(SSL_WrapperPacket* _packet,int _t)
{
    packet = _packet;
    t      = _t;
}

void SSLWorld::sendVisionBuffer()
{
    int t = timer->elapsed();
    sendQueue.push_back(new SendingPacket(generatePacket(0),t));    
    sendQueue.push_back(new SendingPacket(generatePacket(1),t+1));
    sendQueue.push_back(new SendingPacket(generatePacket(2),t+2));
    sendQueue.push_back(new SendingPacket(generatePacket(3),t+3));
    while (t - sendQueue.front()->t>=cfg->sendDelay())
    {
        SSL_WrapperPacket *packet = sendQueue.front()->packet;
        delete sendQueue.front();
        sendQueue.pop_front();
        visionServer->send(*packet);
        delete packet;
        if (sendQueue.isEmpty()) break;
    }
}

void RobotsFomation::setAll(dReal* xx,dReal *yy)
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
        dReal teamPosX[MAX_ROBOT_COUNT] = {2.2, 1.0, 1.0, 1.0, 0.33, 1.22,
                                           3, 3.2, 3.4, 3.6, 3.8, 4.0};
        dReal teamPosY[MAX_ROBOT_COUNT] = {0.0, -0.75, 0.0, 0.75, 0.25, 0.0,
                                           1, 1, 1, 1, 1, 1};
        setAll(teamPosX,teamPosY);
    }
    if (type==1) // formation 1
    {
        dReal teamPosX[MAX_ROBOT_COUNT] = {1.5, 1.5, 1.5, 0.55, 2.5, 3.6,
                                               3.2, 3.2, 3.2, 3.2, 3.2, 3.2};
        dReal teamPosY[MAX_ROBOT_COUNT] = {1.12, 0.0, -1.12, 0.0, 0.0, 0.0,
                                               0.75, -0.75, 1.5, -1.5, 2.25, -2.25};
        setAll(teamPosX,teamPosY);
    }
    if (type==2) // formation 2
    {
        dReal teamPosX[MAX_ROBOT_COUNT] = {4.2, 3.40,  3.40, 0.7, 0.7,  0.7,
                                               2, 2, 2, 2, 2, 2};
        dReal teamPosY[MAX_ROBOT_COUNT] = {0.0, -0.20, 0.20, 0.0, 2.25, -2.25,
                                               0.75, -0.75, 1.5, -1.5, 2.25, -2.25};
        setAll(teamPosX,teamPosY);
    }
    if (type==3) // outside field
    {
        dReal teamPosX[MAX_ROBOT_COUNT] = {0.4,  0.8,  1.2,  1.6,  2.0,  2.4,
                                           2.8, 3.2, 3.6, 4.0, 4.4, 4.8};
        dReal teamPosY[MAX_ROBOT_COUNT] = {-4.0, -4.0, -4.0, -4.0, -4.0, -4.0,
                                           -4.0, -4.0, -4.0, -4.0, -4.0, -4.0};
        setAll(teamPosX,teamPosY);
    }
    if (type==4)
    {
        dReal teamPosX[MAX_ROBOT_COUNT] = {2.8, 2.5, 2.5, 0.8, 0.8, 1.1, 3, 3.2, 3.4, 3.6, 3.8, 4.0};
        dReal teamPosY[MAX_ROBOT_COUNT] = {5+0.0, 5-0.3, 5+0.3, 5+0.0, 5+1.5, 5.5, 1, 1, 1, 1, 1, 1};
        setAll(teamPosX,teamPosY);
    }
    if (type==-1) // outside
    {
        dReal teamPosX[MAX_ROBOT_COUNT] = {0.4,  0.8,  1.2,  1.6,  2.0,  2.4,
                                           2.8, 3.2, 3.6, 4.0, 4.4, 4.8};
        dReal teamPosY[MAX_ROBOT_COUNT] = {-3.4, -3.4, -3.4, -3.4, -3.4, -3.4,
                                           -3.4, -3.4, -3.4, -3.4, -3.4, -3.4};
        setAll(teamPosX,teamPosY);
    }

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
    dReal dir=-1;
    if (team==1) dir = 1;
    for (int k=0;k<ROBOT_COUNT;k++)
    {
        r[robotIndex(k,team)]->setXY(x[k]*dir,y[k]);
        r[robotIndex(k,team)]->resetRobot();
    }
}




//// Copy & pasted from http://www.dreamincode.net/code/snippet1446.htm
/******************************************************************************/
/* randn()
 *
 * Normally (Gaussian) distributed random numbers, using the Box-Muller
 * transformation.  This transformation takes two uniformly distributed deviates
 * within the unit circle, and transforms them into two independently
 * distributed normal deviates.  Utilizes the internal rand() function; this can
 * easily be changed to use a better and faster RNG.
 *
 * The parameters passed to the function are the mean and standard deviation of
 * the desired distribution.  The default values used, when no arguments are
 * passed, are 0 and 1 - the standard normal distribution.
 *
 *
 * Two functions are provided:
 *
 * The first uses the so-called polar version of the B-M transformation, using
 * multiple calls to a uniform RNG to ensure the initial deviates are within the
 * unit circle.  This avoids making any costly trigonometric function calls.
 *
 * The second makes only a single set of calls to the RNG, and calculates a
 * position within the unit circle with two trigonometric function calls.
 *
 * The polar version is generally superior in terms of speed; however, on some
 * systems, the optimization of the math libraries may result in better
 * performance of the second.  Try it out on the target system to see which
 * works best for you.  On my test machine (Athlon 3800+), the non-trig version
 * runs at about 3x10^6 calls/s; while the trig version runs at about
 * 1.8x10^6 calls/s (-O2 optimization).
 *
 *
 * Example calls:
 * randn_notrig();	//returns normal deviate with mean=0.0, std. deviation=1.0
 * randn_notrig(5.2,3.0);	//returns deviate with mean=5.2, std. deviation=3.0
 *
 *
 * Dependencies - requires <cmath> for the sqrt(), sin(), and cos() calls, and a
 * #defined value for PI.
 */

/******************************************************************************/
//	"Polar" version without trigonometric calls
dReal randn_notrig(dReal mu, dReal sigma) {
    if (sigma==0) return mu;
    static bool deviateAvailable=false;	//	flag
    static dReal storedDeviate;			//	deviate from previous calculation
    dReal polar, rsquared, var1, var2;

    //	If no deviate has been stored, the polar Box-Muller transformation is
    //	performed, producing two independent normally-distributed random
    //	deviates.  One is stored for the next round, and one is returned.
    if (!deviateAvailable) {

        //	choose pairs of uniformly distributed deviates, discarding those
        //	that don't fall within the unit circle
        do {
            var1=2.0*( dReal(rand())/dReal(RAND_MAX) ) - 1.0;
            var2=2.0*( dReal(rand())/dReal(RAND_MAX) ) - 1.0;
            rsquared=var1*var1+var2*var2;
        } while ( rsquared>=1.0 || rsquared == 0.0);

        //	calculate polar tranformation for each deviate
        polar=sqrt(-2.0*log(rsquared)/rsquared);

        //	store first deviate and set flag
        storedDeviate=var1*polar;
        deviateAvailable=true;

        //	return second deviate
        return var2*polar*sigma + mu;
    }

    //	If a deviate is available from a previous call to this function, it is
    //	returned, and the flag is set to false.
    else {
        deviateAvailable=false;
        return storedDeviate*sigma + mu;
    }
}


/******************************************************************************/
//	Standard version with trigonometric calls
#define PI 3.14159265358979323846

dReal randn_trig(dReal mu, dReal sigma) {
    static bool deviateAvailable=false;	//	flag
    static dReal storedDeviate;			//	deviate from previous calculation
    dReal dist, angle;

    //	If no deviate has been stored, the standard Box-Muller transformation is
    //	performed, producing two independent normally-distributed random
    //	deviates.  One is stored for the next round, and one is returned.
    if (!deviateAvailable) {

        //	choose a pair of uniformly distributed deviates, one for the
        //	distance and one for the angle, and perform transformations
        dist=sqrt( -2.0 * log(dReal(rand()) / dReal(RAND_MAX)) );
        angle=2.0 * PI * (dReal(rand()) / dReal(RAND_MAX));

        //	calculate and store first deviate and set flag
        storedDeviate=dist*cos(angle);
        deviateAvailable=true;

        //	calcaulate return second deviate
        return dist * sin(angle) * sigma + mu;
    }

    //	If a deviate is available from a previous call to this function, it is
    //	returned, and the flag is set to false.
    else {
        deviateAvailable=false;
        return storedDeviate*sigma + mu;
    }
}

dReal rand0_1()
{
    return (dReal) (rand()) / (dReal) (RAND_MAX);
}
