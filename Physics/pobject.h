#ifndef POBJECT_H
#define POBJECT_H

#include "../include/ode/ode.h"
#include "../Graphics/graphics.h"

class PObject
{
private:
    bool isQSet;
protected:
    float m_x,m_y,m_z,m_red,m_green,m_blue;
    dQuaternion q;
    void initPosBody();
    void initPosGeom();
    bool visible;
public:
    PObject(float x,float y,float z,float red,float green,float blue);
    ~PObject();
    void setRotation(float x_axis,float y_axis,float z_axis,float ang); //Must be called before init()
    void setBodyPosition(float x,float y,float z);
    void setBodyRotation(float x_axis,float y_axis,float z_axis,float ang);
    void getBodyPosition(float &x,float &y,float &z);
    void getBodyDirection(float &x,float &y,float &z);
    void setVisibility(bool v);
    void setColor(float r,float g,float b);
    void getColor(float& r,float& g,float& b);
    bool getVisibility();
    virtual void init()=0;
    virtual void glinit();
    virtual void draw();

    dBodyID body;
    dGeomID geom;
    dWorldID world;
    dSpaceID space;
    CGraphics *g;
    int id;
};

#endif // POBJECT_H
