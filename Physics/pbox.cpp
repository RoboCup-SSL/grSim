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

#include "pbox.h"

PBox::PBox(dReal x,dReal y,dReal z,dReal w,dReal h,dReal l,dReal mass,dReal r,dReal g,dReal b)
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

void PBox::setMass(dReal mass)
{
  m_mass = mass;
  dMass m;
  dMassSetBoxTotal (&m,m_mass,m_w,m_h,m_l);
  dBodySetMass (body,&m);
}

void PBox::draw()
{
    PObject::draw();
    dReal dim[3] = {m_w,m_h,m_l};
    g->drawBox (dGeomGetPosition(geom),dGeomGetRotation(geom),dim);
}
