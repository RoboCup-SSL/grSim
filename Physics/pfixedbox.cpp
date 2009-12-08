#include "pfixedbox.h"

PFixedBox::PFixedBox(float x,float y,float z,float w,float h,float l,float r,float g,float b)
       : PObject(x,y,z,r,g,b,0)
{
    m_w = w;
    m_h = h;
    m_l = l;
}

PFixedBox::~PFixedBox()
{
}

void PFixedBox::init()
{
    geom = dCreateBox (space,m_w,m_h,m_l);
    initPosGeom();
}

void PFixedBox::draw()
{
    PObject::draw();
    float dim[3] = {m_w,m_h,m_l};
    g->drawBox (dGeomGetPosition(geom),dGeomGetRotation(geom),dim);
}
