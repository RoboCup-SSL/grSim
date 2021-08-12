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
#include "ssl_vision_detection.pb.h"
#include "ssl_vision_geometry.pb.h"
#include "ssl_vision_wrapper.pb.h"


#define ROBOT_GRAY .8
#define WHEEL_COUNT 4

QColor ROBOT_BLUE_CHASSIS_COLOR {QColor("#0000ff")};
QColor ROBOT_YELLOW_CHASSIS_COLOR {QColor("#ffff00")};

SSLWorld* _w;
dReal randn_notrig(dReal mu=0.0, dReal sigma=1.0);
dReal randn_trig(dReal mu=0.0, dReal sigma=1.0);
dReal rand0_1();

dReal fric(dReal f)
{
    if (f==-1) return dInfinity;
    return f;
}

bool wheelCallBack(dGeomID o1,dGeomID o2,PSurface* s, int /*robots_count*/)
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

bool rayCallback(dGeomID o1,dGeomID o2,PSurface* s, int robots_count)
{
    if (!_w->updatedCursor) return false;
    dGeomID obj;
    if (o1==_w->ray->geom) obj = o2;
    else obj = o1;
    for (int i=0;i<robots_count * 2;i++)
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

bool ballCallBack(dGeomID o1,dGeomID o2,PSurface* s, int /*robots_count*/)
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

SSLWorld::SSLWorld(QGLWidget* parent, ConfigWidget* _cfg, RobotsFormation *form1, RobotsFormation *form2) : QObject(parent) {
    isGLEnabled = true;
    customDT = -1;    
    _w = this;
    cfg = _cfg;
    m_parent = parent;
    show3DCursor = false;
    updatedCursor = false;
    frame_num = 0;
    last_dt = -1;    
    g = new CGraphics(parent);
    g->setSphereQuality(1);
    g->setViewpoint(0,-(cfg->Field_Width()+cfg->Field_Margin()*2.0f)/2.0f,3,90,-45,0);
    p = new PWorld(0.05,9.81f,g,cfg->Robots_Count());
    ball = new PBall (0,0,0.5,cfg->BallRadius(),cfg->BallMass(), 1,0.7,0);

    ground = new PGround(cfg->Field_Rad(),cfg->Field_Length(),cfg->Field_Width(),cfg->Field_Penalty_Depth(),cfg->Field_Penalty_Width(),cfg->Field_Penalty_Point(),cfg->Field_Line_Width(),0);
    ray = new PRay(50);
    
    ROBOT_BLUE_CHASSIS_COLOR = QColor(QString::fromStdString(_cfg->v_ColorRobotBlue->getString()));
    ROBOT_YELLOW_CHASSIS_COLOR = QColor(QString::fromStdString(_cfg->v_ColorRobotYellow->getString()));

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
    for (auto & wall : walls) p->addObject(wall);
    const int wheeltexid = 4 * cfg->Robots_Count() + 12 + 1 ; //37 for 6 robots


    cfg->robotSettings = cfg->blueSettings;
    for (int k=0;k<cfg->Robots_Count();k++) {
        float a1 = -form1->x[k];
        float a2 = form1->y[k];
        float a3 = ROBOT_START_Z(cfg);
        robots[k] = new Robot(p,
                              ball,
                              cfg,
                              -form1->x[k],
                              form1->y[k],
                              ROBOT_START_Z(cfg),
                              ROBOT_GRAY,
                              ROBOT_GRAY,
                              ROBOT_GRAY,
                              k + 1,
                              wheeltexid,
                              1);
    }
    cfg->robotSettings = cfg->yellowSettings;
    for (int k=0;k<cfg->Robots_Count();k++)
        robots[k+cfg->Robots_Count()] = new Robot(p,ball,cfg,form2->x[k],form2->y[k],ROBOT_START_Z(cfg),ROBOT_GRAY,ROBOT_GRAY,ROBOT_GRAY,k+cfg->Robots_Count()+1,wheeltexid,-1);//XXX

    p->initAllObjects();

    //Surfaces

    p->createSurface(ray,ground)->callback = rayCallback;
    p->createSurface(ray,ball)->callback = rayCallback;
    for (int k=0;k<cfg->Robots_Count() * 2;k++)
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
    
    for (auto & wall : walls) p->createSurface(ball, wall)->surface = ballwithwall.surface;
    
    for (int k = 0; k < 2 * cfg->Robots_Count(); k++)
    {
        p->createSurface(robots[k]->chassis,ground);
        for (auto & wall : walls) p->createSurface(robots[k]->chassis,wall);
        p->createSurface(robots[k]->dummy,ball);
        //p->createSurface(robots[k]->chassis,ball);
        p->createSurface(robots[k]->kicker->box,ball)->surface = ballwithkicker.surface;
        for (auto & wheel : robots[k]->wheels)
        {
            p->createSurface(wheel->cyl,ball);
            PSurface* w_g = p->createSurface(wheel->cyl,ground);
            w_g->surface=wheelswithground.surface;
            w_g->usefdir1=true;
            w_g->callback=wheelCallBack;
        }
        for (int j = k + 1; j < 2 * cfg->Robots_Count(); j++)
        {            
            if (k != j)
            {
                p->createSurface(robots[k]->dummy,robots[j]->dummy); //seams ode doesn't understand cylinder-cylinder contacts, so I used spheres
                p->createSurface(robots[k]->chassis,robots[j]->kicker->box);
            }
        }
    }
    sendGeomCount = 0;

    // initialize robot state
    for (int team = 0; team < 2; ++team)
    {
        for (int i = 0; i < MAX_ROBOT_COUNT; ++i)
        {
            lastInfraredState[team][i] = false;
            lastKickState[team][i] = NO_KICK; 
        }
    }

    restartRequired = false;
}

int SSLWorld::robotIndex(int robot,int team) {
    if (robot >= cfg->Robots_Count()) return -1;
    return robot + team*cfg->Robots_Count();
}

SSLWorld::~SSLWorld() {
    delete g;
    delete p;
}

QImage* createBlob(char yb,int i,QImage** res, const QColor& color = QColor(63, 63, 63)){
    *res = new QImage(QString(":/%1%2").arg(yb).arg(i)+QString(".png"));

    const QColor robot_color = QColor(63, 63, 63);

    if(robot_color == color)
        return *res;

    // Change the main color of the texture
    for(int i=0; i<(*res)->width(); ++i)
    {
        for(int j=0; j<(*res)->height(); ++j)
        {
            if((*res)->pixelColor(i, j) == robot_color)
                (*res)->setPixelColor(i, j, color);
        }
    }

    return *res;
}

QImage* createNumber(int i,int r,int g,int b,int a) {
    auto* img = new QImage(32,32,QImage::Format_ARGB32);
    auto* p = new QPainter();
    QBrush br;
    p->begin(img);
    QColor black(0,0,0,0);
    for (int x = 0; x < img->width(); x++) {
        for (int j= 0; j < img->height();j++) {
            img->setPixel(x,j,black.rgba());
        }
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


void SSLWorld::glinit() {
    g->loadTexture(new QImage(":/grass.png"));

    // Loading Robot textures for each robot
    for (int i = 0; i < cfg->Robots_Count(); i++)
        g->loadTexture(createBlob('b', i, &robots[i]->img, ROBOT_BLUE_CHASSIS_COLOR));

    for (int i = 0; i < cfg->Robots_Count(); i++)
        g->loadTexture(createBlob('y', i, &robots[cfg->Robots_Count() + i]->img, ROBOT_YELLOW_CHASSIS_COLOR));

    // Creating number textures
    for (int i=0; i<cfg->Robots_Count();i++)
        g->loadTexture(createNumber(i,15,193,225,255));

    for (int i=0; i<cfg->Robots_Count();i++)
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

void SSLWorld::step(dReal dt) {
    if (customDT > 0) dt = customDT;
    const auto ratio = m_parent->devicePixelRatio();
    g->initScene(m_parent->width()*ratio,m_parent->height()*ratio,0,0.7,1);
    int ballCollisionTry = 5;
    for (int kk=0;kk < ballCollisionTry;kk++) {
        const dReal* ballvel = dBodyGetLinearVel(ball->body);
        dReal ballspeed = ballvel[0]*ballvel[0] + ballvel[1]*ballvel[1] + ballvel[2]*ballvel[2];
        ballspeed = sqrt(ballspeed);
        if (ballspeed > 0.01) {
            dReal fk = cfg->BallFriction()*cfg->BallMass()*cfg->Gravity();
            dReal ballfx = -fk*ballvel[0] / ballspeed;
            dReal ballfy = -fk*ballvel[1] / ballspeed;
            dReal ballfz = -fk*ballvel[2] / ballspeed;
            dReal balltx = -ballfy*cfg->BallRadius();
            dReal ballty = ballfx*cfg->BallRadius();
            dReal balltz = 0;
            dBodyAddTorque(ball->body,balltx,ballty,balltz);
            dBodyAddForce(ball->body,ballfx,ballfy,ballfz);
        } else {
            dBodySetAngularVel(ball->body, 0, 0, 0);
            dBodySetLinearVel(ball->body, 0, 0, 0);
        }
        if (dt == 0) dt = last_dt;
        else last_dt = dt;

        selected = -1;
        p->step(dt/ballCollisionTry);
    }

    sim_time += last_dt;

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
    for (int k=0;k<cfg->Robots_Count() * 2;k++)
    {
        if (robots[k]->selected)
        {
            g->getViewpoint(xyz,hpr);
            dReal dist= (robots[k]->select_x-xyz[0])*(robots[k]->select_x-xyz[0])
                    +(robots[k]->select_y-xyz[1])*(robots[k]->select_y-xyz[1])
                    +(robots[k]->select_z-xyz[2])*(robots[k]->select_z-xyz[2]);
            if (dist < best_dist) {
                best_dist = dist;
                best_k = k;
            }
        }

        // Yellow robots are on the last half of count
        if(k >= cfg->Robots_Count())
            robots[k]->chassis->setColor(ROBOT_YELLOW_CHASSIS_COLOR);
        else
            robots[k]->chassis->setColor(ROBOT_BLUE_CHASSIS_COLOR);
    }
    if(best_k>=0)
    {
        if(best_k >= cfg->Robots_Count())
            robots[best_k]->chassis->setColor(
                        QColor::fromRgbF(ROBOT_YELLOW_CHASSIS_COLOR.redF()*2,
                                         ROBOT_YELLOW_CHASSIS_COLOR.greenF()*1.5,
                                         ROBOT_YELLOW_CHASSIS_COLOR.blueF()*1.5)
                        );
        else
            robots[best_k]->chassis->setColor(
                        QColor::fromRgbF(ROBOT_BLUE_CHASSIS_COLOR.redF()*2,
                                         ROBOT_BLUE_CHASSIS_COLOR.greenF()*1.5,
                                         ROBOT_BLUE_CHASSIS_COLOR.blueF()*1.5)
                        );
    }
    selected = best_k;
    ball->tag = -1;
    for (int k=0;k<cfg->Robots_Count() * 2;k++)
    {
        robots[k]->step();
        robots[k]->selected = false;
    }
    p->draw();
    g->drawSkybox(4 * cfg->Robots_Count() + 6 + 1, //31 for 6 robot
                  4 * cfg->Robots_Count() + 6 + 2, //32 for 6 robot
                  4 * cfg->Robots_Count() + 6 + 3, //33 for 6 robot
                  4 * cfg->Robots_Count() + 6 + 4, //34 for 6 robot
                  4 * cfg->Robots_Count() + 6 + 5, //31 for 6 robot
                  4 * cfg->Robots_Count() + 6 + 6);//36 for 6 robot

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

    g->finalizeScene();


    sendVisionBuffer();
    frame_num ++;
}

void SSLWorld::addRobotStatus(Robots_Status& robotsPacket, int robotID, bool infrared, KickStatus kickStatus) {
    Robot_Status* robot_status = robotsPacket.add_robots_status();
    robot_status->set_robot_id(robotID);
    robot_status->set_infrared(infrared);

    switch(kickStatus){
        case NO_KICK:
            robot_status->set_flat_kick(false);
            robot_status->set_chip_kick(false);
            break;
        case FLAT_KICK:
            robot_status->set_flat_kick(true);
            robot_status->set_chip_kick(false);
            break;
        case CHIP_KICK:
            robot_status->set_flat_kick(false);
            robot_status->set_chip_kick(true);
            break;
        default:
            robot_status->set_flat_kick(false);
            robot_status->set_chip_kick(false);
            break;
    }
}

void SSLWorld::sendRobotStatus(Robots_Status& robotsPacket, const QHostAddress& sender, int team) {
    int size = robotsPacket.ByteSize();
    QByteArray buffer(size, 0);
    robotsPacket.SerializeToArray(buffer.data(), buffer.size());
    if (team == 0) {
        blueStatusSocket->writeDatagram(buffer.data(), buffer.size(), sender, cfg->BlueStatusSendPort());
    } else {
        yellowStatusSocket->writeDatagram(buffer.data(), buffer.size(), sender, cfg->YellowStatusSendPort());
    }
}

void SSLWorld::recvActions() {
    grSim_Packet grSimPacket;
    while (commandSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = commandSocket->receiveDatagram();
        if (datagram.isValid()) {
            grSimPacket.ParseFromArray(datagram.data().data(), datagram.data().size());

            if (grSimPacket.has_commands()) {
                int team = 0;
                if (grSimPacket.commands().has_isteamyellow() && grSimPacket.commands().isteamyellow()) team = 1;

                for (int i=0; i < grSimPacket.commands().robot_commands_size(); i++) {
                    if (!grSimPacket.commands().robot_commands(i).has_id()) continue;
                    int k = grSimPacket.commands().robot_commands(i).id();
                    int id = robotIndex(k, team);
                    if ((id < 0) || (id >= cfg->Robots_Count()*2)) continue;
                    bool wheels = false;
                    if (grSimPacket.commands().robot_commands(i).has_wheelsspeed() && grSimPacket.commands().robot_commands(i).wheelsspeed()) {
                            if (grSimPacket.commands().robot_commands(i).has_wheel1()) robots[id]->setSpeed(0, grSimPacket.commands().robot_commands(i).wheel1());
                            if (grSimPacket.commands().robot_commands(i).has_wheel2()) robots[id]->setSpeed(1, grSimPacket.commands().robot_commands(i).wheel2());
                            if (grSimPacket.commands().robot_commands(i).has_wheel3()) robots[id]->setSpeed(2, grSimPacket.commands().robot_commands(i).wheel3());
                            if (grSimPacket.commands().robot_commands(i).has_wheel4()) robots[id]->setSpeed(3, grSimPacket.commands().robot_commands(i).wheel4());
                            wheels = true;
                    }

                    if (!wheels) {
                        dReal vx = 0;if (grSimPacket.commands().robot_commands(i).has_veltangent()) vx = grSimPacket.commands().robot_commands(i).veltangent();
                        dReal vy = 0;if (grSimPacket.commands().robot_commands(i).has_velnormal()) vy = grSimPacket.commands().robot_commands(i).velnormal();
                        dReal vw = 0;if (grSimPacket.commands().robot_commands(i).has_velangular()) vw = grSimPacket.commands().robot_commands(i).velangular();
                        robots[id]->setSpeed(vx, vy, vw);
                    }

                    dReal kickx = 0 , kickz = 0;
                    bool kick = false;
                    if (grSimPacket.commands().robot_commands(i).has_kickspeedx()) {
                        kick = true;
                        kickx = grSimPacket.commands().robot_commands(i).kickspeedx();
                    }

                    if (grSimPacket.commands().robot_commands(i).has_kickspeedz()) {
                        kick = true;
                        kickz = grSimPacket.commands().robot_commands(i).kickspeedz();
                    }

                    if (kick && ((kickx>0.0001) || (kickz>0.0001))) {
                        robots[id]->kicker->kick(kickx,kickz);
                    }

                    int rolling = 0;
                    if (grSimPacket.commands().robot_commands(i).has_spinner() && grSimPacket.commands().robot_commands(i).spinner()) {
                        rolling = 1;
                    }
                    robots[id]->kicker->setRoller(rolling);
                }
            }
            if (grSimPacket.has_replacement()) {
                for (int i=0; i < grSimPacket.replacement().robots_size(); i++) {
                    int team = 0;
                    if (grSimPacket.replacement().robots(i).has_yellowteam() && grSimPacket.replacement().robots(i).yellowteam()) team = 1;

                    if (!grSimPacket.replacement().robots(i).has_id()) continue;
                    int k = grSimPacket.replacement().robots(i).id();
                    dReal x = 0, y = 0, dir = 0;
                    bool turn_on = true;
                    if (grSimPacket.replacement().robots(i).has_x()) x = grSimPacket.replacement().robots(i).x();
                    if (grSimPacket.replacement().robots(i).has_y()) y = grSimPacket.replacement().robots(i).y();
                    if (grSimPacket.replacement().robots(i).has_dir()) dir = grSimPacket.replacement().robots(i).dir();
                    if (grSimPacket.replacement().robots(i).has_turnon()) turn_on = grSimPacket.replacement().robots(i).turnon();
                    int id = robotIndex(k, team);
                    if ((id < 0) || (id >= cfg->Robots_Count()*2)) continue;
                    robots[id]->setXY(x,y);
                    robots[id]->resetRobot();
                    robots[id]->setDir(dir);
                    robots[id]->on = turn_on;
                }
                if (grSimPacket.replacement().has_ball()) {
                    dReal x = 0, y = 0, z = 0, vx = 0, vy = 0;
                    ball->getBodyPosition(x, y, z);
                    const auto vel_vec = dBodyGetLinearVel(ball->body);
                    vx = vel_vec[0];
                    vy = vel_vec[1];

                    if (grSimPacket.replacement().ball().has_x()) x   = grSimPacket.replacement().ball().x();
                    if (grSimPacket.replacement().ball().has_y()) y   = grSimPacket.replacement().ball().y();
                    if (grSimPacket.replacement().ball().has_vx()) vx = grSimPacket.replacement().ball().vx();
                    if (grSimPacket.replacement().ball().has_vy()) vy = grSimPacket.replacement().ball().vy();

                    ball->setBodyPosition(x,y,cfg->BallRadius()*1.2);
                    dBodySetLinearVel(ball->body,vx,vy,0);
                    dBodySetAngularVel(ball->body,0,0,0);
                }
            }
        }

        // send robot status
        for (int team = 0; team < 2; team++) {
            Robots_Status robotsPacket;
            bool updateRobotStatus = false;
            for (int i = 0; i < cfg->Robots_Count(); i++) {
                int id = robotIndex(i, team);
                bool isInfrared = robots[id]->kicker->isTouchingBall();
                KickStatus kicking = robots[id]->kicker->isKicking();
                if (isInfrared != lastInfraredState[team][i] || kicking != lastKickState[team][i]) {
                    updateRobotStatus = true;
                    addRobotStatus(robotsPacket, i, isInfrared, kicking);
                    lastInfraredState[team][i] = isInfrared;
                    lastKickState[team][i] = kicking;
                }
            }
            if (updateRobotStatus) sendRobotStatus(robotsPacket, datagram.senderAddress(), team);
        }
    }
}

void SSLWorld::simControlSocketReady() {
    SimulatorCommand simulatorCommand;
    while (simControlSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = simControlSocket->receiveDatagram();
        if (!datagram.isValid()) {
            continue;
        }
        simulatorCommand.ParseFromArray(datagram.data().data(), datagram.data().size());

        SimulatorResponse response;
        processSimControl(simulatorCommand, response);
        
        QByteArray buffer(response.ByteSize(), 0);
        response.SerializeToArray(buffer.data(), buffer.size());
        simControlSocket->writeDatagram(buffer.data(), buffer.size(), datagram.senderAddress(), datagram.senderPort());
    }
}

void SSLWorld::blueControlSocketReady() {
    RobotControl robotControl;
    while (blueControlSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = blueControlSocket->receiveDatagram();
        if (!datagram.isValid()) {
            continue;
        }
        robotControl.ParseFromArray(datagram.data().data(), datagram.data().size());

        RobotControlResponse robotControlResponse;
        processRobotControl(robotControl, robotControlResponse, BLUE);

        QByteArray buffer(robotControlResponse.ByteSize(), 0);
        robotControlResponse.SerializeToArray(buffer.data(), buffer.size());
        blueControlSocket->writeDatagram(buffer.data(), buffer.size(), datagram.senderAddress(), datagram.senderPort());
    }
}

void SSLWorld::yellowControlSocketReady() {
    RobotControl robotControl;
    while (yellowControlSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = yellowControlSocket->receiveDatagram();
        if (!datagram.isValid()) {
            continue;
        }
        robotControl.ParseFromArray(datagram.data().data(), datagram.data().size());

        RobotControlResponse robotControlResponse;
        processRobotControl(robotControl, robotControlResponse, YELLOW);
        
        QByteArray buffer(robotControlResponse.ByteSize(), 0);
        robotControlResponse.SerializeToArray(buffer.data(), buffer.size());
        yellowControlSocket->writeDatagram(buffer.data(), buffer.size(), datagram.senderAddress(), datagram.senderPort());
    }
}


void SSLWorld::processSimControl(const SimulatorCommand &simulatorCommand, SimulatorResponse &simulatorResponse) {
    if(simulatorCommand.has_control()) {
        if(simulatorCommand.control().has_teleport_ball()) {
            auto teleBall = simulatorCommand.control().teleport_ball();
            processTeleportBall(simulatorResponse, teleBall);
        }
        
        for(const auto &teleBot : simulatorCommand.control().teleport_robot()) {
            int id = robotIndex(teleBot.id().id(), teleBot.id().team() == YELLOW ? 1 : 0);
            if (id < 0) {
                continue;
            }
            auto robot = robots[id];

            processTeleportRobot(teleBot, robot);
        }
    }

    if (simulatorCommand.has_config()) {
        if (simulatorCommand.config().has_geometry()
            || simulatorCommand.config().has_realism_config()
            || simulatorCommand.config().has_vision_port()) {
            simulatorResponse.add_errors()->set_code("GRSIM_UNSUPPORTED_CONFIG");
        }
        for (const auto &robotSpec : simulatorCommand.config().robot_specs()) {
            processRobotSpec(robotSpec);
        }
        restartRequired = true;
    }
    else {
        restartRequired = false;
    }
}

void SSLWorld::processRobotSpec(const RobotSpecs &robotSpec) const {
    auto settings = robotSpec.id().team() == YELLOW ? &cfg->yellowSettings : &cfg->blueSettings;
    if (robotSpec.has_radius()) {
        settings->RobotRadius = robotSpec.radius();
    }
    if (robotSpec.has_height()) {
        settings->RobotHeight = robotSpec.height();
    }
    if (robotSpec.has_mass()) {
        settings->BodyMass = robotSpec.mass();
    }
    if (robotSpec.has_max_linear_kick_speed()) {
        settings->MaxLinearKickSpeed = robotSpec.max_linear_kick_speed();
    }
    if (robotSpec.has_max_chip_kick_speed()) {
        settings->MaxChipKickSpeed = robotSpec.max_chip_kick_speed();
    }
    if (robotSpec.has_center_to_dribbler()) {
        settings->RobotCenterFromKicker = robotSpec.center_to_dribbler();
    }
    if (robotSpec.has_limits()) {
        processRobotLimits(robotSpec, settings);
    }
    if (robotSpec.has_wheel_angles()) {
        settings->Wheel1Angle = robotSpec.wheel_angles().front_right();
        settings->Wheel2Angle = robotSpec.wheel_angles().back_right();
        settings->Wheel3Angle = robotSpec.wheel_angles().back_left();
        settings->Wheel4Angle = robotSpec.wheel_angles().front_left();
    }
}

void SSLWorld::processRobotLimits(const RobotSpecs &robotSpec, RobotSettings *settings) {
    if (robotSpec.limits().has_acc_speedup_absolute_max()) {
        settings->AccSpeedupAbsoluteMax = robotSpec.limits().acc_speedup_absolute_max();
    }
    if (robotSpec.limits().has_acc_speedup_angular_max()) {
        settings->AccSpeedupAngularMax = robotSpec.limits().acc_speedup_angular_max();
    }
    if (robotSpec.limits().has_acc_brake_absolute_max()) {
        settings->AccBrakeAbsoluteMax = robotSpec.limits().acc_brake_absolute_max();
    }
    if (robotSpec.limits().has_acc_brake_angular_max()) {
        settings->AccBrakeAngularMax = robotSpec.limits().acc_brake_angular_max();
    }
    if (robotSpec.limits().has_vel_absolute_max()) {
        settings->VelAbsoluteMax = robotSpec.limits().vel_absolute_max();
    }
    if (robotSpec.limits().has_vel_angular_max()) {
        settings->VelAngularMax = robotSpec.limits().vel_angular_max();
    }
}

void SSLWorld::processTeleportRobot(const TeleportRobot &teleBot, Robot *robot) {
    dReal x, y, vx = 0, vy = 0, vAngular = 0;
    robot->getXY(x, y);
    dReal orientation = robot->getDir() * M_PI / 180.0;

    if (teleBot.has_x()) x = teleBot.x();
    if (teleBot.has_y()) y = teleBot.y();
    if (teleBot.has_orientation()) orientation = teleBot.orientation();
    if (teleBot.has_v_x()) vx = teleBot.v_x();
    if (teleBot.has_v_y()) vy = teleBot.v_y();
    if (teleBot.has_v_angular()) vAngular = teleBot.v_angular();

    robot->resetRobot();
    robot->setXY(x, y);
    robot->setDir(orientation * 180.0 / M_PI);

    dReal vxLocal = (vx * cos(-orientation)) - (vy * sin(-orientation));
    dReal vyLocal = (vy * cos(-orientation)) + (vx * sin(-orientation));
    robot->setSpeed(vxLocal, vyLocal, vAngular);

    if (teleBot.has_present()) {
        robot->on = teleBot.present();
        if(!teleBot.present()) {
            // Move it out of the field
            robot->setXY(1e6, 1e6);
        }
    }
}

void SSLWorld::processTeleportBall(SimulatorResponse &simulatorResponse, const TeleportBall &teleBall) const {
    dReal x = 0, y = 0, z = 0;
    ball->getBodyPosition(x, y, z);
    const auto vel_vec = dBodyGetLinearVel(ball->body);
    auto vx = vel_vec[0];
    auto vy = vel_vec[1];
    auto vz = vel_vec[2];

    if (teleBall.has_x()) x   = teleBall.x();
    if (teleBall.has_y()) y   = teleBall.y();
    if (teleBall.has_z()) z   = teleBall.z();
    if (teleBall.has_vx()) vx = teleBall.vx();
    if (teleBall.has_vy()) vy = teleBall.vy();
    if (teleBall.has_vz()) vz = teleBall.vz();

    ball->setBodyPosition(x, y, (cfg->BallRadius() + 0.005) + z);
    dBodySetLinearVel(ball->body, vx, vy, vz);
    dBodySetAngularVel(ball->body, 0, 0, 0);

    if(teleBall.has_teleport_safely()) {
        simulatorResponse.add_errors()->set_code("GRSIM_UNSUPPORTED_TELEPORT_SAFELY");
    }
    if(teleBall.has_roll()) {
        simulatorResponse.add_errors()->set_code("GRSIM_UNSUPPORTED_ROLL_BALL");
    }
}

void SSLWorld::processRobotControl(const RobotControl &robotControl, RobotControlResponse &robotControlResponse, Team team) {
    for (const auto &robotCommand : robotControl.robot_commands()) {
        int id = robotIndex(robotCommand.id(), team == YELLOW ? 1 : 0);
        if (id < 0) {
            continue;
        }
        auto robot = robots[id];
        auto robotCfg = team == YELLOW ? cfg->yellowSettings : cfg->blueSettings;

        if (robotCommand.has_kick_speed() && robotCommand.kick_speed() > 0) {
            double kickSpeed = robotCommand.kick_speed();
            double limit = robotCommand.kick_angle() > 0 ? robotCfg.MaxChipKickSpeed : robotCfg.MaxLinearKickSpeed; 
            if (kickSpeed > limit) {
                kickSpeed = limit;
            }
            double kickAngle = robotCommand.kick_angle() * M_PI / 180.0;
            double length = cos(kickAngle) * kickSpeed;
            double z = sin(kickAngle) * kickSpeed;
            robot->kicker->kick(length, z);
        }

        if (robotCommand.has_dribbler_speed()) {
            robot->kicker->setRoller(robotCommand.dribbler_speed() > 0 ? 1 : 0);
        }

        if (robotCommand.has_move_command()) {
            processMoveCommand(robotControlResponse, robotCommand.move_command(), robot);
        }
        
        auto feedback = robotControlResponse.add_feedback();
        feedback->set_id(robotCommand.id());
        feedback->set_dribbler_ball_contact(robot->kicker->isTouchingBall());
    }
}

void SSLWorld::processMoveCommand(RobotControlResponse &robotControlResponse, const RobotMoveCommand &moveCommand,
                                  Robot *robot) {
    if (moveCommand.has_wheel_velocity()) {
        auto &wheelVel = moveCommand.wheel_velocity();
        robot->setSpeed(0, wheelVel.front_right());
        robot->setSpeed(1, wheelVel.back_right());
        robot->setSpeed(2, wheelVel.back_left());
        robot->setSpeed(3, wheelVel.front_left());
    } else if (moveCommand.has_local_velocity()) {
        auto &vel = moveCommand.local_velocity();
        robot->setSpeed(vel.forward(), vel.left(), vel.angular());
    } else if(moveCommand.has_global_velocity()) {
        auto &vel = moveCommand.global_velocity();
        dReal orientation = -robot->getDir() * M_PI / 180.0;
        dReal vx = (vel.x() * cos(orientation)) - (vel.y() * sin(orientation));
        dReal vy = (vel.y() * cos(orientation)) + (vel.x() * sin(orientation));
        robot->setSpeed(vx, vy, vel.angular());
    }  else {
        SimulatorError *pError = robotControlResponse.add_errors();
        pError->set_code("GRSIM_UNSUPPORTED_MOVE_COMMAND");
        pError->set_message("Unsupported move command");
    }
}

dReal normalizeAngle(dReal a) {
    if (a>180) return -360+a;
    if (a<-180) return 360+a;
    return a;
}

bool SSLWorld::visibleInCam(int id, double x, double y) {
    const float CAMERA_OVERLAP = 0.2f;
    id %= 4;
    if (id==0)
    {
        if (x>-CAMERA_OVERLAP && y>-CAMERA_OVERLAP) return true;
    }
    if (id==1)
    {
        if (x>-CAMERA_OVERLAP && y<CAMERA_OVERLAP) return true;
    }
    if (id==2)
    {
        if (x<CAMERA_OVERLAP && y<CAMERA_OVERLAP) return true;
    }
    if (id==3)
    {
        if (x<CAMERA_OVERLAP && y>-CAMERA_OVERLAP) return true;
    }
    return false;
}

QPair<float, float> SSLWorld::cameraPosition(int id) {
    Q_ASSERT(id >= 0 && id < 4);

    // these positions must be kept synchronized with the quadrants in visibleInCam
    QVector<QPair<float, float>> camera_quadrants = {
        {0.25, 0.25},
        {0.25, -0.25},
        {-0.25, -0.25},
        {-0.25, 0.25}
    };
    return {camera_quadrants[id].first * (float)cfg->Field_Length(),
            camera_quadrants[id].second * (float)cfg->Field_Width()};
}

#define CONVUNIT(x) ((int)(1000*(x)))
SSL_WrapperPacket* SSLWorld::generatePacket(int cam_id) {
    auto* pPacket = new SSL_WrapperPacket();
    dReal x,y,z,dir,k;
    ball->getBodyPosition(x,y,z);    
    pPacket->mutable_detection()->set_camera_id(cam_id);
    pPacket->mutable_detection()->set_frame_number(frame_num);
    pPacket->mutable_detection()->set_t_capture(sim_time);
    pPacket->mutable_detection()->set_t_sent(sim_time);
    dReal dev_x = cfg->noiseDeviation_x();
    dReal dev_y = cfg->noiseDeviation_y();
    dReal dev_a = cfg->noiseDeviation_angle();

    const double CAMERA_HEIGHT = cfg->Camera_Height();

    // send the geometry for every camera consecutively to provide the camera setup for all of them
    if (((sendGeomCount++) / 4) % (cfg->sendGeometryEvery() / 4) == 0)
    {
        SSL_GeometryData* geom = pPacket->mutable_geometry();
        SSL_GeometryFieldSize* field = geom->mutable_field();

        auto models = geom->mutable_models();
        auto straight_two_phase = models->mutable_straight_two_phase();
        straight_two_phase->set_acc_slide(cfg->BallModelTwoPhaseAccSlide());
        straight_two_phase->set_acc_roll(cfg->BallModelTwoPhaseAccRoll());
        straight_two_phase->set_k_switch(cfg->BallModelTwoPhaseKSwitch());
        auto chip_fixed_loss = models->mutable_chip_fixed_loss();
        chip_fixed_loss->set_damping_xy_first_hop(cfg->BallModelChipFixedLossDampingXyFirstHop());
        chip_fixed_loss->set_damping_xy_other_hops(cfg->BallModelChipFixedLossDampingXyOtherHops());
        chip_fixed_loss->set_damping_z(cfg->BallModelChipFixedLossDampingZ());

        // Field general info
        field->set_field_length(CONVUNIT(cfg->Field_Length()));
        field->set_field_width(CONVUNIT(cfg->Field_Width()));
        field->set_boundary_width(CONVUNIT(cfg->Field_Margin()));
        field->set_goal_width(CONVUNIT(cfg->Goal_Width()));
        field->set_goal_depth(CONVUNIT(cfg->Goal_Depth()));

        // Field lines and arcs
        addFieldLinesArcs(field);

        // camera calibration
        const double FOCAL_LENGTH = cfg->Camera_Focal_Length();

        SSL_GeometryCameraCalibration* camera = geom->add_calib();
        camera->set_camera_id(cam_id);
        // dummy values
        camera->set_distortion(0.2);
        camera->set_focal_length((float)FOCAL_LENGTH);
        camera->set_principal_point_x(300);
        camera->set_principal_point_y(300);
        camera->set_q0(0.7);
        camera->set_q1(0.7);
        camera->set_q2(0.7);
        camera->set_q3(0.7);
        camera->set_tx(0);
        camera->set_ty(0);
        camera->set_tz(3500);

        auto camera_pos = cameraPosition(cam_id);
        camera->set_derived_camera_world_tx(CONVUNIT(camera_pos.first));
        camera->set_derived_camera_world_ty(CONVUNIT(camera_pos.second));
        camera->set_derived_camera_world_tz(CONVUNIT(CAMERA_HEIGHT));

    }
    if (!cfg->noise()) { dev_x = 0;dev_y = 0;dev_a = 0;}
    if (!cfg->vanishing() || (rand0_1() > cfg->ball_vanishing())) {
        if (visibleInCam(cam_id, x, y)) {
            const double BALL_RADIUS = cfg->BallRadius() * 1000.f;
            const double SCALING_LIMIT = cfg->Camera_Scaling_Limit();
            SSL_DetectionBall* vball = pPacket->mutable_detection()->add_balls();

            float output_x = randn_notrig(x*1000.0f,dev_x);
            float output_y = randn_notrig(y*1000.0f,dev_x);
            float output_z = z*1000.0f;

            const bool project_airborne = cfg->BallProjectAirborne();
            if (project_airborne) {
                output_z = qBound(0.0, output_z - BALL_RADIUS, CONVUNIT(CAMERA_HEIGHT) * SCALING_LIMIT);
                auto camera_pos = cameraPosition(cam_id);
                const float camera_x = CONVUNIT(camera_pos.first);
                const float camera_y = CONVUNIT(camera_pos.second);
                const float camera_z = CONVUNIT(CAMERA_HEIGHT);
                output_x = (output_x - camera_x) * (camera_z / (camera_z - output_z)) + camera_x;
                output_y = (output_y - camera_y) * (camera_z / (camera_z - output_z)) + camera_y;
            }

            vball->set_x(output_x);
            vball->set_y(output_y);
            if (!project_airborne) {
                vball->set_z(output_z);
            }
            vball->set_pixel_x(x*1000.0f);
            vball->set_pixel_y(y*1000.0f);
            vball->set_confidence(0.9 + rand0_1()*0.1);
        }
    }

    for(int i = 0; i < cfg->Robots_Count() * 2; i++) {
        if (!robots[i]->on) continue;
        bool is_blue = i < cfg->Robots_Count();
        const double vanishing = (is_blue) ? cfg->blue_team_vanishing() : cfg->yellow_team_vanishing();
        const int id = (is_blue) ? i : i - cfg->Robots_Count();

        if (!cfg->vanishing() || (rand0_1() > vanishing)) {
            robots[i]->getXY(x,y);
            dir = robots[i]->getDir(k);

            // reset when the robot has turned over
            if (cfg->ResetTurnOver() && k < 0.9) robots[i]->resetRobot();

            if (visibleInCam(cam_id, x, y)) {
                SSL_DetectionRobot* rob = (is_blue) ? pPacket->mutable_detection()->add_robots_blue()
                                                    : pPacket->mutable_detection()->add_robots_yellow();
                rob->set_robot_id(id);
                rob->set_pixel_x(x*1000.0f);
                rob->set_pixel_y(y*1000.0f);
                rob->set_confidence(1);
                rob->set_x(randn_notrig(x*1000.0f,dev_x));
                rob->set_y(randn_notrig(y*1000.0f,dev_y));
                rob->set_orientation(normalizeAngle(randn_notrig(dir,dev_a))*M_PI/180.0f);
            }
        }
    }
    return pPacket;
}

void SSLWorld::addFieldLinesArcs(SSL_GeometryFieldSize *field) {
    const float kFieldLength   = CONVUNIT(cfg->Field_Length());
    const float kFieldWidth    = CONVUNIT(cfg->Field_Width());
    const float kGoalWidth     = CONVUNIT(cfg->Goal_Width());
    const float kGoalDepth     = CONVUNIT(cfg->Goal_Depth());
    const float kBoundaryWidth = CONVUNIT(cfg->Field_Referee_Margin());
    const float kCenterRadius  = CONVUNIT(cfg->Field_Rad());
    const float kLineThickness = CONVUNIT(cfg->Field_Line_Width());
    const float kPenaltyDepth  = CONVUNIT(cfg->Field_Penalty_Depth());
    const float kPenaltyWidth  = CONVUNIT(cfg->Field_Penalty_Width());

    const float kXMax    = (kFieldLength-2*kLineThickness)/2;
    const float kXMin    = -kXMax;
    const float kYMax    = (kFieldWidth-kLineThickness)/2;
    const float kYMin    = -kYMax;
    const float penAreaX = kXMax - kPenaltyDepth;
    const float penAreaY = kPenaltyWidth / 2.0f;

    // Field lines
    addFieldLine(field, "TopTouchLine", kXMin-kLineThickness/2, kYMax, kXMax+kLineThickness/2, kYMax, kLineThickness);
    addFieldLine(field, "BottomTouchLine", kXMin-kLineThickness/2, kYMin, kXMax+kLineThickness/2, kYMin, kLineThickness);
    addFieldLine(field, "LeftGoalLine", kXMin, kYMin, kXMin, kYMax, kLineThickness);
    addFieldLine(field, "RightGoalLine", kXMax, kYMin, kXMax, kYMax, kLineThickness);
    addFieldLine(field, "HalfwayLine", 0, kYMin, 0, kYMax, kLineThickness);
    addFieldLine(field, "CenterLine", kXMin, 0, kXMax, 0, kLineThickness);
    addFieldLine(field, "LeftPenaltyStretch",  kXMin+kPenaltyDepth-kLineThickness/2, -kPenaltyWidth/2, kXMin+kPenaltyDepth-kLineThickness/2, kPenaltyWidth/2, kLineThickness);
    addFieldLine(field, "RightPenaltyStretch", kXMax-kPenaltyDepth+kLineThickness/2, -kPenaltyWidth/2, kXMax-kPenaltyDepth+kLineThickness/2, kPenaltyWidth/2, kLineThickness);

    addFieldLine(field, "RightGoalTopLine", kXMax, kGoalWidth/2, kXMax+kGoalDepth, kGoalWidth/2, kLineThickness);
    addFieldLine(field, "RightGoalBottomLine", kXMax, -kGoalWidth/2, kXMax+kGoalDepth, -kGoalWidth/2, kLineThickness);
    addFieldLine(field, "RightGoalDepthLine", kXMax+kGoalDepth-kLineThickness/2, -kGoalWidth/2, kXMax+kGoalDepth-kLineThickness/2, kGoalWidth/2, kLineThickness);

    addFieldLine(field, "LeftGoalTopLine", -kXMax, kGoalWidth/2, -kXMax-kGoalDepth, kGoalWidth/2, kLineThickness);
    addFieldLine(field, "LeftGoalBottomLine", -kXMax, -kGoalWidth/2, -kXMax-kGoalDepth, -kGoalWidth/2, kLineThickness);
    addFieldLine(field, "LeftGoalDepthLine", -kXMax-kGoalDepth+kLineThickness/2, -kGoalWidth/2, -kXMax-kGoalDepth+kLineThickness/2, kGoalWidth/2, kLineThickness);

    addFieldLine(field, "LeftFieldLeftPenaltyStretch",   kXMin, kPenaltyWidth/2,  kXMin + kPenaltyDepth, kPenaltyWidth/2,   kLineThickness);
    addFieldLine(field, "LeftFieldRightPenaltyStretch",  kXMin, -kPenaltyWidth/2, kXMin + kPenaltyDepth, -kPenaltyWidth/2,  kLineThickness);
    addFieldLine(field, "RightFieldLeftPenaltyStretch",  kXMax, -kPenaltyWidth/2, kXMax - kPenaltyDepth, -kPenaltyWidth/2,kLineThickness);
    addFieldLine(field, "RightFieldRightPenaltyStretch", kXMax, kPenaltyWidth/2,  kXMax - kPenaltyDepth, kPenaltyWidth/2,     kLineThickness);

    // Field arcs
    addFieldArc(field, "CenterCircle",              0,     0,                  kCenterRadius-kLineThickness/2,  0,        2*M_PI,   kLineThickness);
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
    SSL_FieldCircularArc *arc = field->add_field_arcs();
	arc->set_name(name.c_str());
	arc->mutable_center()->set_x(c_x);
	arc->mutable_center()->set_y(c_y);
    arc->set_radius(radius);
    arc->set_a1(a1);
    arc->set_a2(a2);
    arc->set_thickness(thickness);
}

SendingPacket::SendingPacket(SSL_WrapperPacket* _packet,int _t) {
    packet = _packet;
    t      = _t;
}

void SSLWorld::sendVisionBuffer() {
    int t = (int)(sim_time*1000);
    sendQueue.push_back(new SendingPacket(generatePacket(0),t));
    sendQueue.push_back(new SendingPacket(generatePacket(1),t));
    sendQueue.push_back(new SendingPacket(generatePacket(2),t));
    sendQueue.push_back(new SendingPacket(generatePacket(3),t));
    while (t - sendQueue.front()->t>=cfg->sendDelay())
    {
        SSL_WrapperPacket *pPacket = sendQueue.front()->packet;
        delete sendQueue.front();
        sendQueue.pop_front();
        visionServer->send(*pPacket);
        delete pPacket;
        if (sendQueue.isEmpty()) break;
    }
}

void RobotsFormation::setAll(const dReal* xx, const dReal *yy)
{
    for (int i=0;i<MAX_ROBOT_COUNT;i++)
    {
        x[i] = xx[i];
        y[i] = yy[i];
    }
}

RobotsFormation::RobotsFormation(E_FORMATION type, ConfigWidget* _cfg) : cfg(_cfg) {
    switch (type) {
        case FORMATION_OUTSIDE: {
            double yv = -(_cfg->Field_Width() / 2 + _cfg->Field_Margin() / 2);
            dReal teamPosX[MAX_ROBOT_COUNT] = {0.40, 0.80, 1.20, 1.60, 2.00, 2.40,
                                               2.80, 3.20, 3.60, 4.00, 4.40, 4.80,
                                               0.40, 0.80, 1.20, 1.60};
            dReal teamPosY[MAX_ROBOT_COUNT] = {yv, yv, yv, yv, yv, yv, yv, yv, yv, yv, yv, yv, yv, yv, yv, yv};
            setAll(teamPosX, teamPosY);
            break;
        }
        case FORMATION_INSIDE_1: {
            dReal teamPosX[MAX_ROBOT_COUNT] = { 1.50,  1.50,  1.50,  0.55,  2.50,  3.60,
                                                3.20,  3.20,  3.20,  3.20,  3.20,  3.20,
                                                0.40,  0.80,  1.20,  1.60};
            dReal teamPosY[MAX_ROBOT_COUNT] = { 1.12,  0.0,  -1.12,  0.00,  0.00,  0.00,
                                                0.75, -0.75,  1.50, -1.50,  2.25, -2.25,
                                                -3.50, -3.50, -3.50, -3.50};
            setAll(teamPosX,teamPosY);
            break;
        }
        case FORMATION_INSIDE_2: {
            dReal teamPosX[MAX_ROBOT_COUNT] = { 4.20,  3.40,  3.40,  0.70,  0.70,  0.70,
                                                2.00,  2.00,  2.00,  2.00,  2.00,  2.00,
                                                0.40,  0.80,  1.20,  1.60};
            dReal teamPosY[MAX_ROBOT_COUNT] = { 0.00, -0.20,  0.20,  0.00,  2.25, -2.25,
                                                0.75, -0.75,  1.50, -1.50,  2.25, -2.25,
                                                -3.50, -3.50, -3.50, -3.50};
            setAll(teamPosX,teamPosY);
            break;
        }
        case FORMATION_OUTSIDE_FIELD: {
            double yv = -(_cfg->Field_Width() / 2 + _cfg->Field_Margin() + _cfg->Field_Referee_Margin() + 0.5);
            dReal teamPosX[MAX_ROBOT_COUNT] = { 0.40,  0.80,  1.20,  1.60,  2.00,  2.40,
                                                2.80,  3.20,  3.60,  4.00,  4.40,  4.80,
                                                0.40,  0.80,  1.20,  1.60};
            dReal teamPosY[MAX_ROBOT_COUNT] = {yv, yv, yv, yv, yv, yv, yv, yv, yv, yv, yv, yv, yv, yv, yv, yv};
            setAll(teamPosX,teamPosY);
            break;
        }
    }
}

void RobotsFormation::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    int k;
    for (k=0;k<cfg->Robots_Count();k++) x[k] = y[k] = 0;
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
        if (k==cfg->Robots_Count()-1) break;
        k++;
    }
}

void RobotsFormation::resetRobots(Robot** r, int team)
{
    dReal dir=-1;
    if (team==1) dir = 1;
    for (int k=0;k<cfg->Robots_Count();k++)
    {
        r[k + team*cfg->Robots_Count()]->setXY(x[k]*dir,y[k]);
        r[k + team*cfg->Robots_Count()]->resetRobot();
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

        //	calculate return second deviate
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
