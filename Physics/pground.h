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

#ifndef PGROUND_H
#define PGROUND_H

#include "pobject.h"

class PGround : public PObject
{
private:
    float rad,len,wid,prad,plen,ppoint;
    int tex;
public:
    PGround(float field_radius,float field_length,float field_width,float field_penalty_rad,float field_penalty_line_length,float field_penalty_point,int tex_id);
    virtual ~PGround();
    virtual void init();
    virtual void draw();
};

#endif // PGROUND_H
