#ifndef __IMU_DATA_DECODE_H__
#define __IMU_DATA_DECODE_H__

#include <stdint.h>
#include <stdbool.h>

extern bool acc_tag_flag;
extern bool gyr_tag_flag;
extern bool mag_tag_flag;
extern bool eul_tag_flag;
extern bool quat_tag_flag;
extern int frame_count;

typedef enum 
{
    kItemID =                   0x90,   /* user programed ID   */
    kItemAccRaw =               0xA0,   /* raw acc             */
    kItemGyrRaw =               0xB0,   /* raw gyro            */  
    kItemMagRaw =               0xC0,   /* raw mag             */
    kItemRotationEul =          0xD0,   /* eular angle         */
    kItemRotationQuat =         0xD1,   /* att q               */
    kItemPressure =             0xF0,   /* pressure            */
    kItemEnd =                  0x00,   
	KItemIMUSOL =              0x91,   /* IMUSOL  */
	KItemGWSOL =               0x62,   /* RFSOL  */
}ItemID_t;

int imu_data_decode_init(void);
int stream2int16(int *dest,uint8_t *src,int offset);
int get_raw_acc(float* a);
int get_frame_count(void);
int get_raw_gyr(float* g);
int get_raw_mag(float* m);
int get_id(uint8_t *user_id);
int get_eul(float* e);
int get_quat(float* q);

#endif

 
