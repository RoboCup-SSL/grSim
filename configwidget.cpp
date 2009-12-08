#include "configwidget.h"

#define ADD_VALUE(parent,type,name,defaultvalue,namestring) \
    v_##name = new Var##type(namestring,defaultvalue); \
    parent->addChild(v_##name);

ConfigWidget::ConfigWidget()
{  
  tmodel=new VarTreeModel();
  this->setModel(tmodel);  

  geo_vars = new VarList("Geometry");
  world.push_back(geo_vars);
    VarList * field_vars = new VarList("Field");    
    geo_vars->addChild(field_vars);
        ADD_VALUE(field_vars,Double,_SSL_FIELD_LENGTH,6050.0,"Length")
        ADD_VALUE(field_vars,Double,_SSL_FIELD_WIDTH,4050.0,"Width")
        ADD_VALUE(field_vars,Double,_SSL_FIELD_RAD,500.0,"Radius")
        ADD_VALUE(field_vars,Double,_SSL_FIELD_PENALTY,450.0,"Penalty")
        ADD_VALUE(field_vars,Double,_SSL_FIELD_MARGIN,1000.0,"Margin")
        ADD_VALUE(field_vars,Double,_SSL_WALL_THICKNESS,250.0,"Wall thickness")
    VarList * robotg_vars = new VarList("Robot");
    geo_vars->addChild(robotg_vars);
        ADD_VALUE(robotg_vars,Double,CHASSISLENGTH,0.17,"Chassis length")
        ADD_VALUE(robotg_vars,Double,CHASSISWIDTH,0.17,"Chassis width")
        ADD_VALUE(robotg_vars,Double,CHASSISHEIGHT,0.13,"Chassis height")
        ADD_VALUE(robotg_vars,Double,BOTTOMHEIGHT,0.02,"Chassis bottom Z value")
        ADD_VALUE(robotg_vars,Double,KICKERHEIGHT,0.005,"Kicker Z value")
        ADD_VALUE(robotg_vars,Double,KLENGTH,0.005,"Kicker length")
        ADD_VALUE(robotg_vars,Double,KWIDTH,0.08,"Kicker width")
        ADD_VALUE(robotg_vars,Double,KHEIGHT,0.04,"Kicker height")
        ADD_VALUE(robotg_vars,Double,WHEELRADIUS,0.035,"Wheel radius")
        ADD_VALUE(robotg_vars,Double,WHEELLENGTH,0.005,"Wheel thickness")
    VarList * ballg_vars = new VarList("Ball");
    geo_vars->addChild(ballg_vars);
        ADD_VALUE(ballg_vars,Double,BALLRADIUS,0.0215,"Radius")
  VarList * phys_vars = new VarList("Physics");
  world.push_back(phys_vars);
    VarList * worldp_vars = new VarList("World");
    phys_vars->addChild(worldp_vars);
        ADD_VALUE(worldp_vars,Bool,SyncWithGL,true,"Synchronize ODE with OpenGL")
        ADD_VALUE(worldp_vars,Double,DeltaTime,0.01,"ODE time step")
        ADD_VALUE(worldp_vars,Double,Gravity,9.8,"Gravity")
    VarList * robotp_vars = new VarList("Robot");
    phys_vars->addChild(robotp_vars);
        ADD_VALUE(robotp_vars,Double,CHASSISMASS,2,"Chassis mass")
        ADD_VALUE(robotp_vars,Double,WHEELMASS,0.2,"Wheel mass")
        ADD_VALUE(robotp_vars,Double,KICKERMASS,0.02,"Kicker mass")
        ADD_VALUE(robotp_vars,Double,KICKFACTOR,0.2f,"Kick factor")
        ADD_VALUE(robotp_vars,Double,CHIPFACTOR,0.6f,"Chip kick factor")
        ADD_VALUE(robotp_vars,Double,ROLLERTORQUEFACTOR,0.01f,"Roller torque factor")
        ADD_VALUE(robotp_vars,Double,wheeltangentfriction,0.7f,"Wheel tangent friction")
        ADD_VALUE(robotp_vars,Double,wheelperpendicularfriction,0.01f,"Wheel perpendicular friction")
    VarList * ballp_vars = new VarList("Ball");
    phys_vars->addChild(ballp_vars);
        ADD_VALUE(ballp_vars,Double,BALLMASS,0.1,"Ball mass");
        ADD_VALUE(ballp_vars,Double,ballfriction,1,"Ball-ground friction")
        ADD_VALUE(ballp_vars,Double,ballslip,1,"Ball-ground slip")
        ADD_VALUE(ballp_vars,Double,ballbounce,0.7,"Ball-ground bounce factor")
        ADD_VALUE(ballp_vars,Double,ballbouncevel,0.1,"Ball-ground bounce min velocity")
        ADD_VALUE(ballp_vars,Double,balllineardamp,0.004,"Ball linear damping")
        ADD_VALUE(ballp_vars,Double,ballangulardamp,0.004,"Ball angular damping")
  VarList * comm_vars = new VarList("Communication");
  world.push_back(comm_vars);
    ADD_VALUE(comm_vars,String,VisionMulticastAddr,"127.0.0.1","Vision multicast address")  //SSL Vision: "224.5.23.2"
    ADD_VALUE(comm_vars,Int,VisionMulticastPort,10002,"Vision multicast port")
    ADD_VALUE(comm_vars,Int,CommandListenPort,20011,"Command listen port")
    ADD_VALUE(comm_vars,Double,motorfactor,8,"Motor factor")
    ADD_VALUE(comm_vars,Double,shootfactor,1,"Shoot factor")
  world=VarXML::read(world,"../settings.xml");

  tmodel->setRootItems(world);


  this->expandAndFocus(geo_vars);
  this->expandAndFocus(phys_vars);
  this->expandAndFocus(comm_vars);

  this->fitColumns();

  resize(320,400);
}

ConfigWidget::~ConfigWidget() {  
   VarXML::write(world,"../settings.xml");
}


void ConfigWidget::closeEvent(QCloseEvent *event)
{
}

