# Raspberry ROS Car
## Environment
Ubuntu 20.04
ROS Noetic
## Key command
- WLAN
```bash
sudo wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant.conf
sudo dhclient wlan0
```
- HOST
```bash
export ROS_MASTER_URI=http://192.168.239.80:11311
export ROS_IP=$(hostname -I)
```