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

#include <boost/filesystem.hpp>

#include "grsim/configwidget.h"
#include "grsim/config.h"

#define ADD_ENUM(type,name,Defaultvalue,namestring) \
    v_##name = std::shared_ptr<Var##type>(new Var##type(namestring,Defaultvalue));
#define ADD_VALUE(parent,type,name,defaultvalue,namestring) \
    v_##name = std::shared_ptr<Var##type>(new Var##type(namestring,defaultvalue)); \
    parent->addChild(v_##name);

#define END_ENUM(parents, name) \
    parents->addChild(v_##name);
#define ADD_TO_ENUM(name,str) \
    v_##name->addItem(str);


ConfigWidget::ConfigWidget(boost::optional<std::string> cli_yellowteam, boost::optional<std::string> cli_blueteam) : teamConfigPath()
{
  loadTeamConfigPath();

  tmodel=new VarTreeModel();
  this->setModel(tmodel);  
  geo_vars = VarListPtr(new VarList("Geometry"));
  world.push_back(geo_vars);

  VarListPtr game_vars(new VarList("Game"));
  geo_vars->addChild(game_vars);
  ADD_ENUM(StringEnum, Division, "Division A", "Division")
  ADD_TO_ENUM(Division, "Division A");
  ADD_TO_ENUM(Division, "Division B");
  END_ENUM(game_vars, Division);
  ADD_VALUE(game_vars,Int, Robots_Count, 8, "Robots Count")
  VarListPtr fields_vars(new VarList("Field"));
  VarListPtr div_a_vars(new VarList("Division A"));
  VarListPtr div_b_vars(new VarList("Division B"));
  geo_vars->addChild(fields_vars);
  fields_vars->addChild(div_a_vars);
  fields_vars->addChild(div_b_vars);

  ADD_VALUE(div_a_vars, Double, DivA_Field_Line_Width,0.010,"Line Thickness")
  ADD_VALUE(div_a_vars, Double, DivA_Field_Length,12.000,"Length")
  ADD_VALUE(div_a_vars, Double, DivA_Field_Width,9.000,"Width")
  ADD_VALUE(div_a_vars, Double, DivA_Field_Rad,0.500,"Radius")
  ADD_VALUE(div_a_vars, Double, DivA_Field_Free_Kick,0.700,"Free Kick Distance From Defense Area")
  ADD_VALUE(div_a_vars, Double, DivA_Field_Penalty_Width,2.40,"Penalty width")
  ADD_VALUE(div_a_vars, Double, DivA_Field_Penalty_Depth,1.20,"Penalty depth")
  ADD_VALUE(div_a_vars, Double, DivA_Field_Penalty_Point,1.20,"Penalty point")
  ADD_VALUE(div_a_vars, Double, DivA_Field_Margin,0.3,"Margin")
  ADD_VALUE(div_a_vars, Double, DivA_Field_Referee_Margin,0.4,"Referee margin")
  ADD_VALUE(div_a_vars, Double, DivA_Wall_Thickness,0.050,"Wall thickness")
  ADD_VALUE(div_a_vars, Double, DivA_Goal_Thickness,0.020,"Goal thickness")
  ADD_VALUE(div_a_vars, Double, DivA_Goal_Depth,0.200,"Goal depth")
  ADD_VALUE(div_a_vars, Double, DivA_Goal_Width,1.200,"Goal width")
  ADD_VALUE(div_a_vars, Double, DivA_Goal_Height,0.160,"Goal height")

  ADD_VALUE(div_b_vars, Double, DivB_Field_Line_Width,0.010,"Line Thickness")
  ADD_VALUE(div_b_vars, Double, DivB_Field_Length,9.000,"Length")
  ADD_VALUE(div_b_vars, Double, DivB_Field_Width,6.000,"Width")
  ADD_VALUE(div_b_vars, Double, DivB_Field_Rad,0.500,"Radius")
  ADD_VALUE(div_b_vars, Double, DivB_Field_Free_Kick,0.700,"Free Kick Distance From Defense Area")
  ADD_VALUE(div_b_vars, Double, DivB_Field_Penalty_Width,2.00,"Penalty width")
  ADD_VALUE(div_b_vars, Double, DivB_Field_Penalty_Depth,1.0,"Penalty depth")
  ADD_VALUE(div_b_vars, Double, DivB_Field_Penalty_Point,1.00,"Penalty point")
  ADD_VALUE(div_b_vars, Double, DivB_Field_Margin,0.30,"Margin")
  ADD_VALUE(div_b_vars, Double, DivB_Field_Referee_Margin,0.4,"Referee margin")
  ADD_VALUE(div_b_vars, Double, DivB_Wall_Thickness,0.050,"Wall thickness")
  ADD_VALUE(div_b_vars, Double, DivB_Goal_Thickness,0.020,"Goal thickness")
  ADD_VALUE(div_b_vars, Double, DivB_Goal_Depth,0.200,"Goal depth")
  ADD_VALUE(div_b_vars, Double, DivB_Goal_Width,1.000,"Goal width")
  ADD_VALUE(div_b_vars, Double, DivB_Goal_Height,0.160,"Goal height")

    VarListPtr ballg_vars(new VarList("Ball"));
    geo_vars->addChild(ballg_vars);
        ADD_VALUE(ballg_vars,Double,BallRadius,0.0215,"Radius")
  VarListPtr phys_vars(new VarList("Physics"));
  world.push_back(phys_vars);
    VarListPtr worldp_vars(new VarList("World"));
    phys_vars->addChild(worldp_vars);  
        ADD_VALUE(worldp_vars,Double,DesiredFPS,65,"Desired FPS")
        ADD_VALUE(worldp_vars,Bool,SyncWithGL,false,"Synchronize ODE with OpenGL")
        ADD_VALUE(worldp_vars,Double,DeltaTime,0.016,"ODE time step")
        ADD_VALUE(worldp_vars,Double,Gravity,9.8,"Gravity")
  VarListPtr ballp_vars(new VarList("Ball"));
    phys_vars->addChild(ballp_vars);
        ADD_VALUE(ballp_vars,Double,BallMass,0.043,"Ball mass");
        ADD_VALUE(ballp_vars,Double,BallFriction,0.05,"Ball-ground friction")
        ADD_VALUE(ballp_vars,Double,BallSlip,1,"Ball-ground slip")
        ADD_VALUE(ballp_vars,Double,BallBounce,0.5,"Ball-ground bounce factor")
        ADD_VALUE(ballp_vars,Double,BallBounceVel,0.1,"Ball-ground bounce min velocity")
        ADD_VALUE(ballp_vars,Double,BallLinearDamp,0.004,"Ball linear damping")
        ADD_VALUE(ballp_vars,Double,BallAngularDamp,0.004,"Ball angular damping")
  VarListPtr comm_vars(new VarList("Communication"));
  world.push_back(comm_vars);
    ADD_VALUE(comm_vars,String,VisionMulticastAddr,"224.5.23.2","Vision multicast address")  //SSL Vision: "224.5.23.2"
    ADD_VALUE(comm_vars,Int,VisionMulticastPort,10020,"Vision multicast port")
    ADD_VALUE(comm_vars,Int,CommandListenPort,20011,"Command listen port")
    ADD_VALUE(comm_vars,Int,BlueStatusSendPort,30011,"Blue Team status send port")
    ADD_VALUE(comm_vars,Int,YellowStatusSendPort,30012,"Yellow Team status send port")
    ADD_VALUE(comm_vars,Int,sendDelay,0,"Sending delay (milliseconds)")
    ADD_VALUE(comm_vars,Int,sendGeometryEvery,120,"Send geometry every X frames")
    VarListPtr gauss_vars(new VarList("Gaussian noise"));
        comm_vars->addChild(gauss_vars);
        ADD_VALUE(gauss_vars,Bool,noise,false,"Noise")
        ADD_VALUE(gauss_vars,Double,noiseDeviation_x,3,"Deviation for x values")
        ADD_VALUE(gauss_vars,Double,noiseDeviation_y,3,"Deviation for y values")
        ADD_VALUE(gauss_vars,Double,noiseDeviation_angle,2,"Deviation for angle values")
    VarListPtr vanishing_vars(new VarList("Vanishing probability"));
        comm_vars->addChild(vanishing_vars);
        ADD_VALUE(gauss_vars,Bool,vanishing,false,"Vanishing")
        ADD_VALUE(vanishing_vars,Double,blue_team_vanishing,0,"Blue team")
        ADD_VALUE(vanishing_vars,Double,yellow_team_vanishing,0,"Yellow team")
        ADD_VALUE(vanishing_vars,Double,ball_vanishing,0,"Ball")

    ADD_ENUM(StringEnum,YellowTeam,DEFAULT_TEAM,"Yellow Team")
    END_ENUM(geo_vars,YellowTeam)
    ADD_ENUM(StringEnum,BlueTeam,DEFAULT_TEAM,"Blue Team")
    END_ENUM(geo_vars,BlueTeam)

    // Loading xml configuration
    world = VarXML::read(world,(QDir::homePath() + QString("/.grsim.xml")).toStdString());
    tmodel->setRootItems(world);

    // Save team selected in xml
    std::string xml_blueteam = BlueTeam();
    std::string xml_yellowteam = YellowTeam();
    // Remove list of teams from XML, since they may not exist anymore
    v_BlueTeam->resetToDefault();
    v_YellowTeam->resetToDefault();

    // Populate the list of available teams
    std::map<std::string, team_config_t>::iterator it;
    for ( it = teamConfigPath.begin(); it != teamConfigPath.end(); it++ ) {
        std::string teamname = it->first;
        ADD_TO_ENUM(BlueTeam, teamname)
        ADD_TO_ENUM(YellowTeam, teamname)
    }

    // The cli has priority over the xml configuration
    std::string selected_blueteam = cli_blueteam ? cli_blueteam.get() : xml_blueteam;
    std::string selected_yellowteam = cli_yellowteam ? cli_yellowteam.get() : xml_yellowteam;

    // Check that the team configurations actually exists.
    if(teamConfigPath.find(selected_blueteam) == teamConfigPath.end()) {
        std::cerr << "Can not find '" << selected_blueteam << "', the blue team configuration. Check that your config/ folder exists and containts the .ini ." << std::endl;
        std::cerr << "Reverting to the default team '" << DEFAULT_TEAM << "'"<< std::endl;
        selected_blueteam = DEFAULT_TEAM;
    }

    if (teamConfigPath.find(selected_yellowteam) == teamConfigPath.end()) {
        std::cerr << "Can not find '" << selected_yellowteam << "', the yellow team configuration. Check that your config/ folder exists and containts the .ini ." << std::endl;
        std::cerr << "Reverting to the default team '" << DEFAULT_TEAM << "'"<< std::endl;
        selected_yellowteam = DEFAULT_TEAM;
    }
    // If the command line force a different team configuration, we switch to it
    v_BlueTeam->setString(selected_blueteam);
    v_YellowTeam->setString(selected_yellowteam);

    std::cout << "Using " << selected_blueteam << " configuration for blue team" << std::endl;
    std::cout << "Using " << selected_yellowteam << " configuration for yellow team" << std::endl;

    this->expandAndFocus(geo_vars);
    this->expandAndFocus(phys_vars);
    this->expandAndFocus(comm_vars);

    this->fitColumns();

    resize(320,400);
    connect(v_BlueTeam.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(loadRobotsSettings()));
    connect(v_YellowTeam.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(loadRobotsSettings()));
    loadRobotsSettings();
}

ConfigWidget::~ConfigWidget(){
   VarXML::write(world,(QDir::homePath() + QString("/.grsim.xml")).toStdString());
}


ConfigDockWidget::ConfigDockWidget(QWidget* _parent,ConfigWidget* _conf){
    parent=_parent;conf=_conf;
    setWidget(conf);
    conf->move(0,20);
}  
void ConfigDockWidget::closeEvent(QCloseEvent*)
{
    emit closeSignal(false);
}


void ConfigWidget::loadRobotsSettings()
{
    loadRobotSettings(YellowTeam());
    yellowSettings = robotSettings;
    loadRobotSettings(BlueTeam());
    blueSettings = robotSettings;
}


void ConfigWidget::loadTeamConfigPath() {
    teamConfigPath.clear();

    QDir dir;

    dir.setCurrent(CONFIG_DIR);
    dir.setNameFilters(QStringList() << "*.ini");
    dir.setSorting(QDir::Size | QDir::Reversed);
    QFileInfoList list = dir.entryInfoList();

    dir.setCurrent(qApp->applicationDirPath()+"/../share/grsim/config/");
    dir.setNameFilters(QStringList() << "*.ini");
    dir.setSorting(QDir::Size | QDir::Reversed);
    list += dir.entryInfoList();

    dir.setCurrent(qApp->applicationDirPath()+"/../config/");
    dir.setNameFilters(QStringList() << "*.ini");
    dir.setSorting(QDir::Size | QDir::Reversed);
    list += dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QStringList s = fileInfo.fileName().split(".");
        if (fileInfo.exists() && fileInfo.canonicalFilePath().toStdString() != "" && s.count() > 0 && s[0] != "") {
            std::string teamname = s[0].toStdString();
            team_config_t configPath;
            configPath.path_ini = fileInfo.canonicalFilePath();

            // Check if team's configuration required loading a custom DLL
            QSettings iniSetting(configPath.path_ini, QSettings::IniFormat);
            if (iniSetting.contains("CustomTeamPlugin/NameDLL")) {
                std::cout << teamname << " have custom team plugin" << std::endl;
                const std::string nameDLL = iniSetting.value("CustomTeamPlugin/NameDLL", "").toString().toStdString();
                if (nameDLL != "")
                    configPath.path_dll = findTeamPluginPath(nameDLL);
            }
            teamConfigPath.insert(std::make_pair(teamname, configPath));
        }
    }
    std::cout << "[Team configuration found]" << std::endl;
    std::map<std::string, team_config_t>::iterator it;
    for ( it = teamConfigPath.begin(); it != teamConfigPath.end(); it++ ) {
        std::cout << "\t" << it->first << "  => init conf == " << it->second.path_ini.toStdString() << std::endl;
    }
    // Sanity check
    if (teamConfigPath.find(DEFAULT_TEAM) == teamConfigPath.end()) {
        std::cerr << "Can not find '" << DEFAULT_TEAM << "', the default team configuration. Check that your config folder exist" << std::endl;
        exit(-1);
    }
}

boost::optional<std::string> ConfigWidget::findTeamPluginPath(const std::string& teamname) {
    std::vector<boost::filesystem::path> target_paths;
    boost::filesystem::path default_plugin_path((QDir::homePath()+"/.grsim/plugins/").toStdString());
    if (!boost::filesystem::exists(default_plugin_path)) {
        std::cerr << default_plugin_path << " does not exist" << std::endl;
    } else {
        target_paths.push_back(default_plugin_path);
    }
    const char* env_grsim_plugin_path = getenv("GRSIM_PLUGIN_PATH");
    if (env_grsim_plugin_path != nullptr) {
        boost::filesystem::path grsim_plugin_path = std::string(env_grsim_plugin_path);
        if (!boost::filesystem::exists(grsim_plugin_path)) {
            std::cerr << grsim_plugin_path << " does not exist" << std::endl;
        } else {
            target_paths.push_back(grsim_plugin_path);
        }
    }
    for (const boost::filesystem::path& target_path: target_paths) {
        boost::filesystem::directory_iterator end_itr;
        for( boost::filesystem::directory_iterator iter(target_path); iter != end_itr; ++iter ) {
            boost::filesystem::path path = iter->path();
            if (boost::filesystem::is_regular_file(path) && path.stem() == teamname) {
                std::cout << "Found DLL " << path.string() << " for team " << teamname << std::endl;
                return path.string();
            }
        }
    }
    std::cerr << "Can not find the Custom Team Plugin for team " << teamname << std::endl;
    return {};
}

void ConfigWidget::loadRobotSettings(std::string team)
{
    if (teamConfigPath.find(team) == teamConfigPath.end()) {
        std::cerr << "Can not find configuration for team '" << team << "', fallback to default." << std::endl;
        team = DEFAULT_TEAM;
    }
    QString ss = teamConfigPath[team].path_ini;
    std::cout << "Loading team " << team << " at " << ss.toStdString() << std::endl;
    QSettings ini_setting(ss, QSettings::IniFormat);
    robotSettings.RobotCenterFromKicker = ini_setting.value("Geometery/CenterFromKicker", 0.073).toDouble();
    robotSettings.RobotRadius = ini_setting.value("Geometery/Radius", 0.09).toDouble();
    robotSettings.RobotHeight = ini_setting.value("Geometery/Height", 0.13).toDouble();
    robotSettings.BottomHeight = ini_setting.value("Geometery/RobotBottomZValue", 0.02).toDouble();
    robotSettings.KickerZ = ini_setting.value("Geometery/KickerZValue", 0.005).toDouble();
    robotSettings.KickerThickness = ini_setting.value("Geometery/KickerThickness", 0.005).toDouble();
    robotSettings.KickerWidth = ini_setting.value("Geometery/KickerWidth", 0.08).toDouble();
    robotSettings.KickerHeight = ini_setting.value("Geometery/KickerHeight", 0.04).toDouble();
    robotSettings.WheelRadius = ini_setting.value("Geometery/WheelRadius", 0.0325).toDouble();
    robotSettings.WheelThickness = ini_setting.value("Geometery/WheelThickness", 0.005).toDouble();
    robotSettings.Wheel1Angle = ini_setting.value("Geometery/Wheel1Angle", 60).toDouble();
    robotSettings.Wheel2Angle = ini_setting.value("Geometery/Wheel2Angle", 135).toDouble();
    robotSettings.Wheel3Angle = ini_setting.value("Geometery/Wheel3Angle", 225).toDouble();
    robotSettings.Wheel4Angle = ini_setting.value("Geometery/Wheel4Angle", 300).toDouble();

    robotSettings.BodyMass  = ini_setting.value("Physics/BodyMass", 2).toDouble();
    robotSettings.WheelMass = ini_setting.value("Physics/WheelMass", 0.2).toDouble();
    robotSettings.KickerMass= ini_setting.value("Physics/KickerMass",0.02).toDouble();
    robotSettings.KickerDampFactor = ini_setting.value("Physics/KickerDampFactor", 0.2f).toDouble();
    robotSettings.RollerTorqueFactor = ini_setting.value("Physics/RollerTorqueFactor", 0.06f).toDouble();
    robotSettings.RollerPerpendicularTorqueFactor = ini_setting.value("Physics/RollerPerpendicularTorqueFactor", 0.005f).toDouble();
    robotSettings.Kicker_Friction = ini_setting.value("Physics/KickerFriction", 0.8f).toDouble();
    robotSettings.WheelTangentFriction = ini_setting.value("Physics/WheelTangentFriction", 0.8f).toDouble();
    robotSettings.WheelPerpendicularFriction = ini_setting.value("Physics/WheelPerpendicularFriction", 0.05f).toDouble();
    robotSettings.Wheel_Motor_FMax = ini_setting.value("Physics/WheelMotorMaximumApplyingTorque", 0.2f).toDouble();
}
