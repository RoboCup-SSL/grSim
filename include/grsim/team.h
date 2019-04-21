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

#ifndef TEAM_H
#define TEAM_H

#include "grsim/robot.h"


class Team
{
protected:
    std::vector<PtrRobot> robots;
    bool is_yellow;
public:
    typedef char Status;

    Team(bool is_yellow): is_yellow(is_yellow) {}
    virtual ~Team() {};

    virtual void create_robots(std::vector<PtrRobot>& all_robots, unsigned int nb_robots) = 0;
    virtual std::vector<Status> handle_cmds(char *in_buffer, int size) = 0;
};
class DefaultTeam : public Team
{
public:

    DefaultTeam(bool is_yellow): Team(is_yellow) {}
    virtual ~DefaultTeam() {}

    virtual void create_robots(std::vector<PtrRobot>& all_robots, unsigned int nb_robots);
    virtual std::vector<Status> handle_cmds(char *in_buffer, int size);
};


typedef std::unique_ptr<Team> PtrTeam;
typedef Team* (create_team_t)(bool is_yellow);

Team* create_default_team(bool is_yellow);

#endif // TEAM_H
