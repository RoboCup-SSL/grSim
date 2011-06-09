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

#include "pground.h"

PGround::PGround(float field_radius,float field_length,float field_width,float field_penalty_rad,float field_penalty_line_length,float field_penalty_point,int tex_id)
        : PObject(0,0,0,0,1,0,0)
{
    rad = field_radius;
    len = field_length;
    wid = field_width;
    prad = field_penalty_rad;
    plen = field_penalty_line_length;
    ppoint = field_penalty_point;
    tex = tex_id;
}

void PGround::init()
{
    geom = dCreatePlane (space,0,0,1,0);
}

void PGround::draw()
{
    PObject::draw();
    g->useTexture(tex);
    g->drawGround();
    g->noTexture();
    g->drawSSLGround(rad,len,wid,prad,plen,ppoint,0.001);
}

PGround::~PGround()
{
}
