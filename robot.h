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
public:    
    ConfigWidget* cfg;
    dSpaceID space;
    PCylinder* chassis;
    PBall* dummy;
    dJointID dummy_to_chassis;
    PBox* boxes[3];
    bool selected; //these values are not controled by this class
    float select_x,select_y,select_z;
    QImage *img,*number;
    class Wheel
    {
      public:
        Wheel(Robot* robot,float ang,float ang2,int wheeltexid);
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
        bool kicking,rolling;
        int kickstate;
        float m_kickspeed,m_kicktime;
      public:
        Kicker(Robot* robot);
        void step();
        void kick(float kickspeed,bool chip=false);
        void setRoller(bool roller);
        bool getRoller();
        void toggleRoller();
        bool isTouchingBall();
        dJointID joint;
        PBox* box;
        Robot* rob;
    } *kicker;

    Robot(PWorld* world,PBall* ball,ConfigWidget* _cfg,float x,float y,float z,float r,float g,float b,int rob_id,int wheeltexid,int dir);
    ~Robot();
    void step();
    void setSpeed(int i,float s); //i = 0,1,2,3
    float getSpeed(int i);
    void incSpeed(int i,float v);
    void resetSpeeds();
    void resetRobot();
    void getXY(float& x,float& y);
    float getDir();
    void setXY(float x,float y);
    void setDir(float ang);
    PBall* getBall();
};


#define ROBOT_START_Z(cfg)  (cfg->CHASSISHEIGHT()*0.5+cfg->WHEELRADIUS()*1.1-cfg->BOTTOMHEIGHT())

#endif // ROBOT_H
