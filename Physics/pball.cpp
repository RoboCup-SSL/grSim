#include "pball.h"

PBall::PBall(float x,float y,float z,float radius,float mass,float red,float green,float blue)
        : PObject(x,y,z,red,green,blue)
{
    m_radius = radius;
    m_mass = mass;
}

void PBall::init()
{
    dMass m;
    body = dBodyCreate (world);
    initPosBody();
    dMassSetSphere (&m,1,m_radius);
    dMassAdjust (&m,m_mass);
    dBodySetMass (body,&m);
    geom = dCreateSphere (0,m_radius);
    dGeomSetBody (geom,body);
    dSpaceAdd(space,geom);
}

void PBall::draw()
{
  PObject::draw();
  g->drawSphere(dBodyGetPosition(body),dBodyGetRotation(body),m_radius);
}
