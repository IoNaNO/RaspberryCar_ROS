#include<ros/ros.h>
#include<serial/serial.h>
#include<string>
#include "control/Serialmsg.h"

int main(int argc,char** argv){
    ros::init(argc,argv,"serial_node");

    ros::NodeHandle n;
    ros::Publisher ser_pub=n.advertise<control::Serialmsg>("/contro_info",1000);

    ros::Rate loop_rate(10);
    serial::Serial ser;

    try{
        ser.setPort("/dev/ttyACM0");
        ser.setBaudrate(9600);
        serial::Timeout tout=serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(tout);
        ser.open();
    }
    catch(serial::IOException&e){
        ROS_ERROR_STREAM("Unable to open port ");
        return -1;
    }
    if (ser.isOpen())
    {
        ROS_INFO_STREAM("Serial Port initialized");
    }
    else
    {
        return -1;
    }

    char t='G';
    std::string test="Hello";
    while(ros::ok()){
        control::Serialmsg send_msg;
        send_msg.type=1;
        send_msg.data=100;
        ser_pub.publish(send_msg);
        // ser.write(test);
        ser.write((uint8_t*)&t,1);
        ROS_INFO_STREAM("MESSAGE SEND:"<<t);
        ros::spinOnce();
        loop_rate.sleep();
    }
}

