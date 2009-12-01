#ifndef PBALL_H
#define PBALL_H

#include "pobject.h"

class PBall : public PObject
{
private:
    float m_radius,m_mass;
public:
    PBall(float x,float y,float z,float radius,float mass,float red,float green,float blue);
    virtual void init();
    virtual void draw();
};

#endif // PBALL_H
