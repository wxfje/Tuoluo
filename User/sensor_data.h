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
}direct_data_t;//存储从传感器读出的实际值

typedef struct
{
    float gyro_x;
    float gyro_y;
    float gyro_z;//陀螺仪的数据为弧度
    
    float accel_x;
    float accel_y;
    float accel_z;   

    float angle_x;
    float angle_y;
    float angle_z;
}after_data_t;//经过处理后的值

typedef struct
{
    direct_data_t direct;
    after_data_t after;
}sensor_data_t;



extern volatile sensor_data_t SensorData;
extern void sensor_data_with(void);


#endif

