/*
grSim - RoboCup Small Size Soccer Robots Simulator
Copyright (C) 2011, Parsian Robotic Center (eew.aut.ac.ir/~parsian/grsim)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pworld.h"
PSurface::PSurface()
{
  callback = NULL;
  usefdir1 = false;
  surface.mode = dContactApprox1;
  surface.mu = 0.5;
}
bool PSurface::isIt(dGeomID i1,dGeomID i2)
{
    return ((i1==id1) && (i2==id2)) || ((i1==id2) && (i2==id1));
}


void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
  ((PWorld*) data)->handleCollisions(o1,o2);
}


PWorld::PWorld(dReal dt,dReal gravity,CGraphics* graphics, int _robot_count)
{
    robot_count = _robot_count;
    //dInitODE2(0);
    dInitODE();
    world = dWorldCreate();
    space = dHashSpaceCreate (0);
    contactgroup = dJointGroupCreate (0);
    dWorldSetGravity (world,0,0,-gravity);
    objects_count = 0;
    sur_matrix = NULL;
    //dAllocateODEDataForThread(dAllocateMaskAll);
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

void PWorld::setGravity(dReal gravity)
{
    dWorldSetGravity (world,0,0,-gravity);
}

void PWorld::handleCollisions(dGeomID o1, dGeomID o2)
{   
    PSurface* sur;
    int j=sur_matrix[*((int*)(dGeomGetData(o1)))][*((int*)(dGeomGetData(o2)))];
    if (j!=-1)
    {
        const int N = 10;
        dContact contact[N];
        int n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));
        if (n > 0) {
          sur = surfaces[j];
          sur->contactPos   [0] = contact[0].geom.pos[0];
          sur->contactPos   [1] = contact[0].geom.pos[1];
          sur->contactPos   [2] = contact[0].geom.pos[2];
          sur->contactNormal[0] = contact[0].geom.normal[0];
          sur->contactNormal[1] = contact[0].geom.normal[1];
          sur->contactNormal[2] = contact[0].geom.normal[2];
          bool flag=true;
          if (sur->callback!=NULL) flag = sur->callback(o1,o2,sur,robot_count);
          if (flag)
          for (int i=0; i<n; i++) {
              contact[i].surface = sur->surface;
              if (sur->usefdir1)
              {
                  contact[i].fdir1[0] = sur->fdir1[0];
                  contact[i].fdir1[1] = sur->fdir1[1];
                  contact[i].fdir1[2] = sur->fdir1[2];
                  contact[i].fdir1[3] = sur->fdir1[3];
              }
              dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);

              dJointAttach (c,
                            dGeomGetBody(contact[i].geom.g1),
                            dGeomGetBody(contact[i].geom.g2));
            }
        }
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
    dGeomSetData(o->geom,(void*)(&(o->id)));
    objects.append(o);
}

void PWorld::initAllObjects()
{
    objects_count = objects.count();
    int c = objects_count;
    bool flag = false;
    if (sur_matrix!=NULL)
    {
        for (int i=0;i<c;i++)
            delete sur_matrix[i];
        delete sur_matrix;
        flag = true;
    }    
    sur_matrix = new int* [c];
    for (int i=0;i<c;i++)
    {
        sur_matrix[i] = new int [c];    
        for (int j=0;j<c;j++)
            sur_matrix[i][j] = -1;
    }
    if (flag)
    {
        for (int i=0;i<surfaces.count();i++)
            sur_matrix[(*(int*)(dGeomGetData(surfaces[i]->id1)))][*((int*)(dGeomGetData(surfaces[i]->id2)))] =
            sur_matrix[(*(int*)(dGeomGetData(surfaces[i]->id2)))][*((int*)(dGeomGetData(surfaces[i]->id1)))] = i;
    }
}

PSurface* PWorld::createSurface(PObject* o1,PObject* o2)
{
    PSurface *s = new PSurface();
    s->id1 = o1->geom;
    s->id2 = o2->geom;
    surfaces.append(s);
    sur_matrix[o1->id][o2->id] =
    sur_matrix[o2->id][o1->id] = surfaces.count() - 1;
    return s;
}

PSurface* PWorld::findSurface(PObject* o1,PObject* o2)
{
    for (int i=0;i<surfaces.count();i++)
    {
        if (surfaces[i]->isIt(o1->geom,o2->geom)) return (surfaces[i]);
    }
    return NULL;
}

void PWorld::step(dReal dt)
{
    try {
        dSpaceCollide (space,this,&nearCallback);
        dWorldStep(world,(dt<0) ? delta_time : dt);
        dJointGroupEmpty (contactgroup);
    }
    catch (...) {
        //qDebug() << "Some Error Happened;";
    }
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

