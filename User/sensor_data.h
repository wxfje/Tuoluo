#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <stdint.h>

typedef struct
{
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
}direct_data_t;//�洢�Ӵ�����������ʵ��ֵ

typedef struct
{
    float gyro_x;
    float gyro_y;
    float gyro_z;//�����ǵ�����Ϊ����
    
    float accel_x;
    float accel_y;
    float accel_z;   

    float angle_x;
    float angle_y;
    float angle_z;
}after_data_t;//����������ֵ

typedef struct
{
    direct_data_t direct;
    after_data_t after;
}sensor_data_t;



extern volatile sensor_data_t SensorData;
extern void sensor_data_with(void);


#endif

