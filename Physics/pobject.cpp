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

#include "pobject.h"

PObject::PObject(float x,float y,float z,float red,float green,float blue,float mass)
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
    m_mass = mass;
    visible = true;
    isQSet = false;
    tag = 0;
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

void PObject::setBodyPosition(float x,float y,float z,bool local)
{
    if (!local) dBodySetPosition(body,x,y,z);
    else {local_Pos[0]=x;local_Pos[1]=y;local_Pos[2]=z;}
}

void PObject::setBodyRotation(float x_axis,float y_axis,float z_axis,float ang,bool local)
{
    if (!local)
    {
        dQFromAxisAndAngle (q,x_axis,y_axis,z_axis,ang);
        dBodySetQuaternion(body,q);
    }
    else {
        dRFromAxisAndAngle(local_Rot,x_axis,y_axis,z_axis,ang);
    }
}

void PObject::getBodyPosition(float &x,float &y,float &z,bool local)
{
    if (local) {
        x = local_Pos[0];
        y = local_Pos[1];
        z = local_Pos[2];
        return;
    }
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

void PObject::getBodyRotation(dMatrix3 r,bool local)
{
    if (local)
    {
        for (int k=0;k<12;k++) r[k] = local_Rot[k];
    }
    else {
        const dReal* rr = dBodyGetRotation(body);
        for (int k=0;k<12;k++) r[k] = rr[k];
    }
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


void PObject::setMass(float mass)
{
    m_mass = mass;
}

void PObject::glinit()
{
}

void PObject::draw()
{
    g->setColor(m_red,m_green,m_blue,1);
}

