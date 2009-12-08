#ifndef PBOX_H
#define PBOX_H


#include "pobject.h"

class PBox : public PObject
{
private:
    float m_w,m_h,m_l;
public:
    PBox(float x,float y,float z,float w,float h,float l,float mass,float r,float g,float b);
    virtual ~PBox();
    virtual void setMass(float mass);
    virtual void init();
    virtual void draw();
};

#endif // PBOX_H
