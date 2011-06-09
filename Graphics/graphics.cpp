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

#include "graphics.h"
#include <QImage>
#include <math.h>


// light vector. LIGHTZ is implicitly 1
#define LIGHTX (1.0f)
#define LIGHTY (0.4f)

// ground and sky
#define GROUND_R (0.5f) 	// ground color for when there's no texture
#define GROUND_G (0.5f)
#define GROUND_B (0.3f)



const float ground_scale = 1.0f/1.0f;	// ground texture scale (1/size)
const float ground_ofsx = 0.5;		// offset of ground texture
const float ground_ofsy = 0.5;
const float sky_scale = 1.0f/4.0f;	// sky texture scale (1/size)
const float sky_height = 1.0f;		// sky height above viewpoint


CGraphics::CGraphics(QGLWidget* _owner)
{
  owner = _owner;
  float xyz[3] = {0.8317f,-0.9817f,0.8000f};
  float hpr[3] = {121.0000f,-27.5000f,0.0000f};
  setViewpoint (xyz,hpr);
  sphere_quality = 1;
  m_renderDepth = 100;
  graphicDisabled = false;
}

CGraphics::~CGraphics()
{

}

void CGraphics::disableGraphics()
{
    graphicDisabled = true;
}

void CGraphics::enableGraphics()
{
    graphicDisabled = false;
}

bool CGraphics::isGraphicsEnabled()
{
    return !graphicDisabled;
}

void CGraphics::setSphereQuality(int q) {sphere_quality = q;}
int CGraphics::loadTexture(QImage* img)
{
    if (graphicDisabled) return -1;
    GLuint id;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &id);
    id = owner->bindTexture(*img);

    tex_ids.append(id);
    return tex_ids.size()-1;
}

int CGraphics::loadTextureSkyBox(QImage* img)
{
    if (graphicDisabled) return -1;
    GLuint id;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D,id);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    char* buffer=new char[img->width()*img->height()*4];
    int ofs = img->width()*4;
    for (int j=0;j<img->height();j++)
    {
        memcpy(buffer+j*ofs,img->scanLine(img->height()-1-j),ofs);
    }
    for (int j=0;j<img->height();j++)
    {
        for (int i=0;i<img->width();i++)
        {
            int p=j*ofs+i*4;
            char temp = buffer[p+2];
            buffer[p+2]=buffer[p];
            buffer[p]=temp;
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width(), img->height(), 0, GL_RGBA,
               GL_UNSIGNED_BYTE, buffer);
    delete img;
    tex_ids.append(id);
    return tex_ids.size()-1;
}


void CGraphics::getViewpoint (float* xyz, float* hpr)
{
    if (graphicDisabled) return;
    xyz[0] = view_xyz[0];
    xyz[1] = view_xyz[1];
    xyz[2] = view_xyz[2];
    hpr[0] = view_hpr[0];
    hpr[1] = view_hpr[1];
    hpr[2] = view_hpr[2];
}

void CGraphics::getFrustum(float& right,float& bottom,float& vnear)
{
    right = frustum_right;
    bottom= frustum_bottom;
    vnear = frustum_vnear;
}

void CGraphics::setRenderDepth(float depth)
{
    m_renderDepth = depth;
}

float CGraphics::renderDepth()
{
    return m_renderDepth;
}

void CGraphics::setViewpoint (float xyz[3], float hpr[3])
{
  if (xyz) {
    view_xyz[0] = xyz[0];
    view_xyz[1] = xyz[1];
    view_xyz[2] = xyz[2];
  }
  if (hpr) {
    view_hpr[0] = hpr[0];
    view_hpr[1] = hpr[1];
    view_hpr[2] = hpr[2];
    wrapCameraAngles();
  }
}

void CGraphics::setViewpoint (float x,float y,float z,float h,float p,float r)
{
    view_xyz[0] = x;
    view_xyz[1] = y;
    view_xyz[2] = z;
    view_hpr[0] = h;
    view_hpr[1] = p;
    view_hpr[2] = r;
    wrapCameraAngles();
}

void CGraphics::wrapCameraAngles()
{
    for (int i=0; i<3; i++) {
        while (view_hpr[i] > 180) view_hpr[i] -= 360;
        while (view_hpr[i] < -180) view_hpr[i] += 360;
    }
}

void CGraphics::cameraMotion (int mode, int deltax, int deltay)
{
    if (graphicDisabled) return;
  float side = 0.01f * float(deltax);
  float fwd = (mode==4) ? (0.01f * float(deltay)) : 0.0f;
  float s = (float) sin (view_hpr[0]*M_PI/180.0f);
  float c = (float) cos (view_hpr[0]*M_PI/180.0f);

  if (mode==1) {
    view_hpr[0] += float (deltax) * 0.5f;
    view_hpr[1] += float (deltay) * 0.5f;
  }
  else {
    view_xyz[0] += -s*side + c*fwd;
    view_xyz[1] += c*side + s*fwd;
    if (mode==2 || mode==5) view_xyz[2] += 0.01f * float(deltay);
  }
  wrapCameraAngles();
}

void rotxy(float &x,float &y,float a)
{
    float ca = cos(a);
    float sa = sin(a);
    float xx = x*ca - y*sa;
    float yy = x*sa + y*ca;
    x = xx;
    y = yy;
}

void CGraphics::getCameraForward(float& x,float& y,float& z)
{
  float h = view_hpr[0]*M_PI/180.0f;
  float p = view_hpr[1]*M_PI/180.0f;
  float r = view_hpr[2]*M_PI/180.0f;
  x=-1; y=0; z=0;
  rotxy(y,z,r);
  rotxy(z,x,-p);
  rotxy(x,y,h);
}

void CGraphics::lookAt(float x,float y,float z)
{
    float rx = x - view_xyz[0];
    float ry = y - view_xyz[1];
    float rz = z - view_xyz[2];
    float rr = sqrt(rx*rx + ry*ry + rz*rz);
    if (rr==0) return;
    float r = 0;
    float p = asin(rz/rr)*180.0/M_PI;
    float h = atan2(ry,rx)*180.0/M_PI;
    view_hpr[0] = h;
    view_hpr[1] = p;
    view_hpr[2] = r;
}

void CGraphics::getCameraRight(float& x,float& y,float& z)
{
  float h = view_hpr[0]*M_PI/180.0f;
  float p = view_hpr[1]*M_PI/180.0f;
  float r = view_hpr[2]*M_PI/180.0f;
  x=0; y=-1; z=0;
  rotxy(y,z,r);
  rotxy(z,x,-p);
  rotxy(x,y,h);
}

void CGraphics::zoomCamera(float dz)
{
    float xx,yy,zz;
    getCameraForward(xx,yy,zz);
    view_xyz[0] += xx*dz;
    view_xyz[1] += yy*dz;
    view_xyz[2] += zz*dz;
}

void CGraphics::setCamera (float x, float y, float z, float h, float p, float r)
{
    if (graphicDisabled) return;
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
  glRotatef (90, 0,0,1);
  glRotatef (90, 0,1,0);
  glRotatef (r, 1,0,0);
  glRotatef (p, 0,1,0);
  glRotatef (-h, 0,0,1);
  glTranslatef (-x,-y,-z);
}


// sets the material color, not the light color
void CGraphics::setColor (float r, float g, float b, float alpha)
{
    if (graphicDisabled) return;
  GLfloat light_ambient[4],light_diffuse[4],light_specular[4];
  light_ambient[0] = r*0.3f;
  light_ambient[1] = g*0.3f;
  light_ambient[2] = b*0.3f;
  light_ambient[3] = alpha;
  light_diffuse[0] = r*0.7f;
  light_diffuse[1] = g*0.7f;
  light_diffuse[2] = b*0.7f;
  light_diffuse[3] = alpha;
  light_specular[0] = r*0.2f;
  light_specular[1] = g*0.2f;
  light_specular[2] = b*0.2f;
  light_specular[3] = alpha;
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, light_ambient);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, light_diffuse);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, light_specular);
  glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 5.0f);
}
void CGraphics::drawSkybox(int t1,int t2,int t3,int t4,int t5,int t6)
{
    if (graphicDisabled) return;
    // Store the current matrix
    glPushMatrix();

//    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    glRotatef (90, 0,0,1);
    glRotatef (90, 0,1,0);
    glRotatef (view_hpr[2], 1,0,0);
    glRotatef (view_hpr[1], 0,1,0);
    glRotatef (-view_hpr[0], 0,0,1);    
    glScalef (m_renderDepth,m_renderDepth,m_renderDepth);
    // Enable/Disable features

    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    //glDisable(GL_DEPTH_TEST);
    glShadeModel (GL_FLAT);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    // Just in case we set all vertices to white.
    glColor4f(1,1,1,1);
    const float r = 1.005f;  //to overcome borders problem
    // neg_x
    glBindTexture(GL_TEXTURE_2D, tex_ids[t1]);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f*r, -0.5f*r );
        glTexCoord2f(1, 0); glVertex3f( -0.5f,  0.5f*r, -0.5f*r );
        glTexCoord2f(1, 1); glVertex3f( -0.5f,  0.5f*r,  0.5f*r );
        glTexCoord2f(0, 1); glVertex3f( -0.5f, -0.5f*r,  0.5f*r );
    glEnd();
    // pos_y
    glBindTexture(GL_TEXTURE_2D, tex_ids[t4]);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -0.5f*r,  0.5f, -0.5f*r );
        glTexCoord2f(1, 0); glVertex3f(  0.5f*r,  0.5f, -0.5f*r );
        glTexCoord2f(1, 1); glVertex3f(  0.5f*r,  0.5f,  0.5f*r );
        glTexCoord2f(0, 1); glVertex3f( -0.5f*r,  0.5f,  0.5f*r );
    glEnd();
    //pos_x
    glBindTexture(GL_TEXTURE_2D, tex_ids[t2]);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(  0.5f,  0.5f*r, -0.5f*r );
        glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f*r, -0.5f*r );
        glTexCoord2f(1, 1); glVertex3f(  0.5f, -0.5f*r,  0.5f*r );
        glTexCoord2f(0, 1); glVertex3f(  0.5f,  0.5f*r,  0.5f*r );
    glEnd();
    // neg_y
    glBindTexture(GL_TEXTURE_2D, tex_ids[t3]);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(  0.5f*r, -0.5f, -0.5f*r );
        glTexCoord2f(1, 0); glVertex3f( -0.5f*r, -0.5f, -0.5f*r );
        glTexCoord2f(1, 1); glVertex3f( -0.5f*r, -0.5f,  0.5f*r );
        glTexCoord2f(0, 1); glVertex3f(  0.5f*r, -0.5f,  0.5f*r );
    glEnd();
    // neg_z
    glBindTexture(GL_TEXTURE_2D, tex_ids[t5]);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -0.5f*r, -0.5f*r, -0.5f );
        glTexCoord2f(1, 0); glVertex3f( -0.5f*r,  0.5f*r, -0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f*r,  0.5f*r, -0.5f );
        glTexCoord2f(0, 1); glVertex3f(  0.5f*r, -0.5f*r, -0.5f );
    glEnd();
    // pos_z
    glBindTexture(GL_TEXTURE_2D, tex_ids[t6]);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -0.5f*r, -0.5f*r,  0.5f );
        glTexCoord2f(1, 0); glVertex3f( -0.5f*r,  0.5f*r,  0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f*r,  0.5f*r,  0.5f );
        glTexCoord2f(0, 1); glVertex3f(  0.5f*r, -0.5f*r,  0.5f );
    glEnd();

    // Restore enable bits and matrix
    glPopAttrib();
    glPopMatrix();
}

void CGraphics::useTexture(int tex_id)
{
    if (graphicDisabled) return;
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, tex_ids[tex_id]);

    glEnable (GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_ids[tex_id]);
}

void CGraphics::noTexture()
{
    if (graphicDisabled) return;
    glDisable(GL_TEXTURE_2D);
}

void CGraphics::setTransform (const float pos[3], const float R[12])
{
    if (graphicDisabled) return;
  GLfloat matrix[16];
  matrix[0]=R[0];
  matrix[1]=R[4];
  matrix[2]=R[8];
  matrix[3]=0;
  matrix[4]=R[1];
  matrix[5]=R[5];
  matrix[6]=R[9];
  matrix[7]=0;
  matrix[8]=R[2];
  matrix[9]=R[6];
  matrix[10]=R[10];
  matrix[11]=0;
  matrix[12]=pos[0];
  matrix[13]=pos[1];
  matrix[14]=pos[2];
  matrix[15]=1;
  glPushMatrix();
  glMultMatrixf (matrix);
}

void CGraphics::setTransformD (const double pos[3], const double R[12])
{
    if (graphicDisabled) return;
  GLdouble matrix[16];
  matrix[0]=R[0];
  matrix[1]=R[4];
  matrix[2]=R[8];
  matrix[3]=0;
  matrix[4]=R[1];
  matrix[5]=R[5];
  matrix[6]=R[9];
  matrix[7]=0;
  matrix[8]=R[2];
  matrix[9]=R[6];
  matrix[10]=R[10];
  matrix[11]=0;
  matrix[12]=pos[0];
  matrix[13]=pos[1];
  matrix[14]=pos[2];
  matrix[15]=1;
  glPushMatrix();
  glMultMatrixd (matrix);
}

void CGraphics::initScene(int width,int height,float rc,float gc,float bc,bool fog,float fogr,float fogg,float fogb,float fogdensity)
{
    if (graphicDisabled) return;
  _width = width;
  _height = height;
  // setup stuff
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_TEXTURE_GEN_S);
  glDisable (GL_TEXTURE_GEN_T);
  glShadeModel (GL_FLAT);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LESS);
  glEnable (GL_CULL_FACE);
  glCullFace (GL_BACK);
  glFrontFace (GL_CCW);


  // setup viewport
  glViewport (0,0,width,height);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  const float vnear = 0.1f;
  const float vfar = m_renderDepth;
  const float k = 0.8f;     // view scale, 1 = +/- 45 degrees
  frustum_vnear = vnear;
  if (width >= height) {
    float k2 = float(height)/float(width);
    frustum_right  = vnear*k;
    frustum_bottom = vnear*k*k2;
  }
  else {
    float k2 = float(width)/float(height);
    frustum_right  = vnear*k*k2;
    frustum_bottom = vnear*k;
  }
  glFrustum (-frustum_right,frustum_right,-frustum_bottom,frustum_bottom,vnear,vfar);

  // setup lights. it makes a difference whether this is done in the
  // GL_PROJECTION matrix mode (lights are scene relative) or the
  // GL_MODELVIEW matrix mode (lights are camera relative, bad!).
  static GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
  static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
  glColor3f (1.0, 1.0, 1.0);

  // clear the window
  glClearColor (rc,gc,bc,1);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // snapshot camera position (in MS Windows it is changed by the GUI thread)
  float view2_xyz[3];
  float view2_hpr[3];
  memcpy (view2_xyz,view_xyz,sizeof(float)*3);
  memcpy (view2_hpr,view_hpr,sizeof(float)*3);

  // go to GL_MODELVIEW matrix mode and set the camera
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
  setCamera (view2_xyz[0],view2_xyz[1],view2_xyz[2],
             view2_hpr[0],view2_hpr[1],view2_hpr[2]);

  // set the light position (for some reason we have to do this in model view.
  static GLfloat light_position[] = { LIGHTX, LIGHTY, 1.0, 0.0 };
  glLightfv (GL_LIGHT0, GL_POSITION, light_position);


    if (fog)
    {
        GLfloat fogColor [4] = {fogr,fogg,fogb,1};
        glFogi (GL_FOG_MODE, GL_EXP2); //set the fog mode to GL_EXP2
        glFogf(GL_FOG_START,5);
        glFogf(GL_FOG_END,10);
        glFogfv (GL_FOG_COLOR, fogColor); //set the fog color to our color chosen above
        glFogf (GL_FOG_DENSITY, fogdensity); //set the density to the value above
        //glHint (GL_FOG_HINT, GL_NICEST); // set the fog to look the nicest, may slow down on older cards
    }

}

int CGraphics::getWidth()
{
    return _width;
}

int CGraphics::getHeight()
{
    return _height;
}

void CGraphics::finalizeScene()
{
 //   owner->swapBuffers();
}

void CGraphics::drawSky ()
{
    if (graphicDisabled) return;
/*  glDisable (GL_LIGHTING);

  // make sure sky depth is as far back as possible
  glShadeModel (GL_FLAT);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);
  glDepthRange (1,1);
*/
  const float ssize = 1000.0f;
  float offset = 0.0f;

  float x = ssize*sky_scale;
  float z = view_xyz[2] + sky_height;

  glBegin (GL_QUADS);
  glNormal3f (0,0,-1);
  glTexCoord2f (-x+offset,-x+offset);
  glVertex3f (-ssize+view_xyz[0],-ssize+view_xyz[1],z);
  glTexCoord2f (-x+offset,x+offset);
  glVertex3f (-ssize+view_xyz[0],ssize+view_xyz[1],z);
  glTexCoord2f (x+offset,x+offset);
  glVertex3f (ssize+view_xyz[0],ssize+view_xyz[1],z);
  glTexCoord2f (x+offset,-x+offset);
  glVertex3f (ssize+view_xyz[0],-ssize+view_xyz[1],z);
  glEnd();

  if (offset > 1) offset -= 1;

  glDepthFunc (GL_LESS);
  glDepthRange (0,1);


  resetState();
}

void CGraphics::resetState()
{
    if (graphicDisabled) return;
  glEnable (GL_LIGHTING);
  glDisable (GL_TEXTURE_2D);
  glShadeModel (GL_FLAT);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LESS);
  glColor3f (1,1,1);
  setColor (1,1,1,1);
}

void CGraphics::drawGround()
{
    if (graphicDisabled) return;
  glDisable (GL_LIGHTING);
  glShadeModel (GL_FLAT);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LESS);

  const float gsize = 100.0f;
  const float offset = 0;

  glBegin (GL_QUADS);
  glNormal3f (0,0,1);
  glTexCoord2f (-gsize*ground_scale + ground_ofsx,
                -gsize*ground_scale + ground_ofsy);
  glVertex3f (-gsize,-gsize,offset);
  glTexCoord2f (gsize*ground_scale + ground_ofsx,
                -gsize*ground_scale + ground_ofsy);
  glVertex3f (gsize,-gsize,offset);
  glTexCoord2f (gsize*ground_scale + ground_ofsx,
                gsize*ground_scale + ground_ofsy);
  glVertex3f (gsize,gsize,offset);
  glTexCoord2f (-gsize*ground_scale + ground_ofsx,
                gsize*ground_scale + ground_ofsy);
  glVertex3f (-gsize,gsize,offset);
  glEnd();

  resetState();
}

void CGraphics::drawSSLGround(float SSL_FIELD_RAD,float SSL_FIELD_LENGTH,float SSL_FIELD_WIDTH,float SSL_FIELD_PENALTY,float SSL_FIELD_LINE_LENGTH,float SSL_FIELD_PENALTY_POINT,float epsilon)
{
    if (graphicDisabled) return;
    float angle,x,y,z;
    float radx = (SSL_FIELD_PENALTY) / ( SSL_FIELD_LENGTH / 2.0);
    float radz = (SSL_FIELD_PENALTY) / ( SSL_FIELD_WIDTH / 2.0);
    float penaltyx = SSL_FIELD_PENALTY_POINT / (SSL_FIELD_LENGTH / 2.0);
    glPushMatrix();
            glScaled(SSL_FIELD_LENGTH / 2.0 ,  SSL_FIELD_WIDTH / 2.0,1);

            GLfloat sizes[2];  // Store supported line width range
            GLfloat step;     // Store supported line width increments
            // Get supported line width range and step size
            glGetFloatv(GL_LINE_WIDTH_RANGE,sizes);
            glGetFloatv(GL_LINE_WIDTH_GRANULARITY,&step);
            float fCurrSize = sizes[0];

            glLineWidth(fCurrSize * 5);
            glBegin(GL_LINE_LOOP);
                    glColor3f(1.0f,1.0f,1.0f);
                    //glNormal3f(0, 1, 0);
                    glVertex3f(1,  1, epsilon);
                    glVertex3f(1,  -1 , epsilon);
                    glVertex3f(-1, -1 , epsilon);
                    glVertex3f(-1, 1 , epsilon);
            glEnd();

            glBegin(GL_LINES);
                    glColor3f(1.0f,1.0f,1.0f);
                    glVertex3f(0.0, 1.0 , epsilon);
                    glVertex3f(0.0, -1.0, epsilon );
            glEnd();

            glBegin(GL_LINE_LOOP);
                    z = epsilon;
                    for(angle = 0.0f; angle <= 2.0 * M_PI; angle += (M_PI/20.0f))
                      {
                              x = radx * sin(angle);
                              y = radz * cos(angle);
                              glVertex3f(x, y, z);
                      }
            glEnd();

            float h = SSL_FIELD_LINE_LENGTH / SSL_FIELD_WIDTH;

            glBegin(GL_LINE_LOOP);
                    z = epsilon;
                    for(angle = 0.0f; angle <=  M_PI*0.5; angle += (M_PI/20.0f))
                      {
                              x = -1.0 + (radx * sin(angle));
                              y = radz * cos(angle) + h*0.5f;
                              glVertex3f(x, y, z);
                      }
                    for(angle = M_PI*0.5; angle >=  0.0f; angle -= (M_PI/20.0f))
                      {
                              x = -1.0 + (radx * sin(angle));
                              y = -radz * cos(angle) - h*0.5f;
                              glVertex3f(x, y, z);
                      }
                    glVertex3f(-1.0f,-h-radz,z);
            glEnd();

            glBegin(GL_LINE_LOOP);
                    z = epsilon;
                    for(angle = 0.0f; angle <=  M_PI*0.5; angle += (M_PI/20.0f))
                      {
                              x = 1.0 - (radx * sin(angle));
                              y = radz * cos(angle) + h*0.5f;
                              glVertex3f(x, y, z);
                      }
                    for(angle = M_PI*0.5; angle >= 0.0 ; angle -= (M_PI/20.0f))
                      {
                              x = 1.0 - (radx * sin(angle));
                              y = -radz * cos(angle) - h*0.5f;
                              glVertex3f(x, y, z);
                      }
                    glVertex3f(1.0f,-h-radz,z);
            glEnd();

            drawCircle(-1+penaltyx,0,epsilon,0.005);
            drawCircle(+1-penaltyx,0,epsilon,0.005);
            glLineWidth(fCurrSize);
    glPopMatrix();

}

void CGraphics::_drawBox (const float sides[3])
{
    if (graphicDisabled) return;
  float lx = sides[0]*0.5f;
  float ly = sides[1]*0.5f;
  float lz = sides[2]*0.5f;

  // sides
  glBegin (GL_TRIANGLE_STRIP);
  glNormal3f (-1,0,0);
  glVertex3f (-lx,-ly,-lz);
  glVertex3f (-lx,-ly,lz);
  glVertex3f (-lx,ly,-lz);
  glVertex3f (-lx,ly,lz);
  glNormal3f (0,1,0);
  glVertex3f (lx,ly,-lz);
  glVertex3f (lx,ly,lz);
  glNormal3f (1,0,0);
  glVertex3f (lx,-ly,-lz);
  glVertex3f (lx,-ly,lz);
  glNormal3f (0,-1,0);
  glVertex3f (-lx,-ly,-lz);
  glVertex3f (-lx,-ly,lz);
  glEnd();

  // top face
  glBegin (GL_TRIANGLE_FAN);
  glNormal3f (0,0,1);
  glVertex3f (-lx,-ly,lz);
  glVertex3f (lx,-ly,lz);
  glVertex3f (lx,ly,lz);
  glVertex3f (-lx,ly,lz);
  glEnd();

  // bottom face
  glBegin (GL_TRIANGLE_FAN);
  glNormal3f (0,0,-1);
  glVertex3f (-lx,-ly,-lz);
  glVertex3f (-lx,ly,-lz);
  glVertex3f (lx,ly,-lz);
  glVertex3f (lx,-ly,-lz);
  glEnd();
}


// This is recursively subdivides a triangular area (vertices p1,p2,p3) into
// smaller triangles, and then draws the triangles. All triangle vertices are
// normalized to a distance of 1.0 from the origin (p1,p2,p3 are assumed
// to be already normalized). Note this is not super-fast because it draws
// triangles rather than triangle strips.
void CGraphics::_drawPatch (float p1[3], float p2[3], float p3[3], int level)
{
  int i;
  if (level > 0) {
    float q1[3],q2[3],q3[3];		 // sub-vertices
    for (i=0; i<3; i++) {
      q1[i] = 0.5f*(p1[i]+p2[i]);
      q2[i] = 0.5f*(p2[i]+p3[i]);
      q3[i] = 0.5f*(p3[i]+p1[i]);
    }
    float length1 = (float)(1.0/sqrt(q1[0]*q1[0]+q1[1]*q1[1]+q1[2]*q1[2]));
    float length2 = (float)(1.0/sqrt(q2[0]*q2[0]+q2[1]*q2[1]+q2[2]*q2[2]));
    float length3 = (float)(1.0/sqrt(q3[0]*q3[0]+q3[1]*q3[1]+q3[2]*q3[2]));
    for (i=0; i<3; i++) {
      q1[i] *= length1;
      q2[i] *= length2;
      q3[i] *= length3;
    }
    _drawPatch (p1,q1,q3,level-1);
    _drawPatch (q1,p2,q2,level-1);
    _drawPatch (q1,q2,q3,level-1);
    _drawPatch (q3,q2,p3,level-1);
  }
  else {
    glNormal3f (p1[0],p1[1],p1[2]);
    glVertex3f (p1[0],p1[1],p1[2]);
    glNormal3f (p2[0],p2[1],p2[2]);
    glVertex3f (p2[0],p2[1],p2[2]);
    glNormal3f (p3[0],p3[1],p3[2]);
    glVertex3f (p3[0],p3[1],p3[2]);
  }
}


// draw a sphere of radius 1

void CGraphics::_drawSphere()
{
    if (graphicDisabled) return;
  // icosahedron data for an icosahedron of radius 1.0
# define ICX 0.525731112119133606f
# define ICZ 0.850650808352039932f
  static GLfloat idata[12][3] = {
    {-ICX, 0, ICZ},
    {ICX, 0, ICZ},
    {-ICX, 0, -ICZ},
    {ICX, 0, -ICZ},
    {0, ICZ, ICX},
    {0, ICZ, -ICX},
    {0, -ICZ, ICX},
    {0, -ICZ, -ICX},
    {ICZ, ICX, 0},
    {-ICZ, ICX, 0},
    {ICZ, -ICX, 0},
    {-ICZ, -ICX, 0}
  };

  static int index[20][3] = {
    {0, 4, 1},	  {0, 9, 4},
    {9, 5, 4},	  {4, 5, 8},
    {4, 8, 1},	  {8, 10, 1},
    {8, 3, 10},   {5, 3, 8},
    {5, 2, 3},	  {2, 7, 3},
    {7, 10, 3},   {7, 6, 10},
    {7, 11, 6},   {11, 0, 6},
    {0, 1, 6},	  {6, 1, 10},
    {9, 0, 11},   {9, 11, 2},
    {9, 2, 5},	  {7, 2, 11},
  };

  static GLuint listnum = 0;
  if (listnum==0) {
    listnum = glGenLists (1);
    glNewList (listnum,GL_COMPILE);
    glBegin (GL_TRIANGLES);
    for (int i=0; i<20; i++) {
      _drawPatch (&idata[index[i][2]][0],&idata[index[i][1]][0],
                 &idata[index[i][0]][0],sphere_quality);
    }
    glEnd();
    glEndList();
  }
  glCallList (listnum);
}


static int capped_cylinder_quality = 3;
void CGraphics::drawCircle(float x0,float y0,float z0,float r)
{
    if (graphicDisabled) return;
  int i;
  float tmp,ny,nz,a,ca,sa;
  const int n = 24;	// number of sides to the cylinder (divisible by 4)

  a = float(M_PI*2.0)/float(n);
  sa = (float) sin(a);
  ca = (float) cos(a);

    // draw top cap
  glShadeModel (GL_FLAT);
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);
  glNormal3d (0,0,1);
  for (i=0; i<=n; i++) {
    glNormal3d (0,0,1);
    glVertex3d (ny*r+x0,nz*r+y0,z0);
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();
}

// draw a capped cylinder of length l and radius r, aligned along the x axis

void CGraphics::_drawCapsule (float l, float r)
{
    if (graphicDisabled) return;
  int i,j;
  float tmp,nx,ny,nz,start_nx,start_ny,a,ca,sa;
  // number of sides to the cylinder (divisible by 4):
  const int n = capped_cylinder_quality*4;
;
  l *= 0.5;
  a = float(M_PI*2.0)/float(n);
  sa = (float) sin(a);
  ca = (float) cos(a);

  // draw cylinder body
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_STRIP);
  for (i=0; i<=n; i++) {
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,l);
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,-l);
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw first cylinder cap
  start_nx = 0;
  start_ny = 1;
  for (j=0; j<(n/4); j++) {
    // get start_n2 = rotated start_n
    float start_nx2 =  ca*start_nx + sa*start_ny;
    float start_ny2 = -sa*start_nx + ca*start_ny;
    // get n=start_n and n2=start_n2
    nx = start_nx; ny = start_ny; nz = 0;
    float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
    glBegin (GL_TRIANGLE_STRIP);
    for (i=0; i<=n; i++) {
      glNormal3d (ny2,nz2,nx2);
      glVertex3d (ny2*r,nz2*r,l+nx2*r);
      glNormal3d (ny,nz,nx);
      glVertex3d (ny*r,nz*r,l+nx*r);
      // rotate n,n2
      tmp = ca*ny - sa*nz;
      nz = sa*ny + ca*nz;
      ny = tmp;
      tmp = ca*ny2- sa*nz2;
      nz2 = sa*ny2 + ca*nz2;
      ny2 = tmp;
    }
    glEnd();
    start_nx = start_nx2;
    start_ny = start_ny2;
  }

  // draw second cylinder cap
  start_nx = 0;
  start_ny = 1;
  for (j=0; j<(n/4); j++) {
    // get start_n2 = rotated start_n
    float start_nx2 = ca*start_nx - sa*start_ny;
    float start_ny2 = sa*start_nx + ca*start_ny;
    // get n=start_n and n2=start_n2
    nx = start_nx; ny = start_ny; nz = 0;
    float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
    glBegin (GL_TRIANGLE_STRIP);
    for (i=0; i<=n; i++) {
      glNormal3d (ny,nz,nx);
      glVertex3d (ny*r,nz*r,-l+nx*r);
      glNormal3d (ny2,nz2,nx2);
      glVertex3d (ny2*r,nz2*r,-l+nx2*r);
      // rotate n,n2
      tmp = ca*ny - sa*nz;
      nz = sa*ny + ca*nz;
      ny = tmp;
      tmp = ca*ny2- sa*nz2;
      nz2 = sa*ny2 + ca*nz2;
      ny2 = tmp;
    }
    glEnd();
    start_nx = start_nx2;
    start_ny = start_ny2;
  }
}


// draw a cylinder of length l and radius r, aligned along the z axis

void CGraphics::_drawCylinder (float l, float r, float zoffset)
{
    if (graphicDisabled) return;
  int i;
  float tmp,ny,nz,a,ca,sa;
  const int n = 24;	// number of sides to the cylinder (divisible by 4)

  l *= 0.5;
  a = float(M_PI*2.0)/float(n);
  sa = (float) sin(a);
  ca = (float) cos(a);

  // draw cylinder body
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_STRIP);
  for (i=0; i<=n; i++) {
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,l+zoffset);
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,-l+zoffset);
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw top cap
  glShadeModel (GL_FLAT);
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);
  glNormal3d (0,0,1);
  glVertex3d (0,0,l+zoffset);
  for (i=0; i<=n; i++) {
    glNormal3d (0,0,1);
    glVertex3d (ny*r,nz*r,l+zoffset);
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw bottom cap
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);
  glNormal3d (0,0,-1);
  glVertex3d (0,0,-l+zoffset);
  for (i=0; i<=n; i++) {
    glNormal3d (0,0,-1);
    glVertex3d (ny*r,nz*r,-l+zoffset);
    // rotate ny,nz
    tmp = ca*ny + sa*nz;
    nz = -sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();
}

void CGraphics::_drawCylinder_TopTextured (float l, float r, float zoffset,int tex_id,bool robot)
{
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);
    if (graphicDisabled) return;
  int i;
  float tmp,ny,nz,a,ca,sa;
  const int n = 24;	// number of sides to the cylinder (divisible by 4)

  l *= 0.5;
  a = float(M_PI*2.0)/float(n);
  sa = (float) sin(a);
  ca = (float) cos(a);

  // draw cylinder body
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  float nny,nnz;
  glBegin (GL_TRIANGLE_STRIP);
  for (i=0; i<=n; i++) {
      if ((i>2 && i<n-2) || (!robot))
      {
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,l+zoffset);
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,-l+zoffset);    
    }
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }

  for (i=0;i<=1;i++)
  {
      tmp = ca*ny - sa*nz;
      nz = sa*ny + ca*nz;
      ny = tmp;
  }
  glNormal3d (ny,nz,0);
  glVertex3d (ny*r,nz*r,l+zoffset);
  glNormal3d (ny,nz,0);
  glVertex3d (ny*r,nz*r,-l+zoffset);
  glEnd();

  glDisable(GL_LIGHTING);
  useTexture(tex_id);

  // draw top cap
  glShadeModel (GL_FLAT);

  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);

  for (i=0; i<n; i++) {
      if ((i>2 && i<n-2)  || (!robot))
      {

    glNormal3d (0,0,1);
    glTexCoord2f(-0.5*nz+0.5,0.5*ny+0.5);
    glVertex3d (ny*r,nz*r,l+zoffset);
}
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();
  noTexture();
  glEnable(GL_LIGHTING);

  // draw bottom cap
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);
  glNormal3d (0,0,-1);
  glVertex3d (0,0,-l+zoffset);
  for (i=0; i<=n; i++) {
      if ((i>=2 && i<n-2)  || (!robot))
      {

    glNormal3d (0,0,-1);
    glVertex3d (ny*r,nz*r,-l+zoffset);
}
    // rotate ny,nz
    tmp = ca*ny + sa*nz;
    nz = -sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();
//    glDisable(GL_BLEND);
}


void CGraphics::drawBox (const float pos[3], const float R[12],
                           const float sides[3])
{
    if (graphicDisabled) return;
  glShadeModel (GL_FLAT);
  setTransform (pos,R);
  _drawBox (sides);
  glPopMatrix();

}


void CGraphics::drawSphere (const float pos[3], const float R[12],
                              float radius)
{
    if (graphicDisabled) return;
  glEnable (GL_NORMALIZE);
  glShadeModel (GL_SMOOTH);
  setTransform (pos,R);
  glScaled (radius,radius,radius);
  _drawSphere();
  glPopMatrix();
  glDisable (GL_NORMALIZE);


}


void CGraphics::drawCylinder (const float pos[3], const float R[12],
                                float length, float radius)
{
    if (graphicDisabled) return;
  glShadeModel (GL_SMOOTH);
  setTransform (pos,R);
  _drawCylinder (length,radius,0);
  glPopMatrix();
}

void CGraphics::drawCylinder_TopTextured (const float pos[3], const float R[12],
                                float length, float radius,int tex_id,bool robot)
{
    if (graphicDisabled) return;
  glShadeModel (GL_SMOOTH);
  setTransform (pos,R);
  _drawCylinder_TopTextured (length,radius,0,tex_id,robot);
  glPopMatrix();
}

void CGraphics::drawCapsule (const float pos[3], const float R[12],
                                      float length, float radius)
{
    if (graphicDisabled) return;
  glShadeModel (GL_SMOOTH);
  setTransform (pos,R);
  _drawCapsule (length,radius);
  glPopMatrix();

}


void CGraphics::drawLine (const float pos1[3], const float pos2[3])
{
    if (graphicDisabled) return;
  glDisable (GL_LIGHTING);
  glLineWidth (2);
  glShadeModel (GL_FLAT);
  glBegin (GL_LINES);
  glVertex3f (pos1[0],pos1[1],pos1[2]);
  glVertex3f (pos2[0],pos2[1],pos2[2]);
  glEnd();
}


