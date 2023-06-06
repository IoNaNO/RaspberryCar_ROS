#include<ros/ros.h>
#include<sensor_msgs/Image.h>
#include<cv_bridge/cv_bridge.h>
#include<opencv2/highgui.hpp>
#include"control/Serialmsg.h"
#include"detection_msgs/FeaturePoint.h"

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
ros::Subscriber detect_sub;
control::Serialmsg sendmsg;

const int th=30;
const int init_speed=60;

const int wid_t=50;
const int hig_t=200;
const int cnt_t=2;
int cnt=0;
int cnt_stall=0;

bool trackmission=true;
control::Serialmsg stopflag;
control::Serialmsg detourflag;
control::Serialmsg noneflag;

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

bool isregion(const int x,const int y){
    return y>=hig_t&&x>=320-wid_t&&x<=320+wid_t;
}

void imgCallback(const sensor_msgs::Image::ConstPtr& msg){
    if(trackmission){
        cv::Mat image;
        try{
            image=cv_bridge::toCvShare(msg,"mono8")->image;
        }
        catch(cv_bridge::Exception& e){
            ROS_ERROR("%s",e.what());
            return;
        }
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
            // ROS_INFO("Get the track distance:%d",distance);
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
}

void detectCallback(const detection_msgs::FeaturePoint::ConstPtr& msg){
    ROS_INFO("Get detection point type:%d x:%d y:%d",msg->Class,msg->x,msg->y);
    if(msg->Class!=msg->none){
        if(isregion(msg->x,msg->y)){
            cnt++;
        }
        if(cnt>=cnt_t){
            if(msg->Class==msg->person){
                pub.publish(stopflag);
            }
            else if(msg->Class==msg->car){
                pub.publish(detourflag);
            }
            // trackmission=false;
        }
        cnt_stall=0;
    }
    else{
        cnt_stall++;
        if(cnt_stall>=cnt_t){
            cnt=0;
            trackmission=true;
            pub.publish(noneflag);
        }
    }
}

int main(int argc,char **argv){
    ros::init(argc,argv,"control_node");
    ros::NodeHandle nh;
    pub=nh.advertise<control::Serialmsg>("/control_info",1);
    sub=nh.subscribe("/raspi_cam/image_bin",10,imgCallback);
    detect_sub=nh.subscribe("/yolov5/detections",10,detectCallback);
    ros::Rate loop_rate(10);

    stopflag.type=control::Serialmsg::detect;
    stopflag.data=detection_msgs::FeaturePoint::person;

    detourflag.type=control::Serialmsg::detect;
    detourflag.data=detection_msgs::FeaturePoint::car;

    noneflag.type=control::Serialmsg::detect;
    noneflag.data=detection_msgs::FeaturePoint::none;

    while(ros::ok()){
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
