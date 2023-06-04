# Raspberry ROS Car
## Environment
Ubuntu 20.04
ROS Noetic
## Key command
- WLAN
```bash
sudo wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant.conf
sudo dhclient wlan0
sudo route add default gw 192.168.239.199
```
- HOST
```bash
export ROS_MASTER_URI=http://192.168.239.80:11311
export ROS_IP=192.168.239.80
export ROS_IP=192.168.239.201
```
- BUILD
```bash
catkin build
```
- MONITOR
```bash
rosrun rqt_image_view rqt_image_view
```