#include "robot.h"

// ang2 = position angle
// ang  = rotation angle
Robot::Wheel::Wheel(Robot* robot,int _id,float ang,float ang2,int wheeltexid)
{
    id = _id;
    rob = robot;
    float rad = rob->cfg->CHASSISWIDTH()*0.5 - rob->cfg->WHEELLENGTH()*0.5;
    ang *= M_PI/180.0f;
    ang2 *= M_PI/180.0f;
    float x = rob->m_x;
    float y = rob->m_y;
    float z = rob->m_z;
    float centerx = x+rad*cos(ang2);
    float centery = y+rad*sin(ang2);
    float centerz = z-rob->cfg->CHASSISHEIGHT()*0.5+rob->cfg->WHEELRADIUS()-rob->cfg->BOTTOMHEIGHT();
    cyl = new PCylinder(centerx,centery,centerz,rob->cfg->WHEELRADIUS(),rob->cfg->WHEELLENGTH(),rob->cfg->WHEELMASS(),0.9,0.9,0.9,wheeltexid);
    cyl->setRotation(-sin(ang),cos(ang),0,M_PI*0.5);
    cyl->setBodyRotation(-sin(ang),cos(ang),0,M_PI*0.5,true); //set local rotation matrix
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
    dJointSetAMotorParam(motor,dParamFMax,rob->cfg->Wheel_Motor_FMax());
    speed = 0;
}

void Robot::Wheel::step()
{
 //   if (id!=0)
    {
        dJointSetAMotorParam(motor,dParamVel,speed*(rob->cfg->motormaxoutput()*2.0f*M_PI)/(60.0f*rob->cfg->motormaxinput()));
        dJointSetAMotorParam(motor,dParamFMax,rob->cfg->Wheel_Motor_FMax());
    }
    //dJointSetHingeParam (joint,dParamVel,speed);
    //dJointSetHingeParam (joint,dParamFMax,5);
}

Robot::Kicker::Kicker(Robot* robot)
{
  rob = robot;

  float x = rob->m_x;
  float y = rob->m_y;
  float z = rob->m_z;
  float centerx = x+(rob->cfg->CHASSISLENGTH()*0.5+rob->cfg->KLENGTH()*0.5);
  float centery = y;
  float centerz = z-(rob->cfg->CHASSISHEIGHT())*0.5f+rob->cfg->WHEELRADIUS()-rob->cfg->BOTTOMHEIGHT()+rob->cfg->KICKERHEIGHT();
  box = new PBox(centerx,centery,centerz,rob->cfg->KLENGTH(),rob->cfg->KWIDTH(),rob->cfg->KHEIGHT(),rob->cfg->KICKERMASS(),0.9,0.9,0.9);
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

  rolling = false;
  kicking = false;
}

void Robot::Kicker::step()
{
    if (kicking)
    {
        box->setColor(1,0.3,0);
        kickstate--;
        if (kickstate<=0) kicking = false;
    }
    else if (rolling)
    {
        box->setColor(1,0.7,0);
        if (isTouchingBall())
        {
            float fx,fy,fz;
            rob->chassis->getBodyDirection(fx,fy,fz);
            fz = sqrt(fx*fx + fy*fy);
            fx/=fz;fy/=fz;
            rob->getBall()->tag = rob->getID();

            float vx,vy,vz;
            float bx,by,bz;
            float kx,ky,kz;
            rob->chassis->getBodyDirection(vx,vy,vz);
            rob->getBall()->getBodyPosition(bx,by,bz);
            box->getBodyPosition(kx,ky,kz);
            float yy = -((-(kx-bx)*vy + (ky-by)*vx)) / rob->cfg->KWIDTH();
            float dir = 1;
            if (yy>0) dir = -1.0f;
            //dBodySetAngularVel(rob->getBall()->body,fy*rob->cfg->ROLLERTORQUEFACTOR()*700,-fx*rob->cfg->ROLLERTORQUEFACTOR()*700,0);
            dBodyAddTorque(rob->getBall()->body,fy*rob->cfg->ROLLERTORQUEFACTOR(),-fx*rob->cfg->ROLLERTORQUEFACTOR(),0);
            dBodyAddTorque(rob->getBall()->body,yy*fx*rob->cfg->RollerPerpendicularTorqueFactor(),yy*fy*rob->cfg->RollerPerpendicularTorqueFactor(),0);
        }
    }
    else box->setColor(0.9,0.9,0.9);
}

bool Robot::Kicker::isTouchingBall()
{
  float vx,vy,vz;
  float bx,by,bz;
  float kx,ky,kz;
  rob->chassis->getBodyDirection(vx,vy,vz);  
  rob->getBall()->getBodyPosition(bx,by,bz);
  box->getBodyPosition(kx,ky,kz);
  float xx = fabs((kx-bx)*vx + (ky-by)*vy);
  float yy = fabs(-(kx-bx)*vy + (ky-by)*vx);
  float zz = fabs(kz-bz);
  return ((xx<rob->cfg->KLENGTH()*2.0f+rob->cfg->BALLRADIUS()) && (yy<rob->cfg->KWIDTH()*0.5f) && (zz<rob->cfg->KHEIGHT()*0.5f));
}

void Robot::Kicker::setRoller(bool roller)
{
    rolling = roller;
}

bool Robot::Kicker::getRoller()
{
    return rolling;
}

void Robot::Kicker::toggleRoller()
{
    rolling = !rolling;
}

void Robot::Kicker::kick(float kickspeed,bool chip)
{    
    float dx,dy,dz;
    float vx,vy,vz;
    rob->chassis->getBodyDirection(dx,dy,dz);    
    float zf = 0;
    if (chip) zf = kickspeed*rob->cfg->CHIPFACTOR();

    if (isTouchingBall())
    {
        double dlen = dx*dx+dy*dy+dz*dz;
        dlen = sqrt(dlen);
        vx = dx*kickspeed*rob->cfg->KICKFACTOR();
        vy = dy*kickspeed*rob->cfg->KICKFACTOR();
        vz = 0;
        const dReal* vball = dBodyGetLinearVel(rob->getBall()->body);
        double vn = -(vball[0]*dx + vball[1]*dy)*rob->cfg->kickerDampFactor();
        double vt = -(vball[0]*dy - vball[1]*dx);
        vx += vn * dx - vt * dy;
        vy += vn * dy + vt * dx;
        dBodySetLinearVel(rob->getBall()->body,vx,vy,zf);
    }
    kicking = true;kickstate=10;
  if (!kicking)
  {

/*    kicking = true;
    kickstate=1;
    m_kicktime  = (int) (KICK_MAX_ANGLE / kickspeed);
    m_kickspeed = KICK_MAX_ANGLE/m_kicktime;
    dJointSetHingeParam (joint,dParamLoStop,0);
    dJointSetHingeParam (joint,dParamHiStop,1);
*/

    //dJointSetHingeParam (joint,dParamFMax,0.5);
  }
}

Robot::Robot(PWorld* world,PBall *ball,ConfigWidget* _cfg,float x,float y,float z,float r,float g,float b,int rob_id,int wheeltexid,int dir)
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
  //resetSpeeds();

  space = w->space;
  //space = dSimpleSpaceCreate (w->space);
  //dSpaceSetCleanup (space,0);

  chassis = new PCylinder(x,y,z,cfg->CHASSISWIDTH()*0.5f,cfg->CHASSISHEIGHT(),cfg->CHASSISMASS()*0.99f,r,g,b,rob_id);
  chassis->space = space;
  w->addObject(chassis);

  dummy   = new PBall(x,y,z,cfg->CHASSISWIDTH()*0.5f,cfg->CHASSISMASS()*0.01f,0,0,0);
  dummy->setVisibility(false);
  dummy->space = space;
  w->addObject(dummy);

  dummy_to_chassis = dJointCreateFixed(world->world,0);
  dJointAttach (dummy_to_chassis,chassis->body,dummy->body);
  //dJointSetFixedParam(

  kicker = new Kicker(this);

  wheels[0] = new Wheel(this,0,cfg->Wheel1Angle(),cfg->Wheel1Angle(),wheeltexid);
  wheels[1] = new Wheel(this,1,cfg->Wheel2Angle(),cfg->Wheel2Angle(),wheeltexid);
  wheels[2] = new Wheel(this,2,cfg->Wheel3Angle(),cfg->Wheel3Angle(),wheeltexid);
  wheels[3] = new Wheel(this,3,cfg->Wheel4Angle(),cfg->Wheel4Angle(),wheeltexid);
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

int Robot::getID()
{
    return m_rob_id - 1;
}

void normalizeVector(float& x,float& y,float& z)
{
    float d = sqrt(x*x + y*y + z*z);
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
            kicker->setRoller(false);
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
    float fr_r,fr_b,fr_n;w->g->getFrustum(fr_r,fr_b,fr_n);
    const float txtWidth = 12.0f*fr_r/(float)w->g->getWidth();
    const float txtHeight = 24.0f*fr_b/(float)w->g->getHeight();
    pos[0] = dBodyGetPosition(chassis->body)[0];
    pos[1] = dBodyGetPosition(chassis->body)[1];
    pos[2] = dBodyGetPosition(chassis->body)[2];    
    float xyz[3],hpr[3];
    w->g->getViewpoint(xyz,hpr);
    float ax = -pos[0]+xyz[0];
    float ay = -pos[1]+xyz[1];
    float az = -pos[2]+xyz[2];
    float fx,fy,fz;
    float rx,ry,rz;
    w->g->getCameraForward(fx,fy,fz);
    w->g->getCameraRight(rx,ry,rz);
    normalizeVector(fx,fy,fz);
    normalizeVector(rx,ry,rz);
    float zz = fx*ax + fy*ay + fz*az;
    float zfact = zz/fr_n;
    pos[2] += cfg->CHASSISHEIGHT()*0.5f + cfg->BOTTOMHEIGHT() + cfg->WHEELRADIUS() + txtHeight*zfact;
    dMatrix3 rot;
    dRFromAxisAndAngle(rot,0,0,0,0);
    float tx = fy*rz-ry*fz;
    float ty = rx*fz-fx*rz;
    float tz = fx*ry-fy*rx;
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
  float x,y;
  getXY(x,y);
  setXY(x,y);
  if (m_dir==-1) setDir(180);
  else setDir(0);
}

void Robot::getXY(float& x,float &y)
{
    float xx,yy,zz;
    chassis->getBodyPosition(xx,yy,zz);
    x = xx;y = yy;
}

float Robot::getDir()
{
    float x,y,z;
    chassis->getBodyDirection(x,y,z);
    float dot = x;//zarb dar (1.0,0.0,0.0)
    float length = sqrt(x*x + y*y);
    float absAng = (float)(acos((double)(dot/length)) * (180.0f/M_PI));
    return (y > 0) ? absAng : -absAng;
}

void Robot::setXY(float x,float y)
{
    float xx,yy,zz,kx,ky,kz;
    float height = ROBOT_START_Z(cfg);
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

void Robot::setDir(float ang)
{
  ang*=M_PI/180.0f;
  chassis->setBodyRotation(0,0,1,ang);
  kicker->box->setBodyRotation(0,0,1,ang);      
  dummy->setBodyRotation(0,0,1,ang);
  dMatrix3 wLocalRot,wRot,cRot;
  dVector3 localPos,finalPos,cPos;
  chassis->getBodyPosition(cPos[0],cPos[1],cPos[2],false);
  chassis->getBodyRotation(cRot,false);  
  //cRot*
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

