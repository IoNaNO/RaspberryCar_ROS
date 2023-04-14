#include<ros/ros.h>
#include<serial/serial.h>
#include<string>
#include "control/Serialmsg.h"

int main(int argc,char** argv){
    ros::init(argc,argv,"serial_node");

    ros::NodeHandle nh;
    ros::Publisher ser_pub=nh.advertise<control::Serialmsg>("/contro_info",1000);

    ros::Rate loop_rate(10);
    serial::Serial ser;

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

    std::string test="Hello ROS\n";
    while(ros::ok()){
        control::Serialmsg send_msg;
        send_msg.type=send_msg.empty;
        send_msg.type=send_msg.velocity;
        send_msg.data=64;
        ser_pub.publish(send_msg);

        std::vector<uint8_t> buffer;
        buffer.push_back(send_msg.type);
        buffer.push_back(send_msg.data);
        
        ser.write(buffer);

        ROS_INFO_STREAM("Sending Type: "<<(unsigned int)send_msg.type<<", Data: "<<(unsigned int)send_msg.data);
        ros::spinOnce();
        loop_rate.sleep();
    }

    ser.close();
    return 0;
}

