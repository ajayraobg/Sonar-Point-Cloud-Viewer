/*
 This software uses a BSD license.

Copyright (c) 2010, Sean Cross / chumby industries
All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.  
   * Neither the name of Sean Cross / chumby industries nor the names
   of its contributors may be used to endorse or promote products
   derived from this software without specific prior written
   permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
*/

#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
extern void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float* q0, float* q1, float* q2, float* q3);
float ax, ay, az, gx, gy, gz, mx, my, mz;

#define I2C_FILE_NAME "/dev/i2c-2"
#define USAGE_MESSAGE \
    "Usage:\n" \
    "  %s r [addr] [register]   " \
        "to read value from [register]\n" \
    "  %s w [addr] [register] [value]   " \
        "to write a value [value] to register [register]\n" \
    ""

static int set_i2c_register(int file,
                            unsigned char addr,
                            unsigned char reg,
                            unsigned char value) {

  unsigned char outbuf[2];
  struct i2c_rdwr_ioctl_data packets;
  struct i2c_msg messages[1];

  messages[0].addr  = addr;
  messages[0].flags = 0;
  messages[0].len   = sizeof(outbuf);
  messages[0].buf   = outbuf;

  /* The first byte indicates which register we'll write */
  outbuf[0] = reg;

  /* 
   * The second byte indicates the value to write.  Note that for many
   * devices, we can write multiple, sequential registers at once by
   * simply making outbuf bigger.
   */
  outbuf[1] = value;

  /* Transfer the i2c packets to the kernel and verify it worked */
  packets.msgs  = messages;
  packets.nmsgs = 1;
  if(ioctl(file, I2C_RDWR, &packets) < 0) {
    perror("Unable to send data");
    return 1;
  }

  return 0;
}


static int get_i2c_registers(int file,
		      unsigned char addr,
		      unsigned char first_reg,
		      char num_regs,
		      unsigned char *val) {
  unsigned char outbuf;
  struct i2c_rdwr_ioctl_data packets;
  struct i2c_msg messages[2];

  /*
   * In order to read a register, we first do a "dummy write" by writing
   * 0 bytes to the register we want to read from.  This is similar to
   * the packet in set_i2c_register, except it's 1 byte rather than 2.
   */
  outbuf = first_reg;
  messages[0].addr  = addr;
  messages[0].flags = 0;
  messages[0].len   = sizeof(outbuf);
  messages[0].buf   = &outbuf;

  /* The data will get returned in this structure */
  messages[1].addr  = addr;
  messages[1].flags = I2C_M_RD/* | I2C_M_NOSTART*/;
  messages[1].len   = num_regs;
  messages[1].buf   = val;

  /* Send the request to the kernel and get the result back */
  packets.msgs      = messages;
  packets.nmsgs     = 2;
  if(ioctl(file, I2C_RDWR, &packets) < 0) {
    perror("Unable to send data");
    return 1;
  }
  return 0;
}

static int get_i2c_register(int file,
                            unsigned char addr,
                            unsigned char reg,
                            unsigned char *val) {
  unsigned char inbuf, outbuf;
  struct i2c_rdwr_ioctl_data packets;
  struct i2c_msg messages[2];

  /*
   * In order to read a register, we first do a "dummy write" by writing
   * 0 bytes to the register we want to read from.  This is similar to
   * the packet in set_i2c_register, except it's 1 byte rather than 2.
   */
  outbuf = reg;
  messages[0].addr  = addr;
  messages[0].flags = 0;
  messages[0].len   = sizeof(outbuf);
  messages[0].buf   = &outbuf;

  /* The data will get returned in this structure */
  messages[1].addr  = addr;
  messages[1].flags = I2C_M_RD/* | I2C_M_NOSTART*/;
  messages[1].len   = sizeof(inbuf);
  messages[1].buf   = &inbuf;

  /* Send the request to the kernel and get the result back */
  packets.msgs      = messages;
  packets.nmsgs     = 2;
  if(ioctl(file, I2C_RDWR, &packets) < 0) {
    perror("Unable to send data");
    return 1;
  }
  *val = inbuf;

  return 0;
}

void modify_mag_data(int8_t *mag_val_arr, float *mag_val_tot){
  int16_t mag_x_h = (int16_t)mag_val_arr[1]<<8;
  int16_t mag_x_l = (int16_t)mag_val_arr[2];
  int16_t mag_y_h = (int16_t)mag_val_arr[3]<<8;
  int16_t mag_y_l = (int16_t)mag_val_arr[4];
  int16_t mag_z_h = (int16_t)mag_val_arr[5]<<8;
  int16_t mag_z_l = (int16_t)mag_val_arr[6];
  mag_val_tot[0] = 0.15 * (mag_x_h | mag_x_l);
  mag_val_tot[1] = 0.15*(mag_y_h | mag_y_l);
  mag_val_tot[2] = 0.15*(mag_z_h | mag_z_l);
  return;
}

int modify_data(int8_t *val, float *val_arr){ // refer https://www.hackster.io/30503/using-the-mpu9250-to-get-real-time-motion-data-08f011
  val_arr[0] = (2.0/32768.0) * (((int16_t)val[0] << 8) | val[1]);//ax
  val_arr[1] = (2.0/32768.0) * (((int16_t)val[2]<<8) | val[3]);//ay
  val_arr[2] = (2.0/32768.0) * (((int16_t)val[4]<<8) | val[5]);//az
  val_arr[3] = (250.0/32768.0) * (((int16_t)val[8]) << 8 | val[9]);//gx
  val_arr[4] = (250.0/32768.0) * (((int16_t)val[10]) << 8 | val[11]);//gy
  val_arr[5] = (250.0/32768.0) * (((int16_t)val[12]) << 8 | val[13]);//gz
  return 0;
}	

double get_roll(float q0, float q1, float q2, float q3)
{
/*
	// roll (x-axis rotation)
	double sinr = +2.0 * (q.w() * q.x() + q.y() * q.z());
	double cosr = +1.0 - 2.0 * (q.x() * q.x() + q.y() * q.y());
	roll = atan2(sinr, cosr);
*/
  double sinr = 2.0 * (q0 * q1 + q2 * q3);
  double cosr = +1.0 - 2.0 * (q1 * q1 + q2 * q2);
  double roll = atan2f(sinr, cosr);
  printf("roll is %ld\n", roll);
  return roll; 
}

double get_pitch(float q0, float q1, float q2, float q3)
{
/*
	// pitch (y-axis rotation)
	double sinp = +2.0 * (q.w() * q.y() - q.z() * q.x());
	if (fabs(sinp) >= 1)
		pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
	else
		pitch = asin(sinp);
*/
  double sinp = 2.0 * (q0 * q2 - q3 * q1);
  float pitch;
  if (fabs (sinp) >= 1)
      pitch = copysign(M_PI / 2, sinp);
  else
      pitch = asin(sinp);
  printf("pitch is %ld\n", pitch);
  return pitch;
}

double get_yaw(float q0, float q1, float q2, float q3)
{
/*
	// yaw (z-axis rotation)
	double siny = +2.0 * (q.w() * q.z() + q.x() * q.y());
	double cosy = +1.0 - 2.0 * (q.y() * q.y() + q.z() * q.z());  
	yaw = atan2(siny, cosy);
*/

  double siny = 2.0 * (q0 * q3 + q1 * q2);
  double cosy = 1 - 2 * (q2 * q2 + q3 * q3);
  float yaw = atan2(siny, cosy);
  printf("yaw is %ld\n", yaw);
  return yaw;
}

void delay_time()
{
  for (double i = 0;i<6000;i++)
  {
     for(double j = 0; j<2000;j++);
  }
 return;
}
void imu_val_gen(void) {
  int i2c_file;
  int addr = 104; //accel and gyro address
  int mag_addr = 12; // magneto device address
  int mag_reg_addr = 2; //magnetometer register address
  int reg = 59; //accel and gyro register starting address
  int8_t  value[14];
  int8_t mag_val_arr[8];
  int num_reg =6;
  float val_arr[6], val_arr_store[6];
  float mag_val_arr_tot[3], mag_val_arr_tot_old[3];
  int addr_byp = 55;
  int byp_val = 2;
  int mag_sett = 18;
  int mag_cntl1 = 10;
  float diff = 0;
  int changed = 0;
 // float roll = 0, pitch = 0;
  //float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;  // quaternion of sensor frame relative to auxiliary frame
  // Open a connection to the I2C userspace control file.
  if ((i2c_file = open(I2C_FILE_NAME, O_RDWR)) < 0) {
    perror("Unable to open i2c control file");
    exit(1);
  }
  set_i2c_register(i2c_file, addr, addr_byp, byp_val);
  //delay_time();
  set_i2c_register(i2c_file, mag_addr, mag_cntl1, mag_sett);
  delay_time();
 // while(1){
    get_i2c_registers(i2c_file, mag_addr, mag_reg_addr, 8,mag_val_arr);
    modify_mag_data(mag_val_arr, mag_val_arr_tot);
    /*for(int i = 0; i < 3; i++){
    printf("magnetometer value: %f\n", mag_val_arr_tot[i]);
    }*/
    if(get_i2c_registers(i2c_file, addr, reg, 14, value)) {
      printf("Unable to get register!\n");
    }
    else {
      modify_data(value,val_arr);
     // diff = val_arr_store[5] - val_arr[5];
     // for (int k = 0; k<6; k++){
      //diff = val_arr_store[2] - val_arr[2];
      // if((-0.026 >= (val_arr_store[0] - val_arr[0]) >= 0.026) || (-0.05 >= (val_arr_store[1] - val_arr[1]) >= 0.05) && ((-0.9 <= (val_arr[2]) <= 0.9) && (-0.05 >= (val_arr_store[2] - val_arr[2]) >= 0.05)) || (-200 >= (mag_val_arr_tot_old[1] - mag_val_arr_tot[1]) >= 200) || (-200 >= (mag_val_arr_tot_old[2] - mag_val_arr_tot[2]) >= 200) || (-0.4 >= (val_arr_store[3] - val_arr[3]) >= 0.3) || (-0.5 >= (val_arr_store[4] - val_arr[4]) >= 0.5) || (-0.3 >= (val_arr_store[5] - val_arr[5]) >= 0.3) ) {
   //   if ((diff>0 && diff <= 0.5) || (diff<0 && diff >= -0.5))
     // {
          changed = 1;
      //}
     //   printf("Reached here\n");
     //   break;
      //}
   // }
    if (changed == 1){
     //printf("ax: %f\t ay: %f\t az: %f\t gx: %f\t gy: %f\t gz: %f\t mx: %f\t my: %f\t mz: %f\t\n", val_arr[0], val_arr[1], val_arr[2], val_arr[3], val_arr[4], val_arr[5], mag_val_arr_tot[0], mag_val_arr_tot[1], mag_val_arr_tot[2] );//, val_arr[1], val_arr[2], val_arr[3], val_arr[4], val_arr[5]);
    //  , ay: %f, az: %f \nGyro Register gx: %f, gy: %f, gz: %f 
     ax = val_arr[0];
     ay = val_arr[1];
     az = val_arr[2];
     gx = val_arr[3];
     gy = val_arr[4];
     gz = val_arr[5];
     mx = mag_val_arr_tot[0];
     my = mag_val_arr_tot[1];
     mz = mag_val_arr_tot[2];
      changed = 0;
      }
      for (int k = 0; k<6; k++){
      val_arr_store[k] = val_arr[k];
      mag_val_arr_tot_old[k] = mag_val_arr_tot[k];
      }
    }
 // MadgwickAHRSupdate(val_arr[3], val_arr[4], val_arr[5], val_arr[0], val_arr[1], val_arr[2], mag_val_arr_tot[0], mag_val_arr_tot[1], mag_val_arr_tot[2], &q0, &q1, &q2, &q3);
 //printf("Q0: %f, Q1: %f, Q2: %f, Q3: %f\n", q0, q1, q2, q3);
    //printf("Hi");
 
//   roll  = (atan2(-val_arr_store[1], val_arr_store[2]));
//   pitch = (atan2f(val_arr_store[0], sqrt(val_arr_store[1]*val_arr_store[1] + val_arr_store[2]*val_arr_store[2]))*180.0)/M_PI;
//   printf("roll: %f pitch : %f\n", roll, pitch);
 
 // get_roll(q0, q1, q2, q3);
    //get_pitch(q0, q1, q2, q3);
   // get_yaw(q0, q1, q2, q3);
   //delay_time();
  // delay_time();
//	sleep(0.5);
  //}
  close(i2c_file);
 
}
