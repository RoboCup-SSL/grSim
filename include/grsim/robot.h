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

#include <memory>

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
    dReal m_r,m_g,m_b;
    dReal m_dir;
    int m_rob_id;
    bool firsttime;
    bool last_state;
public:
    RobotSettings settings;
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

    class Drive {
      public:
        Drive(Robot* robot) : robot_(robot) {
        }
        virtual ~Drive() {
        }
        virtual void robotPoseChanged() = 0;
        virtual void setSpeed(int i, double speed) = 0;
        virtual double getSpeed(int i) = 0;
        virtual void incSpeed(int i, dReal v) = 0;
        virtual void setVelocity(double vx, double vy, double vw) = 0;
        virtual void forcestop() = 0;
        virtual void step() = 0;
        virtual std::vector<PObject*> getObjects() = 0;
      protected:
        Robot* robot_;
    };

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
    };

    class DefaultDrive : public Drive {
      public:
        DefaultDrive(Robot* robot, int wheeltexid) : Drive(robot) {
            wheels_[0] = std::unique_ptr<Wheel>(new Wheel(robot,0,robot->settings.Wheel1Angle,robot->settings.Wheel1Angle,wheeltexid));
            wheels_[1] = std::unique_ptr<Wheel>(new Wheel(robot,1,robot->settings.Wheel2Angle,robot->settings.Wheel2Angle,wheeltexid));
            wheels_[2] = std::unique_ptr<Wheel>(new Wheel(robot,2,robot->settings.Wheel3Angle,robot->settings.Wheel3Angle,wheeltexid));
            wheels_[3] = std::unique_ptr<Wheel>(new Wheel(robot,3,robot->settings.Wheel4Angle,robot->settings.Wheel4Angle,wheeltexid));
        }
        virtual ~DefaultDrive() {
        }
        virtual void robotPoseChanged() {
            // position
            dReal robotx, roboty, robotz;
            dReal robotdirx, robotdiry, robotdirz;
            dReal localx, localy, localz;
            // rob->chassis->getBodyPosition(robotx, roboty, robotz);
            // rob->chassis->getBodyDirection(robotdirx, robotdiry, robotdirz);
            // box->getBodyPosition(localx, localy, localz, /*local*/ true);
            // box->setBodyPosition(robotx + robotdirx * localx, roboty + robotdiry * localy, robotz + robotdirz * localz);

            robot_->chassis->getBodyPosition(robotx, roboty, robotz);
            robot_->chassis->getBodyDirection(robotdirx, robotdiry, robotdirz);
            //dReal height = ROBOT_START_Z(robot_->cfg);
            for (int i = 0; i < 4; i++) {
                wheels_[i]->cyl->getBodyPosition(localx, localy, localz, true);
                wheels_[i]->cyl->setBodyPosition(robotx + robotdirx * localx, roboty + robotdiry * localy, robotz + robotdirz * localz);
            }
            // direction
            dMatrix3 wLocalRot, wRot, cRot;
            dVector3 localPos,finalPos,cPos;
            robot_->chassis->getBodyPosition(cPos[0],cPos[1],cPos[2],false);
            robot_->chassis->getBodyRotation(cRot,false);
            for (int i = 0; i < 4; i++) {
                wheels_[i]->cyl->getBodyRotation(wLocalRot,true);
                dMultiply0(wRot,cRot,wLocalRot,3,3,3);
                dBodySetRotation(wheels_[i]->cyl->body,wRot);
                wheels_[i]->cyl->getBodyPosition(localPos[0],localPos[1],localPos[2],true);
                dMultiply0(finalPos,cRot,localPos,4,3,1);finalPos[0]+=cPos[0];finalPos[1]+=cPos[1];finalPos[2]+=cPos[2];
                wheels_[i]->cyl->setBodyPosition(finalPos[0],finalPos[1],finalPos[2],false);
            }
        }
        void setSpeed(int i, double speed) {
            if (!((i>=4) || (i<0))) {
                wheels_[i]->speed = speed;
            }
        }
        double getSpeed(int i) {
            if ((i>=4) || (i<0)) return -1;
            return wheels_[i]->speed;
        }
        void incSpeed(int i, dReal v) {
            if (!((i>=4) || (i<0)))
                wheels_[i]->speed += v;
        }
        void setVelocity(double vx, double vy, double vw) {
            // Calculate Motor Speeds
            dReal _DEG2RAD = M_PI / 180.0;
            dReal motorAlpha[4] = {robot_->settings.Wheel1Angle * _DEG2RAD, robot_->settings.Wheel2Angle * _DEG2RAD, robot_->settings.Wheel3Angle * _DEG2RAD, robot_->settings.Wheel4Angle * _DEG2RAD};

            dReal dw1 =  (1.0 / robot_->settings.WheelRadius) * (( (robot_->settings.RobotRadius * vw) - (vx * sin(motorAlpha[0])) + (vy * cos(motorAlpha[0]))) );
            dReal dw2 =  (1.0 / robot_->settings.WheelRadius) * (( (robot_->settings.RobotRadius * vw) - (vx * sin(motorAlpha[1])) + (vy * cos(motorAlpha[1]))) );
            dReal dw3 =  (1.0 / robot_->settings.WheelRadius) * (( (robot_->settings.RobotRadius * vw) - (vx * sin(motorAlpha[2])) + (vy * cos(motorAlpha[2]))) );
            dReal dw4 =  (1.0 / robot_->settings.WheelRadius) * (( (robot_->settings.RobotRadius * vw) - (vx * sin(motorAlpha[3])) + (vy * cos(motorAlpha[3]))) );

            setSpeed(0 , dw1);
            setSpeed(1 , dw2);
            setSpeed(2 , dw3);
            setSpeed(3 , dw4);
        }
        void forcestop() {
            for (int i = 0; i < 4; i++) {
                dBodySetLinearVel(wheels_[i]->cyl->body,0,0,0);
                dBodySetAngularVel(wheels_[i]->cyl->body,0,0,0);
            }
        }
        void step() {
            for (int i = 0; i < 4; i++) {
                wheels_[i]->step();
            }
        }
        std::vector<PObject*> getObjects() {
            std::vector<PObject*> objs(4);
            for (int i = 0; i < 4; i++) {
                objs[i] = wheels_[i]->cyl;
            }
            return objs;
        }
      protected:
        std::unique_ptr<Wheel> wheels_[4];
    };
    Drive* drive;

    Robot(unsigned int rob_id) : firsttime(true), on(true), m_rob_id(rob_id) {
    }
    //Robot(PWorld* world,PBall* ball,ConfigWidget* _cfg,dReal x,dReal y,dReal z,dReal r,dReal g,dReal b,int rob_id,int wheeltexid,int dir);
    virtual ~Robot();
    virtual void initialize(PWorld* world, PBall* ball, ConfigWidget* cfg, RobotSettings settings, dReal x, dReal y, dReal z, dReal r, dReal g, dReal b, int wheeltexid, int dir);
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
    PWorld* getWorld();
};

typedef std::shared_ptr<Robot> PtrRobot;
typedef std::vector<PtrRobot> PtrRobots;

double getRobotZ(const RobotSettings& settings);

#endif // ROBOT_H
