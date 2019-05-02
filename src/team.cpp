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

#include "grSim_Packet.pb.h"

#include "grsim/team.h"

void DefaultTeam::create_robots(std::vector<PtrRobot>& all_robots, unsigned int nb_robots) {
    all_robots.reserve(all_robots.size() + nb_robots);
    const size_t nb_robot_other_team = all_robots.size();
    robots.reserve(robots.size() + nb_robots);
    for (unsigned id = 0; id < nb_robots; ++id) {
        PtrRobot r = std::make_shared<Robot>(nb_robot_other_team + id + 1);
        all_robots.push_back(r);
        robots.push_back(r);
    }
}

std::vector<Team::Status> DefaultTeam::handle_cmds(char *in_buffer, int size) {
    grSim_Packet packet;
    packet.ParseFromArray(in_buffer, size);

    if (!packet.has_commands() || packet.commands().isteamyellow() != is_yellow) {
        return {};
    }
    std::vector<Status> statuses;
    statuses.reserve(packet.commands().robot_commands_size());
    for (int i = 0; i < packet.commands().robot_commands_size(); i++)
    {
        const grSim_Robot_Command& cmd = packet.commands().robot_commands(i);
        if (!cmd.has_id())
            continue;
        unsigned int id = cmd.id();
        if (id >= robots.size())
            continue;
        PtrRobot robot = robots.at(id);
        if (cmd.has_wheelsspeed() && cmd.wheelsspeed()) {
            if (cmd.has_wheel1()) robot->setSpeed(0, (double)cmd.wheel1());
            if (cmd.has_wheel2()) robot->setSpeed(1, (double)cmd.wheel2());
            if (cmd.has_wheel3()) robot->setSpeed(2, (double)cmd.wheel3());
            if (cmd.has_wheel4()) robot->setSpeed(3, (double)cmd.wheel4());
        } else {
            dReal vx = cmd.has_veltangent() ? cmd.veltangent() : 0;
            dReal vy = cmd.has_velnormal() ? cmd.velnormal() : 0;
            dReal vw = cmd.has_velangular() ? cmd.velangular() : 0;
            robot->setSpeed(vx, vy, vw);
        }

        dReal kickx = cmd.kickspeedx();
        dReal kickz = cmd.kickspeedz();
        if (kickx>0.0001 || kickz>0.0001)
            robot->kicker->kick(kickx, kickz);
        int rolling = cmd.spinner()? 1 : 0;
        robot->kicker->setRoller(rolling);

        char status = 0;
        status = (char)id; // TODO: This is so broken, an id larger than 8 will always be touching the ball...
        if (robot->kicker->isTouchingBall()) status = status | 8;
        if (robot->on) status = status | 240;
        statuses.push_back(status);
    }
    return statuses;
}


Team* create_default_team(bool is_yellow) {
    return new DefaultTeam(is_yellow);
//    return std::unique_ptr<Team>(new DefaultTeam(is_yellow));
}
