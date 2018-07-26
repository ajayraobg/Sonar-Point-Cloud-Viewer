import Adafruit_BBIO.UART as UART
import serial
import ctypes
import time
from binascii import hexlify
import re
import datetime
import copy
import sys
import os
import math
#from Myfilter import  Fusion
from Complementary_Filter2 import comp_filt
class Inp(ctypes.Structure):
   _fields_ = [('In1', ctypes.c_float),
               ('In2', ctypes.c_float)]

class Out(ctypes.Structure):
   _fields_ = [('Out1', ctypes.c_float)]

os.system("config-pin p9_11 uart")
os.system("config-pin p9_13 uart")
UART.setup("UART4")
IMU_Values = []

myLib = ctypes.CDLL('./IMU.so')

imu_val_gen = myLib.imu_val_gen



ser = serial.Serial(port = "/dev/ttyO4", baudrate=115200, timeout =1)
ser.close()
ser.open()



#Roll_pitc_yaw = IMU.imu_val_gen()
#roll = IMU.get_roll(0, 0, 0, 0)
#print(roll)
Error_threshold =10
temp_roll =0.0
temp_pitch = 0.0
temp_yaw =0.0
pattern = re.compile("^\s+|\s*,\s*|\s+$")
File_Write_Flag = True
#fusion_obj = Fusion()
new_filter_obj = comp_filt()
try:
        while True:
            ultrasonic_ADC_list = []
            recieved_data = []
            if ser.isOpen():
                  #Trigger_miss_flag = 1
                  imu_val_gen()
                  acc_x_axis_t = Out.in_dll(myLib, 'ax')
                  acc_x_axis = acc_x_axis_t.Out1
                  acc_y_axis_t = Out.in_dll(myLib, 'ay')
                  acc_y_axis = acc_y_axis_t.Out1
                  acc_z_axis_t = Out.in_dll(myLib, 'az')
                  acc_z_axis = acc_z_axis_t.Out1
                  #print("acc x axis")
                  #print(acc_x_axis)
                  #print("acc y axis")
                  #print(acc_y_axis)
                  #print("acc z axis")
                  #print(acc_z_axis)
                  gyro_x_axis_t = Out.in_dll(myLib, 'gx')
                  gyro_x_axis = gyro_x_axis_t.Out1
                  gyro_y_axis_t = Out.in_dll(myLib, 'gy')
                  gyro_y_axis = gyro_y_axis_t.Out1
                  gyro_z_axis_t = Out.in_dll(myLib, 'gz')
                  gyro_z_axis = gyro_z_axis_t.Out1
                  #print("gyro x axis")
                  #print(gyro_x_axis)
                  #print("gyro y axis")
                  #print(gyro_y_axis)
                  #print("gyro z axis")
                  #print(gyro_z_axis)
                  magneto_x_axis_t = Out.in_dll(myLib, 'mx')
                  magneto_x_axis = magneto_x_axis_t.Out1
                  magneto_y_axis_t = Out.in_dll(myLib, 'my')
                  magneto_y_axis = magneto_y_axis_t.Out1
                  magneto_z_axis_t = Out.in_dll(myLib, 'mz')
                  magneto_z_axis = magneto_z_axis_t.Out1
                  #print("magneto x axis")
                  #print(magneto_x_axis)
                  #print("magneto y axis")
                  #print(magneto_y_axis)
                  #print("magneto z axis")
                  #print(magneto_z_axis)
                  acc_tuple = (acc_x_axis,acc_y_axis,acc_z_axis)
                  gyro_tuple = (gyro_x_axis,gyro_y_axis,gyro_z_axis)
                  magneto_tuple = (magneto_x_axis,magneto_y_axis_t,magneto_z_axis_t)
                  #fusion_obj.update(acc_tuple,gyro_tuple,magneto_tuple)
                  #fusion_obj.update_nomag(acc_tuple,gyro_tuple)
                  new_filter_obj.attitude3(acc_x_axis,acc_y_axis,acc_z_axis,gyro_x_axis,gyro_y_axis,gyro_z_axis,magneto_x_axis,magneto_y_axis,magneto_z_axis)
                  roll = 20#fusion_obj.roll #has to be calculated
                  pitch =20#fusion_obj.pitch
                  yaw = 20
                  thetha = new_filter_obj.thetad_d
                  phi = new_filter_obj.phid_d
                  print("\ntheta Value:")
                  print(thetha)
                  print("\nphi Value:")
                  print(phi)
                  #print(yaw)
                  #if((roll > (temp_roll-10) and roll < (temp_roll+10) and
                  #   pitch > (temp_pitch-10) and pitch < (temp_pitch+10) and yaw > (temp_yaw-10) and yaw < (temp_yaw+10) )):
                  #   temp_roll = roll
                  #   temp_pitch = pitch
                  #   temp_yaw = yaw
                  #   print("not triggering")
                  #else:
                  trigger_count =0
                  temp_roll = roll + 20 # have to change here
                  temp_pitch = pitch + 20
                  temp_yaw = yaw + 20
                  #while(Trigger_miss_flag < 2):
                                         
                  single_ADC_list = []
                  Added_ADC_list = []
                  #ser.flushInput()
                  ser.flushInput()
                  #print ("Serial is open!")
                  ser_tem = "a"
                  #print("hi1")
                  ser.write(ser_tem)
                  #print("hi2")
                  recieved_data.append(ser.read(1024))
                  #print("hi3")
                  
                  dummy_val = copy.copy(recieved_data[0])
                  #print(dummy_val)
                  dummy_tester_list = []
                  dummy_tester_list.append([x for x in pattern.split(dummy_val) if x])
                  try: 
                        ultrasonic_ADC_list.append([x for x in pattern.split(dummy_val) if x])
                        ultrasonic_ADC_list = copy.copy(ultrasonic_ADC_list[0])
                        i = 0
                        while i < len(ultrasonic_ADC_list)-1:
                            single_ADC_list.append(ultrasonic_ADC_list[i])
                            Added_ADC_list.append(ultrasonic_ADC_list[i+1])
                            i = i+2
                  except IndexError:
                        pass
                  #print(len(ultrasonic_ADC_list))
                  #print(Added_ADC_list)

                  Full_category_1 =[]
                  Full_category_2 =[]
                  Full_category_3 =[]
                  Full_category_4 =[]
                  single_ADC_category_1 =[]
                  single_ADC_category_2 =[]
                  single_ADC_category_3 =[]
                  single_ADC_category_4 =[]
                  Added_ADC_category_1 =[]
                  Added_ADC_category_2 =[]
                  Added_ADC_category_3 =[]
                  Added_ADC_category_4 =[]
                  obj_distance_category_1 =[]
                  obj_distance_category_2 =[]
                  obj_distance_category_3 =[]
                  obj_distance_category_4 =[]
                  obj_time_category_1 =0
                  obj_time_category_2 =0
                  obj_time_category_3 =0
                  obj_time_category_4 =0
                  i = 0
                  try:
                        while i < len(ultrasonic_ADC_list)-1:
                            if(i < 90):
                                Full_category_1.append(int(ultrasonic_ADC_list[i]))
                            elif(i < 180 and i > 89):
                                Full_category_2.append(int(ultrasonic_ADC_list[i]))
                            elif(i < 270 and i > 179):
                                Full_category_3.append(int(ultrasonic_ADC_list[i]))
                            elif(i < 360 and i > 269):
                                Full_category_4.append(int(ultrasonic_ADC_list[i]))
                            i = i+1
                  except IndexError:
                        pass
                  #print(len(Full_category_1))
                  #print(len(Full_category_2))
                  #print(len(Full_category_3))
                  #print(len(Full_category_4))
                  #print(Full_category_1)
                  #print(Full_category_2)
                  #print(Full_category_3)
                  #print(Full_category_4)
                  i=0
                  try:
                        while i < len(Full_category_1)-1:
                            single_ADC_category_1.append(Full_category_1[i])
                            Added_ADC_category_1.append(Full_category_1[i+1])
                            single_ADC_category_2.append(Full_category_2[i])
                            Added_ADC_category_2.append(Full_category_2[i+1])
                            single_ADC_category_3.append(Full_category_3[i])
                            Added_ADC_category_3.append(Full_category_3[i+1])
                            single_ADC_category_4.append(Full_category_4[i])
                            Added_ADC_category_4.append(Full_category_4[i+1])
                            i = i+2
                  except IndexError:
                        pass
                  #print("single_ADC list")
                  #print(len(single_ADC_category_1))
                  #print(len(single_ADC_category_2))
                  #print(len(single_ADC_category_3))
                  #print(len(single_ADC_category_4))
                  #print(single_ADC_category_1)
                  #print(single_ADC_category_2)
                  #print(single_ADC_category_3)
                  #print(single_ADC_category_4)
                  #print("Added_ADC_category")
                  #print(len(Added_ADC_category_1))
                  #print(len(Added_ADC_category_2))
                  #print(len(Added_ADC_category_3))
                  #print(len(Added_ADC_category_4))
                  #print(Added_ADC_category_1)
                  #print(Added_ADC_category_2)
                  #print(Added_ADC_category_3)
                  #print(Added_ADC_category_4)
                  Category_1_Threshold = max(single_ADC_category_1)
                  Category_2_Threshold = max(single_ADC_category_2)
                  Category_3_Threshold = max(single_ADC_category_3)
                  Category_4_Threshold = max(single_ADC_category_4)
                  #print(Category_1_Threshold)
                  #print(Category_2_Threshold)
                  #print(Category_3_Threshold)
                  #print(Category_4_Threshold)
                  Trigger_Fall_Detect_Flag = True
                  Timer_count_Added_4 = 0
                  try:
                        Temp_threshold_Category_1 =0
                        Added_High_Category_1 =0
                        if( Category_1_Threshold > 100):
                            Temp_threshold_Category_1 = Category_1_Threshold - 75
                            Added_High_Category_1 = Temp_threshold_Category_1 * 4
                        elif(Category_1_Threshold > 75 and Category_1_Threshold < 100):
                            Temp_threshold_Category_1 = Category_1_Threshold - 60
                            Added_High_Category_1 = Temp_threshold_Category_1 * 4
                        elif(Category_1_Threshold < 50 and Category_1_Threshold > 30):
                            Temp_threshold_Category_1 = Category_1_Threshold - 10
                            Added_High_Category_1 = Temp_threshold_Category_1 * 4
                        elif(Category_1_Threshold < 76 and Category_1_Threshold > 49):
                            Temp_threshold_Category_1 = Category_1_Threshold - 30
                            Added_High_Category_1 = Temp_threshold_Category_1 * 4
                        else:
                            Added_High_Category_1 =Temp_threshold_Category_1 * 4
                        for i in range(len(Added_ADC_category_1)):
                            if(Trigger_Fall_Detect_Flag == True):
                                Timer_count_Added_4 = Timer_count_Added_4+1
                                if((Added_ADC_category_1[i] - Added_ADC_category_1[i+1]) > 90):# Trigger detection Part
                                    Trigger_Fall_Detect_Flag = False
                            else:
                                Timer_count_Added_4 = Timer_count_Added_4+1
                                if(Added_ADC_category_1[i] > Added_High_Category_1):
                                    if(Added_ADC_category_1[i-1] > Added_High_Category_1):
                                        pass
                                    else:
                                        obj_time_category_1 = Timer_count_Added_4 * 25 *4 # Counter 25us and multiple of 4(as 4 values are added)
                                        distance = (obj_time_category_1 * 330)/(2*10000)
                                        obj_distance_category_1.append(distance)
                  except IndexError:
                        pass
                  try:
                        Temp_threshold_Category_2 =0
                        Added_High_Category_2 =0
                        if(Category_2_Threshold > 15 and Category_2_Threshold < 35):
                            Temp_threshold_Category_2 = Category_2_Threshold - 2
                            Added_High_Category_2 = Temp_threshold_Category_2 * 3
                        elif( Category_2_Threshold < 15  or Category_2_Threshold > 35):
                            Added_High_Category_2 = Category_2_Threshold * 2
                        #print(Category_2_Threshold)
                        #print(Added_High_Category_2)    
                            
                        for i in range(len(Added_ADC_category_2)):
                                Timer_count_Added_4 = Timer_count_Added_4+1
                                if(Added_ADC_category_2[i] > Added_High_Category_2):
                                    if(Added_ADC_category_2[i-1] > Added_High_Category_2):
                                        pass
                                    else:
                                        obj_time_category_2 = Timer_count_Added_4 * 25 *4 # Counter 25us and multiple of 4(as 4 values are added)
                                        distance = (obj_time_category_2 * 330)/(2*10000)
                                        obj_distance_category_2.append(distance)
                  except IndexError:
                        pass
                  try:
                        Temp_threshold_Category_3 =0
                        Added_High_Category_3 =0
                        if(Category_3_Threshold > 15 and Category_3_Threshold < 35):
                            Temp_threshold_Category_3 = Category_3_Threshold - 6
                            Added_High_Category_3 = Temp_threshold_Category_3 * 3
                        elif( Category_3_Threshold < 15 ):
                            Added_High_Category_3 = Category_3_Threshold * 3
                        elif( Category_3_Threshold > 35):
                            Added_High_Category_3 = Category_3_Threshold * 2
                        #print(Category_3_Threshold)
                        #print(Added_High_Category_3)    
                            
                        for i in range(len(Added_ADC_category_3)):
                                Timer_count_Added_4 = Timer_count_Added_4+1
                                if(Added_ADC_category_3[i] > Added_High_Category_3):
                                    if(Added_ADC_category_3[i-1] > Added_High_Category_3):
                                        pass
                                    else:
                                        obj_time_category_3 = Timer_count_Added_4 * 25 *4 # Counter 25us and multiple of 4(as 4 values are added)
                                        distance = (obj_time_category_3 * 330)/(2*10000)
                                        obj_distance_category_3.append(distance)
                  except IndexError:
                        pass
                  '''
                  Temp_threshold_Category_4 =0
                  Added_High_Category_4 =0
                  if(Category_4_Threshold > 15 and Category_4_Threshold < 35):
                      Temp_threshold_Category_4 = Category_4_Threshold 
                      Added_High_Category_4 = Temp_threshold_Category_4 * 3
                  elif( Category_4_Threshold < 15 ):
                      Added_High_Category_4 = Category_4_Threshold * 3
                  elif( Category_4_Threshold > 35):
                      Added_High_Category_4 = Category_4_Threshold * 2
                  print(Category_4_Threshold)
                  print(Added_High_Category_4)    
                      
                  for i in range(len(Added_ADC_category_4)):
                          Timer_count_Added_4 = Timer_count_Added_4+1
                          if(Added_ADC_category_4[i] > Added_High_Category_4):
                              if(Added_ADC_category_4[i-1] > Added_High_Category_4):
                                  pass
                              else:
                                  obj_time_category_4 = Timer_count_Added_4 * 25 *4 # Counter 25us and multiple of 4(as 4 values are added)
                                  distance = (obj_time_category_4 * 330)/(2*10000)
                                  obj_distance_category_4.append(distance)
                   
                  print(Timer_count_Added_4)
                  print(obj_distance_category_4)
                  '''
                  ultrasonic_distance = []
                  for distance in obj_distance_category_1:
                     ultrasonic_distance.append(distance)
                  for distance in obj_distance_category_2:
                     ultrasonic_distance.append(distance)
                  for distance in obj_distance_category_3:
                     ultrasonic_distance.append(distance)
                  print("\nPrinting Obeject distance:")
                  print(ultrasonic_distance)

                  if(File_Write_Flag == True):
                        File_Write_Flag = False
                        File_name_t = "Point_cloud_" + datetime.datetime.now().strftime("%Y_%m_%d_%H:%M")[:-3] + ".xyz"
                        File_name = os.path.join('/home/debian/Final_Project/Point_Cloud_Data', File_name_t)

                  file = open(File_name,"a+")
                  for distance in ultrasonic_distance:
                        x = distance*math.sin(thetha)*math.cos(phi)
                        y = distance*math.sin(thetha)*math.cos(phi)
                        z = distance*math.cos(thetha)
                        file.write("%f %f %f\n"%(x,y,z))
                  file.close()
                       

                      
        ser.close()

except KeyboardInterrupt:
         ser.close()
 

