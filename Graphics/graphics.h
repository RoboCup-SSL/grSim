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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QVector>
#include <QGLWidget>
#include <QString>

class CGraphics
{
private:
    float view_xyz[3],view_hpr[3];
    QVector<GLuint> tex_ids;
    float frustum_right,frustum_bottom,frustum_vnear,m_renderDepth;
    int _width,_height;
    QGLWidget* owner;
    int sphere_quality;
    void _drawBox (const float sides[3]);
    void _drawPatch (float p1[3], float p2[3], float p3[3], int level);
    void _drawSphere();
    void _drawCapsule (float l, float r);
    void _drawCylinder (float l, float r, float zoffset);
    void _drawCylinder_TopTextured (float l, float r, float zoffset,int tex_id,bool robot=false);
    void wrapCameraAngles();
    void setCamera (float x, float y, float z, float h, float p, float r);
    bool graphicDisabled;
public:
    CGraphics(QGLWidget* _owner);
    ~CGraphics();
    void disableGraphics();
    void enableGraphics();
    bool isGraphicsEnabled();
    int loadTexture(QImage* img);
    int loadTextureSkyBox(QImage* img);
    void setViewpoint (float xyz[3], float hpr[3]);
    void getViewpoint (float* xyz, float* hpr);
    void getFrustum(float& right,float& bottom,float& vnear);
    int getWidth();
    int getHeight();
    float renderDepth();
    void setRenderDepth(float depth);
    void setViewpoint (float x,float y,float z,float h,float p,float r);
    void cameraMotion (int mode, int deltax, int deltay);
    void lookAt(float x,float y,float z);
    void getCameraForward(float& x,float& y,float& z);
    void getCameraRight(float& x,float& y,float& z);
    void zoomCamera(float dz);
    void setColor (float r, float g, float b, float alpha);
    void setSphereQuality(int q);
    void setShadow(bool state);
    void useTexture(int tex_id);
    void noTexture();
    void setTransform (const float pos[3], const float R[12]);
    void setTransformD (const double pos[3], const double R[12]);

    void initScene(int width,int height,float rc,float gc,float bc,bool fog=false,float fogr=0.6,float fogg=0.6,float fogb=0.6,float fogdensity=0.6);
    void finalizeScene();
    void resetState();
    void drawSkybox(int t1,int t2,int t3,int t4,int t5,int t6);
    void drawSky ();
    void drawGround();
    void drawSSLGround(float SSL_FIELD_RAD,float SSL_FIELD_LENGTH,float SSL_FIELD_WIDTH,float SSL_FIELD_PENALTY,float SSL_FIELD_LINE_LENGTH,float SSL_FIELD_PENALTY_POINT,float epsilon);

    void drawBox (const float pos[3], const float R[12],const float sides[3]);
    void drawSphere (const float pos[3], const float R[12],float radius);
    void drawCylinder (const float pos[3], const float R[12],float length, float radius);    
    void drawCylinder_TopTextured (const float pos[3], const float R[12],float length, float radius,int tex_id,bool robot=false);
    void drawCapsule (const float pos[3], const float R[12],float length, float radius);
    void drawLine (const float pos1[3], const float pos2[3]);
    void drawCircle(float x0,float y0,float z0,float r);

};

#endif // GRAPHICS_H



