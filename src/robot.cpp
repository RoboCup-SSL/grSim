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

#include "robot.h"

// ang2 = position angle
// ang  = rotation angle
Robot::Wheel::Wheel(Robot* robot,int _id,dReal ang,dReal ang2,int wheeltexid)
{
    id = _id;
    rob = robot;
    dReal rad = rob->cfg->robotSettings.RobotRadius - rob->cfg->robotSettings.WheelThickness / 2.0;
    ang *= M_PI/180.0f;
    ang2 *= M_PI/180.0f;
    dReal x = rob->m_x;
    dReal y = rob->m_y;
    dReal z = rob->m_z;
    dReal centerx = x+rad*cos(ang2);
    dReal centery = y+rad*sin(ang2);
    dReal centerz = z-rob->cfg->robotSettings.RobotHeight*0.5+rob->cfg->robotSettings.WheelRadius-rob->cfg->robotSettings.BottomHeight;
    cyl = new PCylinder(centerx,centery,centerz,rob->cfg->robotSettings.WheelRadius,rob->cfg->robotSettings.WheelThickness,rob->cfg->robotSettings.WheelMass,0.9,0.9,0.9,wheeltexid);
    cyl->setRotation(-sin(ang),cos(ang),0,M_PI*0.5);
    cyl->setBodyRotation(-sin(ang),cos(ang),0,M_PI*0.5,true);       //set local rotation matrix
    cyl->setBodyPosition(centerx-x,centery-y,centerz-z,true);       //set local position vector
    cyl->space = rob->space;

    rob->w->addObject(cyl);

    joint = dJointCreateHinge (rob->w->world,0);

    dJointAttach (joint,rob->chassis->body,cyl->body);
    const dReal *a = dBodyGetPosition (cyl->body);
    dJointSetHingeAxis (joint,cos(ang),sin(ang),0);
    dJointSetHingeAnchor (joint,a[0],a[1],a[2]);

    motor = dJointCreateAMotor(rob->w->world,0);
    dJointAttach(motor,rob->chassis->body,cyl->body);
    dJointSetAMotorNumAxes(motor,1);
    dJointSetAMotorAxis(motor,0,1,cos(ang),sin(ang),0);
    dJointSetAMotorParam(motor,dParamFMax,rob->cfg->robotSettings.Wheel_Motor_FMax);
    speed = 0;
}

void Robot::Wheel::step()
{
    dJointSetAMotorParam(motor,dParamVel,speed);
    dJointSetAMotorParam(motor,dParamFMax,rob->cfg->robotSettings.Wheel_Motor_FMax);
}

Robot::Kicker::Kicker(Robot* robot) : holdingBall(false)
{
    rob = robot;

    dReal x = rob->m_x;
    dReal y = rob->m_y;
    dReal z = rob->m_z;
    dReal centerx = x+(rob->cfg->robotSettings.RobotCenterFromKicker+rob->cfg->robotSettings.KickerThickness);
    dReal centery = y;
    dReal centerz = z-(rob->cfg->robotSettings.RobotHeight)*0.5f+rob->cfg->robotSettings.WheelRadius-rob->cfg->robotSettings.BottomHeight+rob->cfg->robotSettings.KickerZ;
    box = new PBox(centerx,centery,centerz,rob->cfg->robotSettings.KickerThickness,rob->cfg->robotSettings.KickerWidth,rob->cfg->robotSettings.KickerHeight,rob->cfg->robotSettings.KickerMass,0.9,0.9,0.9);
    box->setBodyPosition(centerx-x,centery-y,centerz-z,true);
    box->space = rob->space;

    rob->w->addObject(box);

    joint = dJointCreateHinge (rob->w->world,0);
    dJointAttach (joint,rob->chassis->body,box->body);
    const dReal *aa = dBodyGetPosition (box->body);
    dJointSetHingeAnchor (joint,aa[0],aa[1],aa[2]);
    dJointSetHingeAxis (joint,0,-1,0);

    dJointSetHingeParam (joint,dParamVel,0);
    dJointSetHingeParam (joint,dParamLoStop,0);
    dJointSetHingeParam (joint,dParamHiStop,0);

    rolling = 0;
    kicking = NO_KICK;
}

void Robot::Kicker::step()
{
    if (!isTouchingBall() || rolling == 0) unholdBall();
    if (kicking != NO_KICK)
    {
        box->setColor(1,0.3,0);
        kickstate--;
        if (kickstate<=0) kicking = NO_KICK;
    }
    else if (rolling!=0)
    {
        box->setColor(1,0.7,0);
        if (isTouchingBall())
        {
            holdBall();
        }
    }
    else box->setColor(0.9,0.9,0.9);
}

bool Robot::Kicker::isTouchingBall()
{
    dReal vx,vy,vz;
    dReal bx,by,bz;
    dReal kx,ky,kz;
    rob->chassis->getBodyDirection(vx,vy,vz);
    rob->getBall()->getBodyPosition(bx,by,bz);
    box->getBodyPosition(kx,ky,kz);
    kx += vx*rob->cfg->robotSettings.KickerThickness*0.5f;
    ky += vy*rob->cfg->robotSettings.KickerThickness*0.5f;
    dReal xx = fabs((kx-bx)*vx + (ky-by)*vy);
    dReal yy = fabs(-(kx-bx)*vy + (ky-by)*vx);
    dReal zz = fabs(kz-bz);
    return ((xx<rob->cfg->robotSettings.KickerThickness*2.0f+rob->cfg->BallRadius()) && (yy<rob->cfg->robotSettings.KickerWidth*0.5f) && (zz<rob->cfg->robotSettings.KickerHeight*0.5f));
}

KickStatus Robot::Kicker::isKicking()
{
    return kicking;
}

void Robot::Kicker::setRoller(int roller)
{
    rolling = roller;
}

int Robot::Kicker::getRoller()
{
    return rolling;
}

void Robot::Kicker::toggleRoller()
{
    if (rolling==0)
        rolling = 1;
    else rolling = 0;
}

void Robot::Kicker::kick(dReal kickspeedx, dReal kickspeedz)
{    
    dReal dx,dy,dz;
    dReal vx,vy,vz;
    rob->chassis->getBodyDirection(dx,dy,dz);dz = 0;
    dReal zf = kickspeedz;
    unholdBall();
    if (isTouchingBall())
    {
        dReal dlen = dx*dx+dy*dy+dz*dz;
        dlen = sqrt(dlen);
        vx = dx*kickspeedx/dlen;
        vy = dy*kickspeedx/dlen;
        vz = zf;
        const dReal* vball = dBodyGetLinearVel(rob->getBall()->body);
        dReal vn = -(vball[0]*dx + vball[1]*dy)*rob->cfg->robotSettings.KickerDampFactor;
        dReal vt = -(vball[0]*dy - vball[1]*dx);
        vx += vn * dx - vt * dy;
        vy += vn * dy + vt * dx;
        dBodySetLinearVel(rob->getBall()->body,vx,vy,vz);
        if (kickspeedz >= 1)
            kicking = CHIP_KICK;
        else
            kicking = FLAT_KICK;
        kickstate = 10;
    }
}

void Robot::Kicker::holdBall(){
    dReal vx,vy,vz;
    dReal bx,by,bz;
    dReal kx,ky,kz;
    rob->chassis->getBodyDirection(vx,vy,vz);
    rob->getBall()->getBodyPosition(bx,by,bz);
    box->getBodyPosition(kx,ky,kz);
    kx += vx*rob->cfg->robotSettings.KickerThickness*0.5f;
    ky += vy*rob->cfg->robotSettings.KickerThickness*0.5f;
    dReal xx = fabs((kx-bx)*vx + (ky-by)*vy);
    dReal yy = fabs(-(kx-bx)*vy + (ky-by)*vx);
    if(holdingBall || xx-rob->cfg->BallRadius() < 0) return;
    dBodySetLinearVel(rob->getBall()->body,0,0,0);
    robot_to_ball = dJointCreateHinge(rob->getWorld()->world,0);
    dJointAttach (robot_to_ball,box->body,rob->getBall()->body);
    holdingBall = true;
}

void Robot::Kicker::unholdBall(){
    if(holdingBall) {
        dJointDestroy(robot_to_ball);
        holdingBall = false;
    }
}

Robot::Robot(PWorld* world,PBall *ball,ConfigWidget* _cfg,dReal x,dReal y,dReal z,dReal r,dReal g,dReal b,int rob_id,int wheeltexid,int dir)
{      
    m_r = r;
    m_g = g;
    m_b = b;
    m_x = x;
    m_y = y;
    m_z = z;
    w = world;
    m_ball = ball;
    m_dir = dir;
    cfg = _cfg;
    m_rob_id = rob_id;

    AccSpeedupAbsoluteMax = cfg->robotSettings.AccSpeedupAbsoluteMax;
    AccSpeedupAngularMax = cfg->robotSettings.AccSpeedupAngularMax;
    AccBrakeAbsoluteMax = cfg->robotSettings.AccBrakeAbsoluteMax;
    AccBrakeAngularMax = cfg->robotSettings.AccBrakeAngularMax;
    VelAbsoluteMax = cfg->robotSettings.VelAbsoluteMax;
    VelAngularMax = cfg->robotSettings.VelAngularMax;

    space = w->space;

    chassis = new PCylinder(x,y,z,cfg->robotSettings.RobotRadius,cfg->robotSettings.RobotHeight,cfg->robotSettings.BodyMass*0.99f,r,g,b,rob_id,true);
    chassis->space = space;
    w->addObject(chassis);

    dummy   = new PBall(x,y,z,cfg->robotSettings.RobotCenterFromKicker,cfg->robotSettings.BodyMass*0.01f,0,0,0);
    dummy->setVisibility(false);
    dummy->space = space;
    w->addObject(dummy);

    dummy_to_chassis = dJointCreateFixed(world->world,0);
    dJointAttach (dummy_to_chassis,chassis->body,dummy->body);

    kicker = new Kicker(this);

    wheels[0] = new Wheel(this,0,cfg->robotSettings.Wheel1Angle,cfg->robotSettings.Wheel1Angle,wheeltexid);
    wheels[1] = new Wheel(this,1,cfg->robotSettings.Wheel2Angle,cfg->robotSettings.Wheel2Angle,wheeltexid);
    wheels[2] = new Wheel(this,2,cfg->robotSettings.Wheel3Angle,cfg->robotSettings.Wheel3Angle,wheeltexid);
    wheels[3] = new Wheel(this,3,cfg->robotSettings.Wheel4Angle,cfg->robotSettings.Wheel4Angle,wheeltexid);
    firsttime=true;
    on = true;
}

Robot::~Robot()
{

}

PBall* Robot::getBall()
{
    return m_ball;
}

PWorld* Robot::getWorld()
{
    return w;
}

int Robot::getID()
{
    return m_rob_id - 1;
}

void normalizeVector(dReal& x,dReal& y,dReal& z)
{
    dReal d = sqrt(x*x + y*y + z*z);
    x /= d;y /= d;z /= d;
}

void Robot::step()
{    
    if (on)
    {
        if (firsttime)
        {
            if (m_dir==-1) setDir(180);
            firsttime = false;
        }
        wheels[0]->step();
        wheels[1]->step();
        wheels[2]->step();
        wheels[3]->step();
        kicker->step();
    }
    else {
        if (last_state)
        {
            wheels[0]->speed = wheels[1]->speed = wheels[2]->speed = wheels[3]->speed = 0;
            kicker->setRoller(0);
            wheels[0]->step();
            wheels[1]->step();
            wheels[2]->step();
            wheels[3]->step();
            kicker->step();
        }
    }
    last_state = on;
}

void Robot::drawLabel()
{
    glPushMatrix();
    dVector3 pos;
    dReal fr_r,fr_b,fr_n;w->g->getFrustum(fr_r,fr_b,fr_n);
    const dReal txtWidth = 12.0f*fr_r/(dReal)w->g->getWidth();
    const dReal txtHeight = 24.0f*fr_b/(dReal)w->g->getHeight();
    pos[0] = dBodyGetPosition(chassis->body)[0];
    pos[1] = dBodyGetPosition(chassis->body)[1];
    pos[2] = dBodyGetPosition(chassis->body)[2];    
    dReal xyz[3],hpr[3];
    w->g->getViewpoint(xyz,hpr);
    dReal ax = -pos[0]+xyz[0];
    dReal ay = -pos[1]+xyz[1];
    dReal az = -pos[2]+xyz[2];
    dReal fx,fy,fz;
    dReal rx,ry,rz;
    w->g->getCameraForward(fx,fy,fz);
    w->g->getCameraRight(rx,ry,rz);
    normalizeVector(fx,fy,fz);
    normalizeVector(rx,ry,rz);
    dReal zz = fx*ax + fy*ay + fz*az;
    dReal zfact = zz/fr_n;
    pos[2] += cfg->robotSettings.RobotHeight*0.5f + cfg->robotSettings.BottomHeight + cfg->robotSettings.WheelRadius + txtHeight*zfact;
    dMatrix3 rot;
    dRFromAxisAndAngle(rot,0,0,0,0);
    dReal tx = fy*rz-ry*fz;
    dReal ty = rx*fz-fx*rz;
    dReal tz = fx*ry-fy*rx;
    w->g->setTransform(pos,rot);
    w->g->useTexture((m_rob_id-1) + 11 + 10*((on)?0:1));
    glShadeModel (GL_FLAT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glTexCoord2f(1,1);glVertex3f( txtWidth*rx*zfact +txtHeight*tx*zfact, txtWidth*ry*zfact +txtHeight*ty*zfact, txtWidth*rz*zfact +txtHeight*tz*zfact);
    glTexCoord2f(0,1);glVertex3f(-txtWidth*rx*zfact +txtHeight*tx*zfact,-txtWidth*ry*zfact +txtHeight*ty*zfact,-txtWidth*rz*zfact +txtHeight*tz*zfact);
    glTexCoord2f(0,0);glVertex3f(-txtWidth*rx*zfact -txtHeight*tx*zfact,-txtWidth*ry*zfact -txtHeight*ty*zfact,-txtWidth*rz*zfact -txtHeight*tz*zfact);
    glTexCoord2f(1,0);glVertex3f( txtWidth*rx*zfact -txtHeight*tx*zfact, txtWidth*ry*zfact -txtHeight*ty*zfact, txtWidth*rz*zfact -txtHeight*tz*zfact);
    glEnd();
    glDisable(GL_BLEND);
    w->g->noTexture();
    glPopMatrix();
}

void Robot::resetSpeeds()
{
    wheels[0]->speed = wheels[1]->speed = wheels[2]->speed = wheels[3]->speed = 0;
}


void Robot::resetRobot()
{
    resetSpeeds();
    dBodySetLinearVel(chassis->body,0,0,0);
    dBodySetAngularVel(chassis->body,0,0,0);
    dBodySetLinearVel(dummy->body,0,0,0);
    dBodySetAngularVel(dummy->body,0,0,0);
    dBodySetLinearVel(kicker->box->body,0,0,0);
    dBodySetAngularVel(kicker->box->body,0,0,0);
    for (int i=0;i<4;i++)
    {
        dBodySetLinearVel(wheels[i]->cyl->body,0,0,0);
        dBodySetAngularVel(wheels[i]->cyl->body,0,0,0);
    }
    dReal x,y;
    getXY(x,y);
    setXY(x,y);
    if (m_dir==-1) setDir(180);
    else setDir(0);
}

void Robot::getXY(dReal& x,dReal &y)
{
    dReal xx,yy,zz;
    chassis->getBodyPosition(xx,yy,zz);
    x = xx;y = yy;
}

dReal Robot::getDir()
{
    dReal x,y,z;
    chassis->getBodyDirection(x,y,z);
    dReal dot = x;//zarb dar (1.0,0.0,0.0)
    dReal length = sqrt(x*x + y*y);
    dReal absAng = (dReal)(acos((dReal)(dot/length)) * (180.0f/M_PI));
    return (y > 0) ? absAng : -absAng;
}

dReal Robot::getDir(dReal &k)
{
    dReal x,y,z;
    chassis->getBodyDirection(x,y,z,k);
    dReal dot = x;//zarb dar (1.0,0.0,0.0)
    dReal length = sqrt(x*x + y*y);
    dReal absAng = (dReal)(acos((dReal)(dot/length)) * (180.0f/M_PI));
    return (y > 0) ? absAng : -absAng;
}

void Robot::setXY(dReal x,dReal y)
{
    dReal xx,yy,zz,kx,ky,kz;
    dReal height = ROBOT_START_Z(cfg);
    chassis->getBodyPosition(xx,yy,zz);
    chassis->setBodyPosition(x,y,height);
    dummy->setBodyPosition(x,y,height);
    kicker->box->getBodyPosition(kx,ky,kz);
    kicker->box->setBodyPosition(kx-xx+x,ky-yy+y,kz-zz+height);
    for (int i=0;i<4;i++)
    {
        wheels[i]->cyl->getBodyPosition(kx,ky,kz);
        wheels[i]->cyl->setBodyPosition(kx-xx+x,ky-yy+y,kz-zz+height);
    }
}

void Robot::setDir(dReal ang)
{
    ang*=M_PI/180.0f;
    chassis->setBodyRotation(0,0,1,ang);
    kicker->box->setBodyRotation(0,0,1,ang);
    dummy->setBodyRotation(0,0,1,ang);
    dMatrix3 wLocalRot,wRot,cRot;
    dVector3 localPos,finalPos,cPos;
    chassis->getBodyPosition(cPos[0],cPos[1],cPos[2],false);
    chassis->getBodyRotation(cRot,false);
    kicker->box->getBodyPosition(localPos[0],localPos[1],localPos[2],true);
    dMultiply0(finalPos,cRot,localPos,4,3,1);finalPos[0]+=cPos[0];finalPos[1]+=cPos[1];finalPos[2]+=cPos[2];
    kicker->box->setBodyPosition(finalPos[0],finalPos[1],finalPos[2],false);
    for (int i=0;i<4;i++)
    {
        wheels[i]->cyl->getBodyRotation(wLocalRot,true);
        dMultiply0(wRot,cRot,wLocalRot,3,3,3);
        dBodySetRotation(wheels[i]->cyl->body,wRot);
        wheels[i]->cyl->getBodyPosition(localPos[0],localPos[1],localPos[2],true);
        dMultiply0(finalPos,cRot,localPos,4,3,1);finalPos[0]+=cPos[0];finalPos[1]+=cPos[1];finalPos[2]+=cPos[2];
        wheels[i]->cyl->setBodyPosition(finalPos[0],finalPos[1],finalPos[2],false);
    }
}

void Robot::setSpeed(int i,dReal s)
{
    if (!((i>=4) || (i<0)))
        wheels[i]->speed = s;
}

void Robot::setSpeed(dReal vx, dReal vy, dReal vw)
{
    dReal _DEG2RAD = M_PI / 180.0;

    dReal v = sqrt(vx * vx + vy * vy);
    if (v > VelAbsoluteMax) {
        vx *= VelAbsoluteMax / v;
        vy *= VelAbsoluteMax / v;
        v = VelAbsoluteMax;
    }
    if (abs(vw) > VelAngularMax) {
        vw = copysign(VelAngularMax, vw);
    }
    
    const dReal* cvv = dBodyGetLinearVel(chassis->body);
    dReal cv = sqrt(cvv[0]*cvv[0]+cvv[1]*cvv[1]);
    dReal a = (v - cv) / cfg->DeltaTime() / 2;
    dReal aLimit = a > 0 ? AccSpeedupAbsoluteMax : AccBrakeAbsoluteMax;
    if (abs(a) > aLimit) {
        a = copysign(aLimit, a);
        dReal new_v = cv + a * cfg->DeltaTime() * 2;
        if (v > 0) {
            vx *= new_v / v;
            vy *= new_v / v;
        } else {
            // convert global to local
            dReal angle;
            angle = getDir();
            angle *= _DEG2RAD;
            dReal cvx = cvv[0]*cos(angle) + cvv[1]*sin(angle);
            dReal cvy = -cvv[0]*sin(angle) + cvv[1]*cos(angle);

            vx = cvx * (new_v / cv);
            vy = cvy * (new_v / cv);
        }
    }

    const dReal* cvvw = dBodyGetAngularVel(chassis->body);
    dReal cvw = cvvw[2];
    dReal aw = (vw - cvw) / cfg->DeltaTime() / 2;
    dReal awLimit = aw > 0 ? AccSpeedupAngularMax : AccBrakeAngularMax;
    if (abs(aw) > awLimit) {
        aw = copysign(awLimit, aw);
        vw = cvw + aw * cfg->DeltaTime() * 2;
    }
    
    // Calculate Motor Speeds
    dReal motorAlpha[4] = {cfg->robotSettings.Wheel1Angle * _DEG2RAD, cfg->robotSettings.Wheel2Angle * _DEG2RAD, cfg->robotSettings.Wheel3Angle * _DEG2RAD, cfg->robotSettings.Wheel4Angle * _DEG2RAD};

    dReal dw1 =  (1.0 / cfg->robotSettings.WheelRadius) * (( (cfg->robotSettings.RobotRadius * vw) - (vx * sin(motorAlpha[0])) + (vy * cos(motorAlpha[0]))) );
    dReal dw2 =  (1.0 / cfg->robotSettings.WheelRadius) * (( (cfg->robotSettings.RobotRadius * vw) - (vx * sin(motorAlpha[1])) + (vy * cos(motorAlpha[1]))) );
    dReal dw3 =  (1.0 / cfg->robotSettings.WheelRadius) * (( (cfg->robotSettings.RobotRadius * vw) - (vx * sin(motorAlpha[2])) + (vy * cos(motorAlpha[2]))) );
    dReal dw4 =  (1.0 / cfg->robotSettings.WheelRadius) * (( (cfg->robotSettings.RobotRadius * vw) - (vx * sin(motorAlpha[3])) + (vy * cos(motorAlpha[3]))) );

    setSpeed(0 , dw1);
    setSpeed(1 , dw2);
    setSpeed(2 , dw3);
    setSpeed(3 , dw4);
}

dReal Robot::getSpeed(int i)
{
    if ((i>=4) || (i<0)) return -1;
    return wheels[i]->speed;
}

void Robot::incSpeed(int i,dReal v)
{
    if (!((i>=4) || (i<0)))
        wheels[i]->speed += v;
}

