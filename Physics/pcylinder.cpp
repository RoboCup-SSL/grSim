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

#include "pcylinder.h"

PCylinder::PCylinder(dReal x,dReal y,dReal z,dReal radius,dReal length,dReal mass,dReal red,dReal green,dReal blue,int texid,bool robot)
          : PObject(x,y,z,red,green,blue,mass)
{
    m_radius = radius;
    m_length = length;    
    m_texid = texid;
    m_robot = robot;
}

PCylinder::~PCylinder()
{
}


void PCylinder::setMass(dReal mass)
{
  m_mass = mass;
  dMass m;
  dMassSetCylinderTotal (&m,m_mass,1,m_radius,m_length);
  dBodySetMass (body,&m);
}

void PCylinder::init()
{
  body = dBodyCreate (world);
  initPosBody();
  setMass(m_mass);
/*  if (m_texid!=-1)
  {
      dTriMeshDataID g = dGeomTriMeshDataCreate();
      const int vertexcount = 19*3+1;
      const int indexcount = 19*9;
      dVector3* vertices = new dVector3[vertexcount];
      dTriIndex* indices = new dTriIndex[indexcount];

      const int n = 24;	// number of sides to the cylinder (divisible by 4)

      dReal l = m_length*0.5;
      dReal a = dReal(M_PI*2.0)/dReal(n);
      dReal sa = (dReal) sin(a);
      dReal ca = (dReal) cos(a);
      dReal r = m_radius;

      dReal ny=1,nz=0,tmp;
      int v=0,f=0;      
      int i;
      for (i=0; i<=n; i++) {
          if (i>2 && i<n-2)
          {
              vertices[v][0] = ny*r;
              vertices[v][1] = nz*r;
              vertices[v][2] = l;v++;
              vertices[v][0] = ny*r;
              vertices[v][1] = nz*r;
              vertices[v][2] = -l;v++;
              indices[f] = v;f++;
              indices[f] = v-2;f++;
              indices[f] = v-1;f++;              
              indices[f] = v-1;f++;
              indices[f] = v;f++;
              indices[f] = v+1;f++;
          }
        }
      //v=38;f=38*3
        // rotate ny,nz
        tmp = ca*ny - sa*nz;
        nz = sa*ny + ca*nz;
        ny = tmp;      

      ny=1; nz=0;		  // normal vector = (0,ny,nz)
      glBegin (GL_TRIANGLE_FAN);
      vertices[v][0] = 0;
      vertices[v][1] = 0;
      vertices[v][2] = l;v++;
      for (i=0; i<n; i++) {
          if (i>2 && i<n-2)
          {
            vertices[v][0] = ny*r;
            vertices[v][1] = nz*r;
            vertices[v][2] = l;v++;
            indices[f] = 38;f++;
            indices[f] = v;f++;
            indices[f] = v-1;f++;
          }
        // rotate ny,nz
        tmp = ca*ny - sa*nz;
        nz = sa*ny + ca*nz;
        ny = tmp;
      }
      // draw bottom cap
//      ny=1; nz=0;		  // normal vector = (0,ny,nz)
//      glBegin (GL_TRIANGLE_FAN);
//      glNormal3d (0,0,-1);
//      glVertex3d (0,0,-l+zoffset);
//      for (i=0; i<=n; i++) {
//          if (i>=2 && i<n-2)
//          {
//
//        glNormal3d (0,0,-1);
//        glVertex3d (ny*r,nz*r,-l+zoffset);
//    }
//        // rotate ny,nz
//        tmp = ca*ny + sa*nz;
//        nz = -sa*ny + ca*nz;
//        ny = tmp;
//      }
      dGeomTriMeshDataBuildSimple(g, (dReal*) vertices, vertexcount, indices, indexcount);
      geom = dCreateTriMesh(0,g,NULL,NULL,NULL);
  }  
  else */
  {
    geom = dCreateCylinder(0,m_radius,m_length);
  }
  dGeomSetBody (geom,body);
  dSpaceAdd (space,geom);
}

void PCylinder::draw()
{
    PObject::draw();    
    if (m_texid==-1)
        g->drawCylinder(dBodyGetPosition(body),dBodyGetRotation(body),m_length,m_radius);
    else
        g->drawCylinder_TopTextured(dBodyGetPosition(body),dBodyGetRotation(body),m_length,m_radius,m_texid,m_robot);

/*    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();

        g->setTransform(dBodyGetPosition(body),dBodyGetRotation(body));
           glScaled(m_radius, m_radius, m_radius);
           glColor3d(0.1, 0.1, 0.1);
           glRasterPos3f(-0.25, 1.5, 0.0);
           //glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
           glutBitmapCharacter(GLUT_BITMAP_8_BY_13,c);
    glPopMatrix();*/
}
