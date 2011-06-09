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

#ifndef ROBOT_H
#define ROBOT_H

#include "Physics/pworld.h"
#include "Physics/pcylinder.h"
#include "Physics/pbox.h"
#include "Physics/pball.h"
#include "configwidget.h"


class Robot
{
    PWorld* w;
    PBall* m_ball;
    float m_x,m_y,m_z;
    float m_r,m_g,m_b;
    float m_dir;
    int m_rob_id;
    bool firsttime;
    bool last_state;
public:    
    ConfigWidget* cfg;
    dSpaceID space;
    PCylinder* chassis;
    PBall* dummy;
    dJointID dummy_to_chassis;
    PBox* boxes[3];    
    bool on;
    //these values are not controled by this class
    bool selected;
    float select_x,select_y,select_z;    
    QImage *img,*number;
    class Wheel
    {
      public:
        int id;
        Wheel(Robot* robot,int _id,float ang,float ang2,int wheeltexid);
        void step();
        dJointID joint;
        dJointID motor;
        PCylinder* cyl;
        float speed;
        Robot* rob;
    } *wheels[4];
    class Kicker
    {
      private:
        bool kicking;
        int rolling;
        int kickstate;
        float m_kickspeed,m_kicktime;
      public:
        Kicker(Robot* robot);
        void step();
        void kick(float kickspeedx, float kickspeedz);
        void setRoller(int roller);
        int getRoller();
        void toggleRoller();
        bool isTouchingBall();
        dJointID joint;
        PBox* box;
        Robot* rob;
    } *kicker;

    Robot(PWorld* world,PBall* ball,ConfigWidget* _cfg,float x,float y,float z,float r,float g,float b,int rob_id,int wheeltexid,int dir);
    ~Robot();
    void step();
    void drawLabel();
    void setSpeed(int i,float s); //i = 0,1,2,3
    void setSpeed(double vx, double vy, double vw);
    float getSpeed(int i);
    void incSpeed(int i,float v);
    void resetSpeeds();
    void resetRobot();
    void getXY(float& x,float& y);
    float getDir();
    void setXY(float x,float y);
    void setDir(float ang);
    int getID();
    PBall* getBall();
};


#define ROBOT_START_Z(cfg)  (cfg->robotSettings.RobotHeight*0.5 + cfg->robotSettings.WheelRadius*1.1 + cfg->robotSettings.BottomHeight)

#endif // ROBOT_H
