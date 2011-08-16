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

#include "pball.h"

PBall::PBall(dReal x,dReal y,dReal z,dReal radius,dReal mass,dReal red,dReal green,dReal blue)
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

void PBall::setMass(dReal mass)
{

    m_mass = mass;
    dMass m;
    dMassSetSphereTotal(&m,m_mass,m_radius);
    dBodySetMass (body,&m);
}

void PBall::draw()
{
  PObject::draw();
  g->drawSphere(dBodyGetPosition(body),dBodyGetRotation(body),m_radius);
}
