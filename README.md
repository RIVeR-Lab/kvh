# KVH 1750 Fibe Optics Gyro IMU Driver
This is the driver for the KVG 1750 Fiber Optic Gyro IMU

### Install Steps
`cd catkin_ws/src`

1. Copy repository
`git clone https://github.com/RIVeR-Lab/kvh.git`

2. Install necessary accompanying packages
`sudo apt-get install ros-melodic-device-driver`

3. Clone necessary accompanying git repos
`git clone https://github.com/RIVeR-Lab/river_util.git`


4. Give user permissions to access the IMU over a serial connnection
```
sudo usermod -a -G tty river
sudo usermod -a -G dialout river
```

5. Roll Back the RIVeR Util to a previous tag
```
git fetch --all --tags
cd river_util
git checkout aero_2014
```

6. Remove unecessary packages
```
cd ..
rm -rf river_util/safety_interface/
rm -rf river_util/robot_teleop/
rm -rf river_util/stereo_image_sync/
```

7. Reboot the computer - this is needed to force the system to pickup the new device permissions
`sudo reboot now`

8. Run launch file
`roslaunch kvh_driver master.launch`
