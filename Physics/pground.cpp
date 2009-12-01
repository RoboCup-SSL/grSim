#include "pground.h"

PGround::PGround(float field_radius,float field_length,float field_width,int tex_id)
        : PObject(0,0,0,0,1,0)
{
    rad = field_radius;
    len = field_length;
    wid = field_width;
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
    g->drawSSLGround(rad,len,wid,0.001);
}
