package Client;

import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.net.InetAddress;
import Protobuf.*;


public class Conection {

DatagramSocket ds;
String ip;
int port, id;
byte [] buffer2;
int envio = 1;
float timeStamp, wheel1, wheel2, wheel3, wheel4, kickspeedx, kickspeedz, velx, vely, velz;
boolean spinner, wheelSpeed, teamYellow;

Conection () {

    ip="127.0.0.1";
    port=20011;
    timeStamp=0;
    wheel1=0;
    wheel2=0;
    wheel3=0;
    wheel4=0;
    kickspeedx=0;
    kickspeedz=0;
    velx=0;
    vely=0;
    velz=0;
    id=0;
    spinner=false;
    wheelSpeed=false;
    teamYellow=false;
}

public void setTime(float timeStamp){
    this.timeStamp=timeStamp;
}

public void setWheel1(float wheel1){
    this.wheel1=wheel1;
}

public void setWheel2(float wheel2){
    this.wheel2=wheel2;
}

public void setWheel3(float wheel3){
    this.wheel3=wheel3;
}

public void setWheel4(float wheel4){
    this.wheel4=wheel4;
}

public void setKickspeedX(float kickspeedx){
    this.kickspeedx=kickspeedx;
}

public void setKickspeedZ(float kickspeedz){
    this.kickspeedz=kickspeedz;
}

public void setVelX(float velx){
    this.velx=velx;
}

public void setVelY(float vely){
    this.vely=vely;
}

public void setVelZ(float velz){
    this.velz=velz;
}

public void setId(int id){
    this.id=id;
}

public void setSpinner(boolean spinner){
    this.spinner=spinner;
}

public void setWheelSpeed(boolean wheelSpeed){
    this.wheelSpeed=wheelSpeed;
}

public void setTeamYellow(boolean teamYellow){
    this.teamYellow=teamYellow;
}

public void setIp(String ip){
    this.ip=ip;
}

public void setPort(int port){
    this.port=port;
}

public void send(){
    GrSimCommands.grSim_Robot_Command command = GrSimCommands.grSim_Robot_Command.newBuilder().setId(this.id).setWheel2(this.wheel2).setWheel1(this.wheel1).setWheel3(this.wheel3).setWheel4(this.wheel4).setKickspeedx(this.kickspeedx).setKickspeedz(this.kickspeedz).setVeltangent(this.velx).setVelnormal(this.vely).setVelangular(this.velz).setSpinner(this.spinner).setWheelsspeed(this.wheelSpeed).build();
    GrSimCommands.grSim_Commands command2 = GrSimCommands.grSim_Commands.newBuilder().setTimestamp(this.timeStamp).setIsteamyellow(this.teamYellow).addRobotCommands(command).build();
    GrSimPacket.grSim_Packet packet = GrSimPacket.grSim_Packet.newBuilder().setCommands(command2).build();
    String buffer = packet.toString();
    buffer2=packet.toByteArray();
    try{
		ds = new DatagramSocket();
		DatagramPacket dp = new DatagramPacket(buffer2, buffer2.length, InetAddress.getByName("127.0.0.1"),20011);
		ds.send(dp);

		System.out.println(buffer);
    }catch(Exception e){
		System.out.println(e);
    }

}

}
