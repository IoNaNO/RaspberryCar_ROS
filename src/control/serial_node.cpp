#include<ros/ros.h>
#include<serial/serial.h>
#include<string>
#include "control/Serialmsg.h"

uint8_t st=0xAF;
serial::Serial ser;
void controlCallback(const control::Serialmsg::ConstPtr& msg){
    std::vector<uint8_t> buffer;
    buffer.push_back(st);
    buffer.push_back(msg->type);
    buffer.push_back(msg->data);
        
    ser.write(buffer);

    ROS_INFO("Serial port: %u,Data: %u",msg->type,msg->data);
}

int main(int argc,char** argv){
    ros::init(argc,argv,"serial_node");

    ros::NodeHandle nh;
    ros::Subscriber sub=nh.subscribe("/control_info",10,controlCallback);

    ros::Rate loop_rate(10);

    try{
        ser.setPort("/dev/ttyACM0");
        ser.setBaudrate(115200);
        ser.setParity(serial::parity_none);
        ser.setBytesize(serial::eightbits);
        ser.setStopbits(serial::stopbits_one);
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

    ros::spin();

    ser.close();
    return 0;
}

