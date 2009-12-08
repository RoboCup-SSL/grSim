#ifndef PGROUND_H
#define PGROUND_H

#include "pobject.h"

class PGround : public PObject
{
private:
    float rad,len,wid;
    int tex;
public:
    PGround(float field_radius,float field_length,float field_width,int tex_id);
    virtual ~PGround();
    virtual void init();
    virtual void draw();
};

#endif // PGROUND_H
