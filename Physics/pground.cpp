#include "pground.h"

PGround::PGround(float field_radius,float field_length,float field_width,float field_penalty_rad,float field_penalty_line_length,int tex_id)
        : PObject(0,0,0,0,1,0,0)
{
    rad = field_radius;
    len = field_length;
    wid = field_width;
    prad = field_penalty_rad;
    plen = field_penalty_line_length;
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
    g->drawSSLGround(rad,len,wid,prad,plen,0.001);
}

PGround::~PGround()
{
}
