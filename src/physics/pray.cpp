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

#include "pray.h"

PRay::PRay(dReal length)
         : PObject(0,0,0,0,0,0,0)
{
    _length = length;
}

void PRay::init()
{
    geom = dCreateRay(space,_length);
}

void PRay::setPose(dReal x,dReal y,dReal z,dReal dx,dReal dy,dReal dz)
{
    dGeomRaySet(geom,x,y,z,dx,dy,dz);
}
