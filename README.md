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