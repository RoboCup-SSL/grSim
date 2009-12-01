#ifndef PBOX_H
#define PBOX_H


#include "pobject.h"

class PBox : public PObject
{
private:
    float m_w,m_h,m_l,m_mass;
public:
    PBox(float x,float y,float z,float w,float h,float l,float mass,float r,float g,float b);
    virtual void init();
    virtual void draw();
};

#endif // PBOX_H
