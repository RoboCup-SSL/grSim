#include "pworld.h"

PSurface::PSurface()
{
  callback = NULL;
  usefdir1 = false;
  surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1;
  //                          dContactSoftERP | dContactSoftCFM ;
  surface.mu = dInfinity;
  surface.slip1 = 1;//0.1;
  surface.slip2 = 1;//0.1;
  //surface.soft_erp = 0.05;
  //surface.soft_cfm = 0.03;
}
bool PSurface::isIt(dGeomID i1,dGeomID i2)
{
    return ((i1==id1) && (i2==id2)) || ((i1==id2) && (i2==id1));
}


void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
  ((PWorld*) data)->handleCollisions(o1,o2);
}


PWorld::PWorld(float dt,float gravity,CGraphics* graphics)
{
    dInitODE2(0);
    world = dWorldCreate();
    space = dHashSpaceCreate (0);
    contactgroup = dJointGroupCreate (0);
    dWorldSetGravity (world,0,0,-gravity);


    dAllocateODEDataForThread(dAllocateMaskAll);
    delta_time = dt;
    g = graphics;
}

PWorld::~PWorld()
{
  dJointGroupDestroy (contactgroup);
  dSpaceDestroy (space);
  dWorldDestroy (world);
  dCloseODE();
}

void PWorld::handleCollisions(dGeomID o1, dGeomID o2)
{
//  if (dGeomGetSpace(o1)==dGeomGetSpace(o2)) return;
  PSurface* sur;
  for (int j=0;j<surfaces.count();j++)
    if (surfaces[j]->isIt(o1,o2))
    {
        sur = surfaces[j];
        if (sur->callback!=NULL) sur->callback(o1,o2,sur);
        const int N = 10;
        dContact contact[N];
        int n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));
        if (n > 0) {
          for (int i=0; i<n; i++) {
              contact[i].surface = sur->surface;
              if (sur->usefdir1)
              {
                  contact[i].fdir1[0] = sur->fdir1[0];
                  contact[i].fdir1[1] = sur->fdir1[1];
                  contact[i].fdir1[2] = sur->fdir1[2];
                  contact[i].fdir1[3] = sur->fdir1[3];
                  //printf("using (%f,%f,%f)\n",sur->fdir1[0],sur->fdir1[1],sur->fdir1[2]);
//                  printf("contact: %d %d\n",i,j);
              }
              dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);

              dJointAttach (c,
                            dGeomGetBody(contact[i].geom.g1),
                            dGeomGetBody(contact[i].geom.g2));
            }
        }

        break;
    }

}

void PWorld::addObject(PObject* o)
{
    int id = objects.count();
    o->id = id;
    if (o->world==NULL) o->world = world;
    if (o->space==NULL) o->space = space;
    o->g = g;
    o->init();
    objects.append(o);
}

PSurface* PWorld::createSurface(PObject* o1,PObject* o2)
{
    PSurface *s = new PSurface();
    s->id1 = o1->geom;
    s->id2 = o2->geom;
    surfaces.append(s);
    return s;
}

void PWorld::step()
{
    dSpaceCollide (space,this,&nearCallback);
    dWorldStep (world,delta_time);
    dJointGroupEmpty (contactgroup);
}

void PWorld::draw()
{
    for (int i=0;i<objects.count();i++)
        if (objects[i]->getVisibility()) objects[i]->draw();
}

void PWorld::glinit()
{
    for (int i=0;i<objects.count();i++)
        objects[i]->glinit();
}

