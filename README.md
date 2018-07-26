# Sonar-Point-Cloud-Viewer
handheld system that employs ultrasonic rangefinder and 9-degree freedom IMU sensors to map the surroundings.
Platform used: BeagleBone Black, NXP FRDM KL25Z with HCSR04 ultrasonic sensor and MPU9250 IMU sensor.

The system uses an ultrasonic rangefinder (HCSR04) along with a 9-degree freedom MARG sensor to determine the locations of objects and to visualize this data on a point cloud viewer. The approach used in the project is as described below:
Upon start up the BeagleBoneBlack triggers the IMU sensor to find the attitude and heading of the system. After finding the system's heading the BBB sends a trigger via UART to FRDM KL25Z which interfaces with the Ultrasonic rangefinder sensor. 
Upon gathering information of the analog echo signal for 25ms, the KL25Z sends out the ADC output of the analog signals back to BBB via UART. In BBB the ADC values are analyzed to extract the distance of multiple objects from the ultrasound sensor.
The process is repeated whenever there is a change in the heading and attitude as detected by the IMU sensor. 
After moving the system through the area we want to map, the obtained distances are converted to points on the x,y,z plane using the heading information. All these values are written to a file in the XYZ format which is rendered on the online LIDAR point cloud viewer to get the point cloud image of the scanned area.
