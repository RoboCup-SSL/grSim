#ifndef PBALL_H
#define PBALL_H

#include "pobject.h"

class PBall : public PObject
{
private:
    float m_radius;
public:
    PBall(float x,float y,float z,float radius,float mass,float red,float green,float blue);
    virtual ~PBall();
    virtual void setMass(float mass);
    virtual void init();
    virtual void draw();
};

#endif // PBALL_H
