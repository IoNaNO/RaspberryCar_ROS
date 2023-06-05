#include<ros/ros.h>
#include<sensor_msgs/Image.h>
#include<cv_bridge/cv_bridge.h>
#include<opencv2/highgui.hpp>
#include"control/Serialmsg.h"

template <typename T>
T clamp(T val, T min, T max) {
    if (val < min) {
        return min;
    } else if (val > max) {
        return max;
    } else {
        return val;
    }
}

int speed=60;
int distance=0;
ros::Subscriber sub;
ros::Publisher pub;
control::Serialmsg sendmsg;

const int th=30;
const int init_speed=60;

bool isedge(const cv::Mat& m,int x,int y,bool dir){
    if(dir==true){
        if(!m.at<uchar>(y,x)){
            // ROS_INFO("left:(%d,%d)",y,x);
            x+=(th/2);
            if(x<m.cols){
                if(!m.at<uchar>(y,x)){
                    x+=(th/2);
                    if(x<m.cols){
                        if(!m.at<uchar>(y,x)){
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
    else{
        if(!m.at<uchar>(y,x)){
            // ROS_INFO("right:(%d,%d)",y,x);
            x-=(th/2);
            if(x>=0){
                if(!m.at<uchar>(y,x)){
                    x-=(th/2);
                    if(x>=0){
                        if(!m.at<uchar>(y,x)){
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
}

void callback(const sensor_msgs::Image::ConstPtr& msg){
    cv::Mat image;
    try{
        image=cv_bridge::toCvShare(msg,"mono8")->image;
    }
    catch(cv_bridge::Exception& e){
        ROS_ERROR("%s",e.what());
        return;
    }
    int d=0;
    //ROS_INFO("pixel:%d",image.at<uchar>(image.rows/2,image.cols/2));
    int l=0,r=0;
    for(int i=0;i<image.rows;i++){
        l=0;
        r=0;
        for(int j=0;j<image.cols;j++){
            if(isedge(image,j,i,true)){
                l=j;
                break;
            }
        }
        for(int j=image.cols-1;j>=0;j--){
            if(isedge(image,j,i,false)){
                r=j;
                break;
            }
        }
        //ROS_INFO("(l:%d,r:%d)",l,r);
        if(l&&r){
            break;
        }
    }
    if(l&&r){
        distance=(l+r-image.cols)/2;
        ROS_INFO("Get the track distance:%d",distance);
        // if(d!=distance){
        //     distance=d;
        //     sendmsg.type=control::Serialmsg::angle;
        //     sendmsg.data=distance;
        //     pub.publish(sendmsg);
        // }
        // speed=init_speed;
        // sendmsg.type=control::Serialmsg::velocity;
        // sendmsg.data=speed;
        // pub.publish(sendmsg);
        sendmsg.type=control::Serialmsg::angle;
        sendmsg.data=clamp(distance, static_cast<int32_t>(std::numeric_limits<int8_t>::min()), static_cast<int32_t>(std::numeric_limits<int8_t>::max()));
        pub.publish(sendmsg);
    }
    else{
        speed=0;
        sendmsg.type=control::Serialmsg::velocity;
        sendmsg.data=clamp(distance, static_cast<int32_t>(std::numeric_limits<int8_t>::min()), static_cast<int32_t>(std::numeric_limits<int8_t>::max()));;
        pub.publish(sendmsg);
    }

}

int main(int argc,char **argv){
    ros::init(argc,argv,"control_node");
    ros::NodeHandle nh;
    pub=nh.advertise<control::Serialmsg>("/control_info",1);
    sub=nh.subscribe("/raspi_cam/image_bin",10,callback);
    ros::Rate loop_rate(10);

    while(ros::ok()){
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
