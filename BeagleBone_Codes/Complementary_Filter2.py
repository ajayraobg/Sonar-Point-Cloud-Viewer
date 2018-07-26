'''
    Complementary_Filter2.py
    
    Description: Implementation of the complementary filter for attitude estimation based on the journal paper
    
    "Gain-Scheduled Complementary Filter Design for a MEMS Based Attitude and Heading Reference System"
    Tae Suk Yoo, Sung Kyung Hong, Hyok Min Yoon, and Sungsu Park
    Sensors, vol. 11, no. 4, 2011
    
    Revision History
    21 Mar 2016 - Created and debugged
    04 Apr 2016 - Added magnetometer readings
    28 Apr 2016 - Debugged magnetomer code    

    Author: Lars Soltmann
    
    INPUTS:     ax,ay,az    - Accelerometer components [g-force]
                gx,gy,gz    - Gyroscope components [deg/s]
                mx,my,mz    - Magnetometer components [uT] - For attitude3() only
                hix,hiy,hiz - Mangetometer hard iron offests - Required for attitude3()
    
    OUTPUTS:
                roll_d      - Roll angle [deg]
                pitch_d     - Pitch angle [deg]
                yaw_d       - Yaw angle [deg]
                roll_r      - Roll angle [rad]
                pitch_r     - Pitch angle [rad]
                yaw_r       - Yaw angle [rad]
                phid_d*     - Roll rate [deg/s] - inertial frame, phi_dot
                thetad_d*   - Pitch rate [deg/s] - inertial frame, theta_dot
                psid_d*     - Yaw rate [deg/s] - inertial frame, psi_dot
    
    NOTES:
    - Written for python3
    - Accelerations and rates follow aircraft body-axis coordinate system, i.e. 'x' out the nose, 'y' out the right wing, 'z' out the the bottom
    - Right hand rule used for rates
    - Because filter is based on Euler angles, filter fails and requires a reset if roll or pitch exceeds 90deg
    - *=unfiltered
    
    '''


import math
import time

class comp_filt:
    def __init__(self,hi_x=0,hi_y=0,hi_z=0):
        self.reset()
        self.hix=hi_x
        self.hiy=hi_y
        self.hiz=hi_z
    
    def reset(self):
        self.iterm_pitch=0
        self.iterm_roll=0
        self.iterm_yaw=0
        self.previous_time=0
        self.first_time=1

    ########## ROLL AND PITCH ONLY ##########
    def attitude2(self,ax,ay,az,gx,gy,gz):
        # Calculate time increment and save for next iteration
        if self.first_time==1:
            dt=0
            self.first_time=0
            self.previous_time=time.time()
            # Use accelerometer angles as initial angles
            self.pitch=math.atan2(ax,math.sqrt(math.pow(ay,2)+math.pow(az,2)))
            self.roll=-math.atan2(ay,math.sqrt(math.pow(ax,2)+math.pow(az,2)))
        else:
            t1=time.time()
            dt=t1-self.previous_time
            self.previous_time=t1
       
        # Schedule gains - based on total acceleration
        # ___See journal paper for details
        # kp = 2*zeta*omega      where omega is the cut-off frequency, zeta is the damping ratio
        # ki = omega^2
        # zeta here is fixed at 0.707
        # kp = sqrt(2)*omega
        accel_mag=math.fabs(math.sqrt(math.pow(ax,2)+math.pow(ay,2)+math.pow(az,2))-1)
        if accel_mag<0.015: # omega_pitch=omega_roll=0.1
            kp_pitch=0.1414
            ki_pitch=0.01
            kp_roll=0.1414
            ki_roll=0.01
        elif (accel_mag>=0.015 and accel_mag<5): # omega_pitch=0.01 omega_roll=0.05
            kp_pitch=0.01414
            ki_pitch=0.0001
            kp_roll=0.0707
            ki_roll=0.0025
        elif accel_mag>=5: # omega_pitch=omega_roll=0
            kp_pitch=0
            ki_pitch=0
            kp_roll=0
            ki_roll=0
        
        # Euler angles based on accelerometers, rad
        pitch_a=math.atan2(ax,math.sqrt(math.pow(ay,2)+math.pow(az,2))) 
        roll_a=-math.atan2(ay,math.sqrt(math.pow(ax,2)+math.pow(az,2))) 
              
        # Rate of change of Euler angles based on gyroscopes, rad
        pitch_dot_g=math.radians(gy*math.cos(self.roll)-gz*math.sin(self.roll))
        roll_dot_g=math.radians(gx+math.tan(self.pitch)*(gy*math.sin(self.roll)+gz*math.cos(self.roll)))
        
        # Error in angles based on accelerometer, rad
        error_pitch=self.pitch-pitch_a 
        error_roll=self.roll-roll_a   
        
        # Multiply error by kp gain
        p_term_pitch=kp_pitch*error_pitch
        p_term_roll=kp_roll*error_roll

        # Multiply by ki gain and integrate
        self.iterm_pitch=self.iterm_pitch+ki_pitch*error_pitch*dt
        self.iterm_roll=self.iterm_roll+ki_roll*error_roll*dt
        
        # Add gyro, p_term, i_term and integrate
        int_term_pitch=pitch_dot_g-p_term_pitch-self.iterm_pitch
        int_term_roll=roll_dot_g-p_term_roll-self.iterm_roll
        self.pitch=self.pitch+int_term_pitch*dt
        self.roll=self.roll+int_term_roll*dt

        # Save pitch and roll for export in both radians and  degrees
        self.pitch_d=math.degrees(self.pitch)#deg
        self.roll_d=math.degrees(self.roll)  #deg
        self.pitch_r=self.pitch              #rad
        self.roll_r=self.roll                #rad
        self.thetad_d=math.degrees(pitch_dot_g)#deg
        self.phid_d=math.degrees(roll_dot_g) #deg


    ########## ROLL, PITCH, YAW ##########
    def attitude3(self,ax,ay,az,gx,gy,gz,mx,my,mz):
        # Remove hard-iron offsets
        mx=mx-self.hix
        my=my-self.hiy
        mz=mz-self.hiz

        # Calculate time increment and save for next iteration
        if self.first_time==1:
            dt=0
            self.first_time=0
            self.previous_time=time.time()
            # Use accelerometer and magnetometer angles as initial angles
            self.pitch=math.atan2(ax,math.sqrt(math.pow(ay,2)+math.pow(az,2)))
            self.roll=-math.atan2(ay,math.sqrt(math.pow(ax,2)+math.pow(az,2)))
            xh=mx*math.cos(self.pitch)+my*math.sin(self.pitch)*math.sin(self.roll)+mz*math.sin(self.pitch)*math.cos(self.roll)
            yh=-my*math.cos(self.roll)+mz*math.sin(self.roll)
            self.yaw=math.atan2(yh,xh)
            # Map atan2 results to 0-2*Pi
            if self.yaw<0:             
                self.yaw=self.yaw+2*math.pi 

        else:
            t1=time.time()
            dt=t1-self.previous_time
            self.previous_time=t1
       
        # Schedule gains - based on total acceleration
        # ___See journal paper for details
        # kp = 2*zeta*omega      where omega is the cut-off frequency, zeta is the damping ratio
        # ki = omega^2
        # zeta here is fixed at 0.707
        # kp = sqrt(2)*omega
        accel_mag=math.fabs(math.sqrt(math.pow(ax,2)+math.pow(ay,2)+math.pow(az,2))-1)
        if accel_mag<0.015: # omega_pitch=omega_roll=0.1
            kp_pitch=0.1414
            ki_pitch=0.01
            kp_roll=0.1414
            ki_roll=0.01
        elif (accel_mag>=0.015 and accel_mag<5): # omega_pitch=0.01 omega_roll=0.05
            kp_pitch=0.01414
            ki_pitch=0.0001
            kp_roll=0.0707
            ki_roll=0.0025
        elif accel_mag>=5: # omega_pitch=omega_roll=0
            kp_pitch=0
            ki_pitch=0
            kp_roll=0
            ki_roll=0
        
        # Gain for yaw axis is fixed at 0.1rad/s
        kp_yaw=0.1414
        ki_yaw=0.01
        
        # Euler angles based on accelerometers, rad
        pitch_a=math.atan2(ax,math.sqrt(math.pow(ay,2)+math.pow(az,2))) 
        roll_a=-math.atan2(ay,math.sqrt(math.pow(ax,2)+math.pow(az,2)))
        
        #Yaw angle based on mangetometer, rad
        xh=mx*math.cos(self.pitch)+my*math.sin(self.pitch)*math.sin(self.roll)+mz*math.sin(self.pitch)*math.cos(self.roll)
        yh=-my*math.cos(self.roll)+mz*math.sin(self.roll)
        yaw_m=math.atan2(yh,xh)
        # Map atan2 results to 0-2*Pi
        if yaw_m<0:             
            yaw_m=yaw_m+2*math.pi   
        
        # Rate of change of Euler angles based on gyroscopes, rad
        pitch_dot_g=math.radians(gy*math.cos(self.roll)-gz*math.sin(self.roll))
        roll_dot_g=math.radians(gx+math.tan(self.pitch)*(gy*math.sin(self.roll)+gz*math.cos(self.roll)))
        yaw_dot_g=math.radians(gy*math.sin(self.roll)/math.cos(self.pitch)+gz*math.cos(self.roll)/math.cos(self.pitch))
        
        # Error in angles based on accelerometer, rad
        error_pitch=self.pitch-pitch_a 
        error_roll=self.roll-roll_a

        # Error in yaw angle based on magnetometer       
        error_yaw=self.yaw-yaw_m

        # A problem arises here because of the switch from 2*Pi to 0 or vice versa, this causes 
        # the error to go to either 2*Pi (if approached 'clockwise') or -2*Pi (if approached 
        # 'counter-clockwise').  In this case a correction to the error
        # must be made to account for the 'circular' difference, not the absolute difference. 
        
        # Arbitrarly selected 1.5*Pi just incase psi_dot is farily large when crossing 180deg
        if error_yaw > 1.5*math.pi:
            error_yaw=error_yaw-2*math.pi
        elif error_yaw < -1.5*math.pi:
            error_yaw=error_yaw+2*math.pi       

       # Multiply error by kp gain
        p_term_pitch=kp_pitch*error_pitch
        p_term_roll=kp_roll*error_roll
        p_term_yaw=kp_yaw*error_yaw

        # Multiply by ki gain and integrate
        self.iterm_pitch=self.iterm_pitch+ki_pitch*error_pitch*dt
        self.iterm_roll=self.iterm_roll+ki_roll*error_roll*dt
        self.iterm_yaw=self.iterm_yaw+ki_yaw*error_yaw*dt
        
        # Add gyro, p_term, i_term and integrate
        int_term_pitch=pitch_dot_g-p_term_pitch-self.iterm_pitch
        int_term_roll=roll_dot_g-p_term_roll-self.iterm_roll
        int_term_yaw=yaw_dot_g-p_term_yaw-self.iterm_yaw

        self.pitch=self.pitch+int_term_pitch*dt
        self.roll=self.roll+int_term_roll*dt
        self.yaw=self.yaw+int_term_yaw*dt
        if self.yaw > 2*math.pi:
            self.yaw=self.yaw-2*math.pi
        elif self.yaw < 0:
            self.yaw=self.yaw+2*math.pi

        # Save pitch, roll, and yaw data for export in both radians and  degrees
        self.pitch_d=math.degrees(self.pitch)#deg
        self.roll_d=math.degrees(self.roll)  #deg
        self.yaw_d=math.degrees(self.yaw)    #deg
        self.pitch_r=self.pitch              #rad
        self.roll_r=self.roll                #rad
        self.yaw_r=self.yaw                  #rad
        self.thetad_d=math.degrees(pitch_dot_g)#deg
        self.phid_d=math.degrees(roll_dot_g) #deg
        self.psid_d=math.degrees(yaw_dot_g)  #deg
