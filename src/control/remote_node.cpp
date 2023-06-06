#include<ros/ros.h>
#include<termio.h>
#include<unistd.h>
#include"control/Serialmsg.h"
#include"detection_msgs/FeaturePoint.h"

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
#define KEYCODE_SPACE 0x20

detection_msgs::FeaturePoint personsignal;
detection_msgs::FeaturePoint carsignal;
detection_msgs::FeaturePoint nonesignal;

int getch()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    //ROS_INFO("Read From Keyboard: %d",ch);
    return ch;
}

int main(int argc,char** argv){
    ros::init(argc,argv,"remote_node");
    ros::NodeHandle nh;
    ros::Publisher pub=nh.advertise<control::Serialmsg>("/control_info",1);
    ros::Publisher yolo_mock=nh.advertise<detection_msgs::FeaturePoint>("/yolov5/detections",1);

    personsignal.Class=detection_msgs::FeaturePoint::person;
    personsignal.x=320;
    personsignal.y=300;

    carsignal.Class=detection_msgs::FeaturePoint::car;
    carsignal.x=320;
    carsignal.y=300;

    nonesignal.Class=detection_msgs::FeaturePoint::none;
    nonesignal.x=0;
    nonesignal.y=0;

    ros::Rate loop_rate(30);

    control::Serialmsg msg;
    int speed=0;
    int angle=0;
    int stop=0;
    int detour=0;
    int clear=0;

    ROS_INFO("Use arrow keys to control the vehicle. Press 'q' to quit.");

    while(ros::ok()){
        int key=0;
        if(!stop&&!detour&&!clear){
            key=getch();
        }
        bool update=false;
        if(key=='q'){
            break;
        }
        ROS_INFO("key now:%d",key);
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
            msg.data=angle;
            update=true;            
        }
        else if(key==KEYCODE_LEFT){
            angle--;
            msg.type=msg.angle;
            msg.data=angle;
            update=true;
        }
        else if(key==KEYCODE_SPACE){
            stop=10;
        }
        else if(key==KEYCODE_C){
            detour=10;
        }
        else if(key==KEYCODE_B){
            clear=10;
        }

        if(update){
            pub.publish(msg);
            ROS_INFO("Sending Type: %u,Data: %d",msg.type,msg.data);
        }
        if(stop){
            stop--;
            yolo_mock.publish(personsignal);
            ROS_INFO("Sending Stop!");
        }
        else if(detour){
            detour--;
            yolo_mock.publish(carsignal);
            ROS_INFO("Sending Detour!");
        }
        else if(clear){
            clear--;
            yolo_mock.publish(nonesignal);
            ROS_INFO("Sending Clear!");
        }
        loop_rate.sleep();
    }
    
    return 0;
}