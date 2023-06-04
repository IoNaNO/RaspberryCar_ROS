#include<ros/ros.h>
#include<sensor_msgs/Image.h>
#include<cv_bridge/cv_bridge.h>
#include<opencv2/highgui.hpp>
#include"control/Serialmsg.h"

int speed=120;
int distance=0;
ros::Subscriber sub;
ros::Publisher pub;
control::Serialmsg sendmsg;

const int th=30;

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
        d=(l+r-image.cols)/2;
        ROS_INFO("Get the track distance:%d",d);
        if(d!=distance){
            distance=d;
            sendmsg.type=control::Serialmsg::angle;
            sendmsg.data=distance;
            pub.publish(sendmsg);
        }
    }

}

int main(int argc,char **argv){
    ros::init(argc,argv,"control_node");
    ros::NodeHandle nh;
    pub=nh.advertise<control::Serialmsg>("/control_info",1);
    sub=nh.subscribe("/raspi_cam/image_bin",10,callback);

    ros::spin();

    return 0;
}
