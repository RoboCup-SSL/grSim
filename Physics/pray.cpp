#include "pray.h"

PRay::PRay(float length)
         : PObject(0,0,0,0,0,0,0)
{
    _length = length;
}

void PRay::init()
{
    geom = dCreateRay(space,_length);
}

void PRay::setPose(float x,float y,float z,float dx,float dy,float dz)
{
    dGeomRaySet(geom,x,y,z,dx,dy,dz);
}
