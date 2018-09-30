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

#include "grsim/physics/pworld.h"
#include "grsim/physics/pcylinder.h"
#include "grsim/physics/pbox.h"
#include "grsim/physics/pball.h"
#include "grsim/configwidget.h"

class Robot
{
protected:
    PWorld* w;
    PBall* m_ball;
    dReal m_x,m_y,m_z;
    dReal m_r,m_g,m_b;
    dReal m_dir;
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
    dReal select_x,select_y,select_z;    
    QImage *img,*number;
    class Wheel
    {
      public:
        int id;
        Wheel(Robot* robot,int _id,dReal ang,dReal ang2,int wheeltexid);
        virtual ~Wheel();
        virtual void step();
        dJointID joint;
        dJointID motor;
        PCylinder* cyl;
        dReal speed;
        Robot* rob;
    } *wheels[4];
    class Kicker
    {
      public:
        Kicker(Robot* robot) {}
        virtual ~Kicker() {}
        virtual void step() = 0;
        virtual void kick(dReal kickspeedx, dReal kickspeedz) = 0;
        virtual void setRoller(int roller) = 0;
        virtual int getRoller() = 0;
        virtual void toggleRoller() = 0;
        virtual bool isTouchingBall() = 0;
        virtual PObject* getKickObject() = 0;
        //< NOTE: you only need to think about the case when an user change the robot manually, i.e. robot is ,parallel to the ground.
        virtual void robotPoseChanged() = 0;
    };

    class DefaultKicker: public Kicker {
      private:
        bool kicking;
        int rolling;
        int kickstate;
        dReal m_kickspeed,m_kicktime;
        dJointID joint;
        PBox* box;
        Robot* rob;

      public:
        DefaultKicker(Robot* robot);
        virtual ~DefaultKicker();
        virtual void step();
        virtual void kick(dReal kickspeedx, dReal kickspeedz);
        virtual void setRoller(int roller);
        virtual int getRoller();
        virtual void toggleRoller();
        virtual bool isTouchingBall();
        //< NOTE: you only need to think about the case when an user change the robot manually, i.e. robot is ,parallel to the ground.
        virtual void robotPoseChanged();
        virtual PObject* getKickObject() {
            return box;
        }
    };
    Kicker *kicker;

    Robot() : firsttime(true), on(true) {
    }
    //Robot(PWorld* world,PBall* ball,ConfigWidget* _cfg,dReal x,dReal y,dReal z,dReal r,dReal g,dReal b,int rob_id,int wheeltexid,int dir);
    virtual ~Robot();
    virtual void initialize(PWorld* world,PBall* ball,ConfigWidget* _cfg,dReal x,dReal y,dReal z,dReal r,dReal g,dReal b,int rob_id,int wheeltexid,int dir);
    virtual void step();
    virtual void drawLabel();
    virtual void setSpeed(int i,dReal s); //i = 0,1,2,3
    virtual void setSpeed(dReal vx, dReal vy, dReal vw);
    virtual dReal getSpeed(int i);
    virtual void incSpeed(int i,dReal v);
    virtual void resetSpeeds();
    virtual void resetRobot();
    virtual void getXY(dReal& x,dReal& y);
    virtual dReal getDir();
    virtual void setXY(dReal x,dReal y);
    virtual void setDir(dReal ang);
    virtual int getID();
    PBall* getBall();
};


#define ROBOT_START_Z(cfg)  (cfg->robotSettings.RobotHeight*0.5 + cfg->robotSettings.WheelRadius*1.1 + cfg->robotSettings.BottomHeight)

#endif // ROBOT_H
