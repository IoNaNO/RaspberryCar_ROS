#!/usr/bin/env python3
import rospy
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
import cv2
import numpy as np

class ImageProcessor:
    def __init__(self):
        image_input_topic='/raspi_cam/image_raw'
        image_output_topic='/raspi_cam/image_bin'
        bev_output_topic='/raspi_cam/image_bev'
        self.sub=rospy.Subscriber(image_input_topic,Image,self.callback)
        self.pub=rospy.Publisher(image_output_topic,Image,queue_size=10)
        self.pub_bev=rospy.Publisher(bev_output_topic,Image,queue_size=10)
        self.H=np.load('/home/ubuntu/RaspberryCar_ROS/src/perception/src/homography.npy')
        # self.kernel=np.ones((30,30),np.uint8)
    def callback(self,msg):
        try:
            image=CvBridge().imgmsg_to_cv2(msg,"bgr8")
        except CvBridgeError as e:
            rospy.logerr(e)
        gray_image=cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)
        bev_image_size=(400,250)
        bev_image=cv2.warpPerspective(gray_image,self.H,bev_image_size)
        bev_image=cv2.GaussianBlur(bev_image,(5,5),0)
        _,binary_image=cv2.threshold(bev_image,0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
        # binary_image=cv2.morphologyEx(binary_image,cv2.MORPH_OPEN,self.kernel)
        # rospy.loginfo(self.H.shape)
        try:
            ros_image=CvBridge().cv2_to_imgmsg(binary_image,encoding="mono8")
            ros_bev=CvBridge().cv2_to_imgmsg(bev_image,encoding="mono8")
            self.pub.publish(ros_image)
            self.pub_bev.publish(ros_bev)
        except CvBridgeError as e:
            rospy.logerr(e)
            return    


def main():
    rospy.init_node('image_process')
    ins=ImageProcessor()
    rospy.spin()

if __name__=='__main__':
    main()