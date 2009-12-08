#include "robot.h"

#define WHEEL_FMAX 5    // fmax value for the wheel


// ang2 = position angle
// ang  = rotation angle
Robot::Wheel::Wheel(Robot* robot,float ang,float ang2)
{
    rob = robot;
    float rad = rob->cfg->CHASSISWIDTH()*0.5 + rob->cfg->WHEELLENGTH();
    ang *= M_PI/180.0f;
    ang2 *= M_PI/180.0f;
    float x = rob->m_x;
    float y = rob->m_y;
    float z = rob->m_z;
    cyl = new PCylinder(x+rad*cos(ang2),y+rad*sin(ang2),z-rob->cfg->CHASSISHEIGHT()*0.5+rob->cfg->WHEELRADIUS()-rob->cfg->BOTTOMHEIGHT(),rob->cfg->WHEELRADIUS(),rob->cfg->WHEELLENGTH(),rob->cfg->WHEELMASS(),0.9,0.9,0.9);
    cyl->setRotation(-sin(ang),cos(ang),0,M_PI*0.5);
    cyl->space = rob->space;

    rob->w->addObject(cyl);

    joint = dJointCreateHinge (rob->w->world,0);

    dJointAttach (joint,rob->chassis->body,cyl->body);
    const dReal *a = dBodyGetPosition (cyl->body);
    dJointSetHingeAxis (joint,cos(ang),sin(ang),0);
    dJointSetHingeAnchor (joint,a[0],a[1],a[2]);
}

void Robot::Wheel::step()
{
    dJointSetHingeParam (joint,dParamVel,speed);
    dJointSetHingeParam (joint,dParamFMax,WHEEL_FMAX);
}

Robot::Kicker::Kicker(Robot* robot)
{
  rob = robot;

  float x = rob->m_x;
  float y = rob->m_y;
  float z = rob->m_z;
  box = new PBox(x+(rob->cfg->CHASSISLENGTH()*0.5+rob->cfg->KLENGTH()*0.5),y,z-(rob->cfg->CHASSISHEIGHT())*0.5f+rob->cfg->WHEELRADIUS()-rob->cfg->BOTTOMHEIGHT()+rob->cfg->KICKERHEIGHT()
                ,rob->cfg->KLENGTH(),rob->cfg->KWIDTH(),rob->cfg->KHEIGHT(),rob->cfg->KICKERMASS(),0.9,0.9,0.9);
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
            //dBodySetAngularVel(rob->getBall()->body,fy*ROLLERTORQUEFACTOR*700,-fx*ROLLERTORQUEFACTOR*700,0);
            dBodyAddTorque(rob->getBall()->body,fy*rob->cfg->ROLLERTORQUEFACTOR(),-fx*rob->cfg->ROLLERTORQUEFACTOR(),0);
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
    float vx,vy,vz;
    rob->chassis->getBodyDirection(vx,vy,vz);
    float zf = 0;
    if (chip) zf = kickspeed*rob->cfg->CHIPFACTOR();

    if (isTouchingBall())
        dBodyAddForce(rob->getBall()->body,vx*kickspeed*rob->cfg->KICKFACTOR(),vy*kickspeed*rob->cfg->KICKFACTOR(),zf);
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

Robot::Robot(PWorld* world,PBall *ball,ConfigWidget* _cfg,float x,float y,float z,float r,float g,float b,int rob_id)
{      
  m_r = r;
  m_g = g;
  m_b = b;
  m_x = x;
  m_y = y;
  m_z = z;
  w = world;
  m_ball = ball;
  cfg = _cfg;

  //resetSpeeds();

  space = w->space;
  //space = dSimpleSpaceCreate (w->space);
  //dSpaceSetCleanup (space,0);

  chassis = new PCylinder(x,y,z,cfg->CHASSISWIDTH()*0.5f,cfg->CHASSISHEIGHT(),cfg->CHASSISMASS()*0.5f,r,g,b,rob_id);
  chassis->space = space;
  w->addObject(chassis);

  dummy   = new PBall(x,y,z,cfg->CHASSISWIDTH()*0.5f,cfg->CHASSISMASS()*0.5f,0,0,0);
  dummy->setVisibility(false);
  dummy->space = space;
  w->addObject(dummy);

  dummy_to_chassis = dJointCreateFixed(world->world,0);
  dJointAttach (dummy_to_chassis,chassis->body,dummy->body);
  //dJointSetFixedParam(

  kicker = new Kicker(this);

  wheels[0] = new Wheel(this,60,60);
  wheels[1] = new Wheel(this,135,135);
  wheels[2] = new Wheel(this,225,225);
  wheels[3] = new Wheel(this,300,300);
}

Robot::~Robot()
{

}

PBall* Robot::getBall()
{
    return m_ball;
}

void Robot::step()
{
    wheels[0]->step();
    wheels[1]->step();
    wheels[2]->step();
    wheels[3]->step();
    kicker->step();

}

void Robot::resetSpeeds()
{
  wheels[0]->speed = wheels[1]->speed = wheels[2]->speed = wheels[3]->speed = 0;
}


void Robot::resetRobot()
{
  resetSpeeds();
  float x,y;
  getXY(x,y);
  setXY(x,y);

  chassis->setBodyRotation(0,0,0,M_PI*0.5);
  kicker->box->setBodyRotation(0,0,0,M_PI*0.5);
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
    chassis->getBodyPosition(xx,yy,zz);
    chassis->setBodyPosition(x,y,0.4);
    dummy->setBodyPosition(x,y,0.4);
    kicker->box->getBodyPosition(kx,ky,kz);
    kicker->box->setBodyPosition(kx-xx+x,ky-yy+y,kz-zz+0.4);
    for (int i=0;i<4;i++)
    {
        wheels[i]->cyl->getBodyPosition(kx,ky,kz);
        wheels[i]->cyl->setBodyPosition(kx-xx+x,ky-yy+y,kz-zz+0.4);
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

