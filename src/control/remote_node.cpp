#include<ros/ros.h>
#include<termio.h>
#include<unistd.h>
#include"control/Serialmsg.h"

#define KEYCODE_RIGHT 0x43
#define KEYCODE_LEFT 0x44
#define KEYCODE_UP 0x41
#define KEYCODE_DOWN 0x42
#define KEYCODE_B 0x62
#define KEYCODE_C 0x63
#define KEYCODE_D 0x64
#define KEYCODE_E 0x65
#define KEYCODE_F 0x66
#define KEYCODE_G 0x67
#define KEYCODE_Q 0x71
#define KEYCODE_R 0x72
#define KEYCODE_T 0x74
#define KEYCODE_V 0x76

int getch(){
    termios oldt,newt;
    int ch;
    tcgetattr(STDIN_FILENO,&oldt);
    newt=oldt;
    newt.c_lflag&=-(ICANON|ECHO);
    tcsetattr(STDIN_FILENO,TCSANOW,&newt);
    ch=getchar();
    tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
    return ch;
}

int main(int argc,char** argv){
    ros::init(argc,argv,"remote_node");
    ros::NodeHandle nh;
    ros::Publisher pub=nh.advertise<control::Serialmsg>("/control_info",1);

    control::Serialmsg msg;
    int speed=0;
    int angle=0;

    ROS_INFO("Use arrow keys to control the vehicle. Press 'q' to quit.");

    while(ros::ok()){
        int key=getch();
        bool update=false;
        if(key=='q'){
            break;
        }

        if(key==KEYCODE_UP){
            speed++;
            msg.type=msg.velocity;
            msg.data=speed;
            update=true;
        }
        else if(key==KEYCODE_DOWN){
            speed--;
            msg.type=msg.velocity;
            msg.data=speed;
            update=true;            
        }
        else if(key==KEYCODE_RIGHT){
            angle++;
            msg.type=msg.angle;
            msg.data=speed;
            update=true;            
        }
        else if(key==KEYCODE_LEFT){
            angle--;
            msg.type=msg.angle;
            msg.data=speed;
            update=true;
        }
    }
}