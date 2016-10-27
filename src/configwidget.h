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

#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QWidget>
#include <QDockWidget>
#include <QtGui>
#include <QSplitter>
#include <QStringList>
#include <QMainWindow>
#include <QSettings>

#include <stdint.h>
#include <stdio.h>

#include <vartypes/VarTreeModel.h>
#include <vartypes/VarItem.h>
#include <vartypes/VarTreeView.h>
#include <vartypes/VarXML.h>
#include <vartypes/VarList.h>
#include <vartypes/VarDouble.h>
#include <vartypes/VarBool.h>
#include <vartypes/VarInt.h>
#include <vartypes/VarTrigger.h>
#include <vartypes/VarXML.h>
#include <vartypes/VarTypes.h>

using namespace VarTypes;

#define DEF_VALUE(type,Type,name)  \
            std::tr1::shared_ptr<VarTypes::Var##Type> v_##name; \
            inline type name() {return v_##name->get##Type();}

#define DEF_ENUM(type,name)  \
            std::tr1::shared_ptr<VarTypes::VarStringEnum> v_##name; \
            type name() {if(v_##name!=NULL) return v_##name->getString();return * (new type);}

#define DEF_TREE(name)  \
            std::tr1::shared_ptr<VarTypes::VarList> name;
#define DEF_PTREE(parents, name)  \
            std::tr1::shared_ptr<VarTypes::VarList> parents##_##name;



class RobotSettings {
public:
    //geometeric settings
    double RobotCenterFromKicker;
    double RobotRadius;
    double RobotHeight;
    double BottomHeight;
    double KickerZ;
    double KickerThickness;
    double KickerWidth;
    double KickerHeight;
    double WheelRadius;
    double WheelThickness;
    double Wheel1Angle;
    double Wheel2Angle;
    double Wheel3Angle;
    double Wheel4Angle;
    //physical settings
    double BodyMass;
    double WheelMass;
    double KickerMass;
    double KickerDampFactor;
    double RollerTorqueFactor;
    double RollerPerpendicularTorqueFactor;
    double Kicker_Friction;
    double WheelTangentFriction;
    double WheelPerpendicularFriction;
    double Wheel_Motor_FMax;
};


class ConfigWidget : public VarTreeView
{
  Q_OBJECT

protected:
  vector<VarPtr> world;
  VarTreeModel * tmodel;    
public:
  VarListPtr geo_vars;
  ConfigWidget();
  virtual ~ConfigWidget();

  QSettings* robot_settings;
  RobotSettings robotSettings;
  RobotSettings blueSettings;
  RobotSettings yellowSettings;

  DEF_VALUE(double,Double,Field_Line_Width)
  DEF_VALUE(double,Double,Field_Length)
  DEF_VALUE(double,Double,Field_Width)
  DEF_VALUE(double,Double,Field_Rad)
  DEF_VALUE(double,Double,Field_Defense_Rad)
  DEF_VALUE(double,Double,Field_Defense_Stretch)
  DEF_VALUE(double,Double,Field_Free_Kick)
  DEF_VALUE(double,Double,Field_Penalty_Rad)//TODO: what's this??
  DEF_VALUE(double,Double,Field_Penalty_Line)
  DEF_VALUE(double,Double,Field_Penalty_Point)
  DEF_VALUE(double,Double,Field_Margin)
  DEF_VALUE(double,Double,Field_Referee_Margin)
  DEF_VALUE(double,Double,Wall_Thickness)
  DEF_VALUE(double,Double,Goal_Thickness)
  DEF_VALUE(double,Double,Goal_Depth)
  DEF_VALUE(double,Double,Goal_Width)
  DEF_VALUE(double,Double,Goal_Height)
  DEF_ENUM(std::string,YellowTeam)
  DEF_ENUM(std::string,BlueTeam)
  DEF_VALUE(double,Double,BallRadius)
  DEF_VALUE(double,Double,BallMass)
  DEF_VALUE(double,Double,BallFriction)
  DEF_VALUE(double,Double,BallSlip)
  DEF_VALUE(double,Double,BallBounce)
  DEF_VALUE(double,Double,BallBounceVel)
  DEF_VALUE(double,Double,BallLinearDamp)
  DEF_VALUE(double,Double,BallAngularDamp)

  DEF_VALUE(bool,Bool,SyncWithGL)
  DEF_VALUE(double,Double,DesiredFPS)
  DEF_VALUE(double,Double,DeltaTime)
  DEF_VALUE(int,Int,sendGeometryEvery)
  DEF_VALUE(double,Double,Gravity)
  DEF_VALUE(std::string,String,VisionMulticastAddr)  
  DEF_VALUE(int,Int,VisionMulticastPort)  
  DEF_VALUE(int,Int,CommandListenPort)
  DEF_VALUE(int,Int,BlueStatusSendPort)
  DEF_VALUE(int,Int,YellowStatusSendPort)
  DEF_VALUE(int,Int,sendDelay)
  DEF_VALUE(bool,Bool,noise)
  DEF_VALUE(double,Double,noiseDeviation_x)
  DEF_VALUE(double,Double,noiseDeviation_y)
  DEF_VALUE(double,Double,noiseDeviation_angle)
  DEF_VALUE(bool,Bool,vanishing)
  DEF_VALUE(double,Double,ball_vanishing)
  DEF_VALUE(double,Double,blue_team_vanishing)
  DEF_VALUE(double,Double,yellow_team_vanishing)
  DEF_VALUE(std::string, String, plotter_addr)
  DEF_VALUE(int, Int, plotter_port)
  DEF_VALUE(bool, Bool, plotter)  
  void loadRobotSettings(QString team);
public slots:  
  void loadRobotsSettings();
};

class ConfigDockWidget : public QDockWidget
{
    Q_OBJECT
  public:
    QWidget* parent;
    ConfigWidget* conf;
    ConfigDockWidget(QWidget* _parent,ConfigWidget* _conf);
  protected:
    void closeEvent(QCloseEvent* event);
  signals:
    void closeSignal(bool);
};

#endif // CONFIGWIDGET_H
