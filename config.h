#ifndef CONFIG_H
#define CONFIG_H


// These Must be in Milimeters
#define _SSL_FIELD_LENGTH 6050.0 //6100.0 //5000.0
#define _SSL_FIELD_WIDTH 4050.0  //3500.0
#define _SSL_FIELD_RAD 500.0
#define _SSL_FIELD_PENALTY 450.0
#define _SSL_FIELD_MARGIN 1000.0
#define _SSL_WALL_THICKNESS	250.0



// some constants
#define KICKFACTOR 0.2f
#define CHIPFACTOR 0.6f
#define ROLLERTORQUEFACTOR 0.01f
#define KICK_MAX_ANGLE 60.0f
#define MAXWHEELSPEED 5
#define KICKERFMAX 0.2

#define CHASSISLENGTH 0.17	// chassis length
#define CHASSISWIDTH 0.17	// chassis width
#define CHASSISHEIGHT 0.13	// chassis height
#define BOTTOMHEIGHT 0.02 //ertefa 'e kaf
#define KICKERHEIGHT 0.005 //ertefa 'e kicker
#define KLENGTH 0.005	// kicker length
#define KWIDTH 0.08	// kicker width
#define KHEIGHT 0.04	// kicker height

#define WHEELRADIUS 0.035	// wheel radius
#define WHEELLENGTH 0.005
#define STARTZ 0.3	// starting height of chassis
#define CHASSISMASS 2		// chassis mass
#define WHEELMASS 0.2	// wheel mass
#define BALLMASS 0.1	// ball mass
#define KICKERMASS 0.02	// kicker mass
#define BALLRADIUS 0.0215



#define VISION_PORT 13002
#define COMMAND_PORT 13003

#endif // CONFIG_H
