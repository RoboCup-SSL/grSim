#ifndef PWORLD_H
#define PWORLD_H

#include "pobject.h"
#include <QMap>
#include <QVector>

class PSurface;
class PWorld
{
private:
    dJointGroupID contactgroup;
    QVector<PObject*> objects;
    QVector<PSurface*> surfaces;
    float delta_time;
public:
    PWorld(float dt,float gravity,CGraphics* graphics);
    ~PWorld();
    void setGravity(float gravity);
    void addObject(PObject* o);
    PSurface* createSurface(PObject* o1,PObject* o2);
    void step(float dt=-1);
    void glinit();
    void draw();
    void handleCollisions(dGeomID o1, dGeomID o2);
    dWorldID world;
    dSpaceID space;
    CGraphics* g;
};

typedef bool PSurfaceCallback(dGeomID o1,dGeomID o2,PSurface* s);
class PSurface
{
public:
    PSurface();
    dSurfaceParameters surface;
    bool isIt(dGeomID i1,dGeomID i2);
    dGeomID id1,id2;
    bool usefdir1;   //if true use fdir1 instead of ODE value
    dVector3 fdir1;  //fdir1 is a normalized vector tangent to friction force vector
    dVector3 contactPos,contactNormal;
    PSurfaceCallback* callback;
};
#endif // PWORLD_H
