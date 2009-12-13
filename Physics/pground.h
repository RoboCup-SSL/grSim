#ifndef PGROUND_H
#define PGROUND_H

#include "pobject.h"

class PGround : public PObject
{
private:
    float rad,len,wid,prad,plen;
    int tex;
public:
    PGround(float field_radius,float field_length,float field_width,float field_penalty_rad,float field_penalty_line_length,int tex_id);
    virtual ~PGround();
    virtual void init();
    virtual void draw();
};

#endif // PGROUND_H
