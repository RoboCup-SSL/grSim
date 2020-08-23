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

#define GL_SILENCE_DEPRECATION
#include <ode/ode.h>

#include <QVector>
#include <QGLWidget>
#include <QString>

enum CameraMotionMode {
    ROTATE_VIEW_POINT = 1,
    MOVE_POSITION_FREELY = 2,
    MOVE_POSITION_LR = 4,
};

class CGraphics
{
private:
    dReal view_xyz[3],view_hpr[3];
    QVector<GLuint> tex_ids;
    dReal frustum_right,frustum_bottom,frustum_vnear,m_renderDepth;
    int _width,_height;
    QGLWidget* owner;
    int sphere_quality;
    void _drawBox (const dReal sides[3]);
    void _drawPatch (dReal p1[3], dReal p2[3], dReal p3[3], int level);
    void _drawSphere();
    void _drawCapsule (dReal l, dReal r);
    void _drawCylinder (dReal l, dReal r, dReal zoffset);
    void _drawCylinder_TopTextured (dReal l, dReal r, dReal zoffset,int tex_id,bool robot=false);
    void wrapCameraAngles();
    void setCamera (dReal x, dReal y, dReal z, dReal h, dReal p, dReal r);
    bool graphicDisabled;
public:
    CGraphics(QGLWidget* _owner);
    ~CGraphics();
    void disableGraphics();
    void enableGraphics();
    bool isGraphicsEnabled();
    int loadTexture(QImage* img);
    int loadTextureSkyBox(QImage* img);
    void setViewpoint (dReal xyz[3], dReal hpr[3]);
    void getViewpoint (dReal* xyz, dReal* hpr);
    void getFrustum(dReal& right,dReal& bottom,dReal& vnear);
    int getWidth();
    int getHeight();
    dReal renderDepth();
    void setRenderDepth(dReal depth);
    void setViewpoint (dReal x,dReal y,dReal z,dReal h,dReal p,dReal r);
    void cameraMotion (CameraMotionMode mode, int deltax, int deltay);
    void lookAt(dReal x,dReal y,dReal z);
    void getCameraForward(dReal& x,dReal& y,dReal& z);
    void getCameraRight(dReal& x,dReal& y,dReal& z);
    void zoomCamera(dReal dz);
    void setColor (dReal r, dReal g, dReal b, dReal alpha);
    void setSphereQuality(int q);
    void setShadow(bool state);
    void useTexture(int tex_id);
    void noTexture();
    void setTransform (const dReal pos[3], const dReal R[12]);
    void setTransformD (const double pos[3], const double R[12]);

    void initScene(int width,int height,dReal rc,dReal gc,dReal bc,bool fog=false,dReal fogr=0.6,dReal fogg=0.6,dReal fogb=0.6,dReal fogdensity=0.6);
    void finalizeScene();
    void resetState();
    void drawSkybox(int t1,int t2,int t3,int t4,int t5,int t6);
    void drawSky ();
    void drawGround();
    void drawSSLGround(dReal SSL_FIELD_RAD,dReal SSL_FIELD_LENGTH,dReal SSL_FIELD_WIDTH,dReal SSL_FIELD_PENALTY_DEPTH,dReal SSL_FIELD_PENALTY_WIDTH,dReal SSL_FIELD_PENALTY_POINT, dReal SSL_FIELD_LINE_WIDTH, dReal epsilon);

    void drawBox (const dReal pos[3], const dReal R[12],const dReal sides[3]);
    void drawSphere (const dReal pos[3], const dReal R[12],dReal radius);
    void drawCylinder (const dReal pos[3], const dReal R[12],dReal length, dReal radius);
    void drawCylinder_TopTextured (const dReal pos[3], const dReal R[12],dReal length, dReal radius,int tex_id,bool robot=false);
    void drawCapsule (const dReal pos[3], const dReal R[12],dReal length, dReal radius);
    void drawLine (const dReal pos1[3], const dReal pos2[3]);
    void drawCircle(dReal x0,dReal y0,dReal z0,dReal r);

};

#endif // GRAPHICS_H



