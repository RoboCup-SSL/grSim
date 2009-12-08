#include "pbox.h"

PBox::PBox(float x,float y,float z,float w,float h,float l,float mass,float r,float g,float b)
       : PObject(x,y,z,r,g,b,mass)
{
    m_w = w;
    m_h = h;
    m_l = l;
}

PBox::~PBox()
{
}

void PBox::init()
{
  body = dBodyCreate (world);
  initPosBody();
  setMass(m_mass);
  geom = dCreateBox (0,m_w,m_h,m_l);
  dGeomSetBody (geom,body);
  dSpaceAdd (space,geom);
}

void PBox::setMass(float mass)
{
  m_mass = mass;
  dMass m;
  dMassSetBox (&m,1,m_w,m_h,m_l);
  dMassAdjust (&m,m_mass);
  dBodySetMass (body,&m);
}

void PBox::draw()
{
    PObject::draw();
    float dim[3] = {m_w,m_h,m_l};
    g->drawBox (dGeomGetPosition(geom),dGeomGetRotation(geom),dim);
}
