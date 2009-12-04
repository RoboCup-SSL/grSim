#include "robot.h"
// ang2 = position angle
// ang  = rotation angle
Robot::Wheel::Wheel(Robot* robot,float ang,float ang2)
{
    rob = robot;
    float rad = CHASSISWIDTH*0.5 + WHEELLENGTH;
    ang *= M_PI/180.0f;
    ang2 *= M_PI/180.0f;
    float x = rob->m_x;
    float y = rob->m_y;
    float z = rob->m_z;
    cyl = new PCylinder(x+rad*cos(ang2),y+rad*sin(ang2),z-CHASSISHEIGHT*0.5+WHEELRADIUS-BOTTOMHEIGHT,WHEELRADIUS,WHEELLENGTH,WHEELMASS,0.9,0.9,0.9);
    cyl->setRotation(-sin(ang),cos(ang),0,M_PI*0.5);
    cyl->space = rob->space;

    rob->w->addObject(cyl);

    joint = dJointCreateHinge (rob->w->world,0);
//    joint = dJointCreateHinge2 (rob->w->world,0);


    dJointAttach (joint,rob->chassis->body,cyl->body);
    const dReal *a = dBodyGetPosition (cyl->body);
    dJointSetHingeAxis (joint,cos(ang),sin(ang),0);
    dJointSetHingeAnchor (joint,a[0],a[1],a[2]);
/*    dJointSetHinge2Axis2 (joint,cos(ang),sin(ang),0);
    dJointSetHinge2Axis1 (joint,0,0,1);
    dJointSetHinge2Anchor (joint,a[0],a[1],a[2]);

    dJointSetHinge2Param(joint,dParamLoStop,0);
    dJointSetHinge2Param(joint,dParamHiStop,0);
*/

//    dJointSetHinge2Param (joint,dParamSuspensionERP,0.04);
//    dJointSetHinge2Param (joint,dParamSuspensionCFM,0.08);
}

void Robot::Wheel::step()
{
    dJointSetHingeParam (joint,dParamVel,speed);
    dJointSetHingeParam (joint,dParamFMax,MAXWHEELSPEED);
//    dJointSetHinge2Param (joint,dParamVel2,speed);
//    dJointSetHinge2Param (joint,dParamFMax2,MAXWHEELSPEED);
}

Robot::Kicker::Kicker(Robot* robot)
{
  rob = robot;

  float x = rob->m_x;
  float y = rob->m_y;
  float z = rob->m_z;
  box = new PBox(x+(CHASSISLENGTH*0.5+KLENGTH*0.5),y,z-(CHASSISHEIGHT)*0.5f+WHEELRADIUS-BOTTOMHEIGHT+KICKERHEIGHT
                ,KLENGTH,KWIDTH,KHEIGHT,KICKERMASS,0.9,0.9,0.9);
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
            dBodyAddTorque(rob->getBall()->body,fy*ROLLERTORQUEFACTOR,-fx*ROLLERTORQUEFACTOR,0);
        }
    }
    else box->setColor(0.9,0.9,0.9);
/*    if (kicking)
    {
      if (kickstate<=m_kicktime)
      {
        dJointSetHingeParam (joint,dParamVel,m_kickspeed);
        dJointSetHingeParam (joint,dParamFMax,KICKERFMAX);
        kickstate++;
      }
      if ((kickstate>m_kicktime) && (kickstate<=m_kicktime*2))
      {
        dJointSetHingeParam (joint,dParamVel,-m_kickspeed);
        dJointSetHingeParam (joint,dParamFMax,KICKERFMAX);
        kickstate++;
      }
      if (kickstate>m_kicktime*2)
      {
        dJointSetHingeParam (joint,dParamVel,0);
        dJointSetHingeParam (joint,dParamFMax,0);
        dJointSetHingeParam (joint,dParamLoStop,0);
        dJointSetHingeParam (joint,dParamHiStop,0);
        kicking=false;
      }
    }*/
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
  return ((xx<KLENGTH*2.0f+BALLRADIUS) && (yy<KWIDTH*0.5f) && (zz<KHEIGHT*0.5f));
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
    if (chip) zf = kickspeed*CHIPFACTOR;
    if (isTouchingBall())
        dBodyAddForce(rob->getBall()->body,vx*kickspeed*KICKFACTOR,vy*kickspeed*KICKFACTOR,zf);
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

Robot::Robot(PWorld* world,PBall *ball,float x,float y,float z,float r,float g,float b,int rob_id)
{  
  m_r = r;
  m_g = g;
  m_b = b;
  m_x = x;
  m_y = y;
  m_z = z;
  w = world;
  m_ball = ball;
  //resetSpeeds();

  space = w->space;
  //space = dSimpleSpaceCreate (w->space);
  //dSpaceSetCleanup (space,0);

  chassis = new PCylinder(x,y,z,CHASSISWIDTH*0.5f,CHASSISHEIGHT,CHASSISMASS*0.5f,r,g,b,rob_id);
  chassis->space = space;
  w->addObject(chassis);

  dummy   = new PBall(x,y,z,CHASSISWIDTH*0.5f,CHASSISMASS*0.5f,0,0,0);
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
/*
  float pos[3][2] = {{0,-CHASSISWIDTH*0.5f},{0,CHASSISWIDTH*0.5f},{-CHASSISWIDTH*0.5f,0}};
  float dim[3][2] = {{0.05,0.01},{0.05,0.01},{0.01,0.05}};
  for (int i=0;i<3;i++)
  {
      boxes[i] = new PBox(x+pos[i][0],y+pos[i][1],z,dim[i][0],dim[i][1],0.1,0.05,0.4,0.4,0.4);
      boxes[i]->setVisibility(false);
      w->addObject(boxes[i]);
      dJointID joint = dJointCreateHinge (w->world,0);
      dJointAttach (joint,chassis->body,boxes[i]->body);
      dJointSetHingeAnchor (joint,x+pos[i][0],y+pos[i][1],z);
      dJointSetHingeAxis(joint,0,0,1);
      dJointSetHingeParam (joint,dParamLoStop,0);
      dJointSetHingeParam (joint,dParamHiStop,0);
  }*/
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

