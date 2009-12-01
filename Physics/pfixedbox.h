#ifndef PFIXEDBOX_H
#define PFIXEDBOX_H

#include "pobject.h"

class PFixedBox : public PObject
{
private:
    float m_w,m_h,m_l;
public:
    PFixedBox(float x,float y,float z,float w,float h,float l,float r,float g,float b);
    virtual void init();
    virtual void draw();
};

#endif // PFIXEDBOX_H
