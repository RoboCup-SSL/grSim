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

#ifndef PBALL_H
#define PBALL_H

#include "pobject.h"

class PBall : public PObject
{
private:
    dReal m_radius;
public:
    PBall(dReal x,dReal y,dReal z,dReal radius,dReal mass,dReal red,dReal green,dReal blue);
    virtual ~PBall();
    virtual void setMass(dReal mass);
    virtual void init();
    virtual void draw();
};

#endif // PBALL_H
