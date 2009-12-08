#include "pball.h"

PBall::PBall(float x,float y,float z,float radius,float mass,float red,float green,float blue)
        : PObject(x,y,z,red,green,blue,mass)
{
    m_radius = radius;    
}

PBall::~PBall()
{
}

void PBall::init()
{
    body = dBodyCreate (world);
    initPosBody();
    setMass(m_mass);
    geom = dCreateSphere (0,m_radius);
    dGeomSetBody (geom,body);
    dSpaceAdd(space,geom);
}

void PBall::setMass(float mass)
{
    m_mass = mass;
    dMass m;
    dMassSetSphere (&m,1,m_radius);
    dMassAdjust (&m,m_mass);
    dBodySetMass (body,&m);
}

void PBall::draw()
{
  PObject::draw();
  g->drawSphere(dBodyGetPosition(body),dBodyGetRotation(body),m_radius);
}
