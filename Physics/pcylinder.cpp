#include "pcylinder.h"

PCylinder::PCylinder(float x,float y,float z,float radius,float length,float mass,float red,float green,float blue,bool sphere_body,int texid)
          : PObject(x,y,z,red,green,blue)
{
    m_radius = radius;
    m_length = length;
    m_mass = mass;
    m_sphere = sphere_body;
    m_texid = texid;
}


void PCylinder::init()
{
  dMass m;
  body = dBodyCreate (world);
  initPosBody();
  if (!m_sphere)
  {
      dMassSetCylinder (&m,1,1,m_radius,m_length);
      dMassAdjust (&m,m_mass);
      dBodySetMass (body,&m);
      geom = dCreateCylinder(0,m_radius,m_length);
  }
  else
  {
      dMassSetSphere(&m,1,m_radius);
      dMassAdjust (&m,m_mass);
      dBodySetMass (body,&m);
      geom = dCreateSphere(0,m_radius);
  }
  dGeomSetBody (geom,body);
  dSpaceAdd (space,geom);
}

void PCylinder::draw()
{
    PObject::draw();    
    if (m_texid==-1)
        g->drawCylinder(dBodyGetPosition(body),dBodyGetRotation(body),m_length,m_radius);
    else
        g->drawCylinder_TopTextured(dBodyGetPosition(body),dBodyGetRotation(body),m_length,m_radius,m_texid);
}
