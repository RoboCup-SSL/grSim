#include "pobject.h"

PObject::PObject(float x,float y,float z,float red,float green,float blue)
{
    geom = NULL;
    body = NULL;
    world = NULL;
    space = NULL;
    m_x = x;
    m_y = y;
    m_z = z;
    m_red = red;
    m_green = green;
    m_blue = blue;
    visible = true;
    isQSet = false;
}

void PObject::setVisibility(bool v)
{
    visible = v;
}

bool PObject::getVisibility()
{
    return visible;
}

void PObject::setColor(float r,float g,float b)
{
    m_red = r;
    m_green = g;
    m_blue = b;
}

void PObject::getColor(float& r,float& g,float& b)
{
    r = m_red;
    g = m_green;
    b = m_blue;
}


PObject::~PObject()
{
    if (geom!=NULL) dGeomDestroy(geom);
    if (body!=NULL) dBodyDestroy(body);
}

void PObject::setRotation(float x_axis,float y_axis,float z_axis,float ang)
{
    dQFromAxisAndAngle (q,x_axis,y_axis,z_axis,ang);
    isQSet = true;
}

void PObject::setBodyPosition(float x,float y,float z)
{
    dBodySetPosition(body,x,y,z);
}

void PObject::setBodyRotation(float x_axis,float y_axis,float z_axis,float ang)
{
    dQFromAxisAndAngle (q,x_axis,y_axis,z_axis,ang);
    dBodySetQuaternion(body,q);
}

void PObject::getBodyPosition(float &x,float &y,float &z)
{
    const dReal *r=dBodyGetPosition(body);
    x = r[0];
    y = r[1];
    z = r[2];
}

void PObject::getBodyDirection(float &x,float &y,float &z)
{
  const dReal *r=dBodyGetRotation(body);
  dVector3 v={1,0,0};
  dVector3 axis;
  dMultiply0(axis,r,v,4,3,1);
  x = axis[0];
  y = axis[1];
  z = axis[2];
}

void PObject::initPosBody()
{
    dBodySetPosition(body,m_x,m_y,m_z);
    if (isQSet) dBodySetQuaternion(body,q);
}

void PObject::initPosGeom()
{
    dGeomSetPosition(geom,m_x,m_y,m_z);
    if (isQSet) dGeomSetQuaternion(geom,q);
}

void PObject::glinit()
{
}

void PObject::draw()
{
    g->setColor(m_red,m_green,m_blue,1);
}

