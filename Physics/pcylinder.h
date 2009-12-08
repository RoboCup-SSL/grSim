#ifndef PCYLINDER_H
#define PCYLINDER_H

#include "pobject.h"

class PCylinder : public PObject
{
private:
    float m_radius,m_length;
    GLuint m_gllistid;
    int m_texid;
public:
    PCylinder(float x,float y,float z,float radius,float length,float mass,float red,float green,float blue,int tex_id=-1);
    virtual ~PCylinder();
    virtual void setMass(float mass);
    virtual void init();
    virtual void draw();
};

#endif // PCYLINDER_H
