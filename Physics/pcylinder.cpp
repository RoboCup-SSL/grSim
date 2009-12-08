#include "pcylinder.h"

PCylinder::PCylinder(float x,float y,float z,float radius,float length,float mass,float red,float green,float blue,int texid)
          : PObject(x,y,z,red,green,blue,mass)
{
    m_radius = radius;
    m_length = length;    
    m_texid = texid;
}

PCylinder::~PCylinder()
{
}


void PCylinder::setMass(float mass)
{
  m_mass = mass;
  dMass m;
  dMassSetCylinder (&m,1,1,m_radius,m_length);
  dMassAdjust (&m,m_mass);
  dBodySetMass (body,&m);
}

void PCylinder::init()
{
  body = dBodyCreate (world);
  initPosBody();
  setMass(m_mass);
  geom = dCreateCylinder(0,m_radius,m_length);
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

/*    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();

        g->setTransform(dBodyGetPosition(body),dBodyGetRotation(body));
           glScaled(m_radius, m_radius, m_radius);
           glColor3d(0.1, 0.1, 0.1);
           glRasterPos3f(-0.25, 1.5, 0.0);
           //glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
           glutBitmapCharacter(GLUT_BITMAP_8_BY_13,c);
    glPopMatrix();*/
}
