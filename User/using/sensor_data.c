/********************************Copyright ( c ) ***************************
** COPYRIGHT (C) 2014 - 2015,sunhuaming, ShengZhen, China 
** FileName     :                  
** Abstruct     :                  
** 
** 
** Change Logs  :  
** Version    Date        Author        Notes
** v0.1.0    2015-4-26    sunhuaming    first version
** 
****************************************************************************/
#include <stdio.h>
#include "stm32f10x.h"
#include "sensor_data.h"
#include "L3G4200D.h"
#include "ADXL345.h"
#include "dlog.h"
#include <math.H>
volatile sensor_data_t SensorData;

#define U16BIT_HIGH8(x)  ((x>>8)&0XFF)
#define U16BIT_LOW_8(x)  (x&0XFF)
extern void USART_SendByte( USART_TypeDef* USARTx, uint8_t Data);



#if 0
void Data_Send_Senser(void);
void L3G4_data(void)
{
    float x,y,z;
    static uint16_t time = 0;
    //读取L3G4200D数据
    uint8_t i;
    short a,b,c;
    static short x_table[100];
    static short y_table[100];
    static short z_table[100];
    static uint8_t xyz = 0;
    //dps = 2000时，灵敏度为70mdps/digit
    // 则x = T_X *70/1000 度/秒
    //
    READ_L3G4200D();
    x = (float)T_X*70/1000;
    y = (float)T_Y*70/1000;
    z = (float)T_Z*70/1000;

    time++;
    Angle_gx = x;
    Angle_gy = y;
    Angle_gz = z;
    /*if( time >= 250)
    {
        time = 0;
        printf( "L3G400D DATA: %d, %d, %d \r\n",T_X,T_Y,T_Z);
        printf( "L3G400D DATA: %4f, %4f, %4f \r\n",Angle_gx,Angle_gy,Angle_gz);
    }*/
    //printf( "x:%d y:%d z:%d \r\n",T_X,T_Y,T_Z);


//printf( "x:%d y:%d z:%d \r\n",T_X,T_Y,T_Z);
}
#endif







/****************************************************************************
** Description        : 根据匿名软件发送数据
** Input parameters   : 
** Output parameters  : 
**                      
** Returned value     : 
** Notice             : 
** Created by         : sunhuaming(2015-4-26)
**--------------------------------------------------------------------------
** Modified by        :      
** Modified by        :      
****************************************************************************/
static void Data_Send_Senser(void)
{
    uint8_t _cnt=0;
    uint8_t sum = 0;
    uint8_t data_to_send[50];
    uint8_t i;

    data_to_send[_cnt++]=0xAA;
    data_to_send[_cnt++]=0xAA;
    data_to_send[_cnt++]=0x02;
    data_to_send[_cnt++]=0;
    data_to_send[_cnt++]= U16BIT_HIGH8(SensorData.direct.accel_x);  
    data_to_send[_cnt++]= U16BIT_LOW_8(SensorData.direct.accel_x); 
    data_to_send[_cnt++]= U16BIT_HIGH8(SensorData.direct.accel_y); 
    data_to_send[_cnt++]= U16BIT_LOW_8(SensorData.direct.accel_y); 
    data_to_send[_cnt++]= U16BIT_HIGH8(SensorData.direct.accel_z); 
    data_to_send[_cnt++]= U16BIT_LOW_8(SensorData.direct.accel_z); 
    data_to_send[_cnt++]= U16BIT_HIGH8(SensorData.direct.gyro_x);
    data_to_send[_cnt++]= U16BIT_LOW_8(SensorData.direct.gyro_x);
    data_to_send[_cnt++]= U16BIT_HIGH8(SensorData.direct.gyro_y);
    data_to_send[_cnt++]= U16BIT_LOW_8(SensorData.direct.gyro_y);
    data_to_send[_cnt++]= U16BIT_HIGH8(SensorData.direct.gyro_z);
    data_to_send[_cnt++]= U16BIT_LOW_8(SensorData.direct.gyro_z);
    data_to_send[_cnt++]=0;
    data_to_send[_cnt++]=0;
    data_to_send[_cnt++]=0;
    data_to_send[_cnt++]=0;
    data_to_send[_cnt++]=0;
    data_to_send[_cnt++]=0;

    data_to_send[3] = _cnt-4;


    for( i=0; i<_cnt; i++)
        sum += data_to_send[i];
    data_to_send[_cnt++] = sum;

    for( i=0; i<_cnt; i++)
        USART_SendByte( USART1, data_to_send[i]);
    //Uart1_Put_Buf(data_to_send,_cnt);
}

void Data_Send_Status(void)
{
	u8 _cnt=0;
    uint8_t i,sum = 0;
    uint8_t data_to_send[50];
    
	vs16 _temp;  
	vs32 _temp2 = 0;//Alt;
    
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x01;
	data_to_send[_cnt++]=0;

	_temp = (int)(SensorData.after.angle_x*100);
	data_to_send[_cnt++]=U16BIT_HIGH8(_temp);
	data_to_send[_cnt++]=U16BIT_LOW_8(_temp);
	_temp = (int)(SensorData.after.angle_y*100);
	data_to_send[_cnt++]=U16BIT_HIGH8(_temp);
	data_to_send[_cnt++]=U16BIT_LOW_8(_temp);
	//_temp = (int)(Q_ANGLE.YAW*100);
	_temp = (int)(0*100);//Mag_Heading
	data_to_send[_cnt++]=U16BIT_HIGH8(_temp);
	data_to_send[_cnt++]=U16BIT_LOW_8(_temp);
	_temp = 0;//Alt_CSB;
	data_to_send[_cnt++]=U16BIT_HIGH8(_temp);
	data_to_send[_cnt++]=U16BIT_LOW_8(_temp);

	data_to_send[_cnt++]= (_temp2>>24)&0xff;
	data_to_send[_cnt++]= (_temp2>>16)&0xff;
	data_to_send[_cnt++]= (_temp2>>8)&0xff;
	data_to_send[_cnt++]= (_temp2)&0xff;
		
//	if(Rc_C.ARMED==0)		data_to_send[_cnt++]=0xA0;	
//	else if(Rc_C.ARMED==1)		data_to_send[_cnt++]=0xA1;
	data_to_send[_cnt++]=0xA0;
    
    
	data_to_send[3] = _cnt-4;
	
    
    for( i=0; i<_cnt; i++)
        sum += data_to_send[i];
    data_to_send[_cnt++] = sum;

    for( i=0; i<_cnt; i++)
        USART_SendByte( USART1, data_to_send[i]);    
}

/****************************************************************************
** Description        : 陀螺仪数据滤波
** Input parameters   : 
** Output parameters  : 
**                      
** Returned value     : 
** Notice             : 
** Created by         : sunhuaming(2015-4-26)
**--------------------------------------------------------------------------
** Modified by        :      
** Modified by        :      
****************************************************************************/
static void gyro_data_with(void)
{
    static short gyro_x_table[10];
    static short gyro_y_table[10];
    static short gyro_z_table[10];
    static uint8_t gyro_count = 0;
    int32_t temp_x,temp_y,temp_z;
    uint8_t i;
    float x,y,z;
    
    READ_L3G4(  (int16_t *)&SensorData.direct.gyro_x,
                (int16_t *)&SensorData.direct.gyro_y,
                (int16_t *)&SensorData.direct.gyro_z);

    gyro_x_table[gyro_count] = SensorData.direct.gyro_x;
    gyro_y_table[gyro_count] = SensorData.direct.gyro_y;
    gyro_z_table[gyro_count] = SensorData.direct.gyro_z;
    gyro_count ++;
    if(gyro_count >= 10)gyro_count = 0;

    temp_x = 0;
    temp_y = 0;
    temp_z = 0;

    for(i = 0; i<10; i++)
    {
        temp_x += gyro_x_table[i];
        temp_y += gyro_y_table[i];
        temp_z += gyro_z_table[i];
    }
    SensorData.direct.gyro_x = temp_x/10-12;
    SensorData.direct.gyro_y = temp_y/10-12;
    SensorData.direct.gyro_z = temp_z/10-12;//滑动平均滤波
    
    //dps = 2000时，灵敏度为70mdps/digit
    // 则x = T_X *70/1000 度/秒
    //
    

    x = (float)SensorData.direct.gyro_x*8.75/1000;//将数据转换为度
    y = (float)SensorData.direct.gyro_y*8.75/1000;
    z = (float)SensorData.direct.gyro_z*8.75/1000; 
    
//    x = (float)SensorData.direct.gyro_x/70;//将数据转换为度
//    y = (float)SensorData.direct.gyro_y/70;
//    z = (float)SensorData.direct.gyro_z/70;   
    
    #define RADI    (3.1415926f/180)
//    SensorData.after.gyro_x = x*RADI; //将度转为弧度
//    SensorData.after.gyro_y = y*RADI; 
//    SensorData.after.gyro_z = z*RADI;     
    //由于硬件原因，需要将陀螺仪和加速度的xy进行调换
    SensorData.after.gyro_x = -y*RADI;
    SensorData.after.gyro_y = x*RADI;
    SensorData.after.gyro_z = z*RADI; 
}

/****************************************************************************
** Description        : 把陀螺仪的数据转换为弧度
** Input parameters   : 
** Output parameters  : 
**                      
** Returned value     : 
** Notice             : 
** Created by         : sunhuaming(2015-4-26)
**--------------------------------------------------------------------------
** Modified by        :      
** Modified by        :      
****************************************************************************/
static void accel_data_with(void)
{
    static short x_table[10];
    static short y_table[10];
    static short z_table[10];
    static uint8_t count = 0;
    int32_t temp_x,temp_y,temp_z;
    uint8_t i;
    float x,y,z;
    
    Read_ADXL345(   (int16_t *)&SensorData.direct.accel_x,
                    (int16_t *)&SensorData.direct.accel_y,
                    (int16_t *)&SensorData.direct.accel_z);
    
    x_table[count] = SensorData.direct.accel_x;
    y_table[count] = SensorData.direct.accel_y;
    z_table[count] = SensorData.direct.accel_z;
    count ++;
    if(count >= 10)count = 0;

    temp_x = 0;
    temp_y = 0;
    temp_z = 0;

    for(i = 0; i<10; i++)
    {
        temp_x += x_table[i];
        temp_y += y_table[i];
        temp_z += z_table[i];
    }
    SensorData.direct.accel_x = temp_x/10;
    SensorData.direct.accel_y = temp_y/10;
    SensorData.direct.accel_z = temp_z/10;//滑动平均滤波    


    SensorData.after.accel_x = SensorData.direct.accel_x*3.9; //单位mg
    SensorData.after.accel_y = SensorData.direct.accel_y*3.9; 
    SensorData.after.accel_z = SensorData.direct.accel_z*3.9;   
    
    SensorData.after.accel_x = SensorData.after.accel_x/1000; //单位g
    SensorData.after.accel_y = SensorData.after.accel_y/1000; 
    SensorData.after.accel_z = SensorData.after.accel_z/1000;    
//    SensorData.direct.accel_x = SensorData.direct.accel_x*3.9; 
//    SensorData.direct.accel_y = SensorData.direct.accel_y*3.9; 
//    SensorData.direct.accel_z = SensorData.direct.accel_z*3.9;      
}

/****************************************************************************
** Description        : 
** Input parameters   : 
** Output parameters  : 
**                      
** Returned value     : 
** Notice             : 
** Created by         : sunhuaming(2015-4-26)
**--------------------------------------------------------------------------
** Modified by        :      
** Modified by        :      
****************************************************************************/
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az);
void sensor_data_with(void)
{
    float AngleAx,AngleAy;
    static uint8_t times = 0;
    gyro_data_with();
    accel_data_with();
    IMUupdate( SensorData.after.gyro_x,SensorData.after.gyro_y,SensorData.after.gyro_z, 
               SensorData.after.accel_x, SensorData.after.accel_y, SensorData.after.accel_z);
    
    AngleAx=atan(SensorData.after.accel_x/sqrt(SensorData.after.accel_y*SensorData.after.accel_y+SensorData.after.accel_z*SensorData.after.accel_z))*57.2957795f; //??????180/PI ????????
    AngleAy=atan(SensorData.after.accel_y/sqrt(SensorData.after.accel_x*SensorData.after.accel_x+SensorData.after.accel_z*SensorData.after.accel_z))*57.2957795f;
    //AngleAx=atan(SensorData.direct.accel_x/sqrt(SensorData.direct.accel_y*SensorData.direct.accel_y+SensorData.direct.accel_z*SensorData.direct.accel_z))*57.2957795f; 
    //AngleAy=atan(SensorData.direct.accel_y/sqrt(SensorData.direct.accel_x*SensorData.direct.accel_x+SensorData.direct.accel_z*SensorData.direct.accel_z))*57.2957795f;
//    SensorData.after.angle_x = AngleAx;
//    SensorData.after.angle_y = AngleAy;
//Data_Send_Status();    
    times++;
    if( times >= 5)
    {
        times = 0;
        printf("%5.2f,%5.2f,    %5.2f,%5.2f\r\n",SensorData.after.angle_x,AngleAx,SensorData.after.angle_y,AngleAy);
        
        printf("      %5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f", SensorData.after.gyro_x,SensorData.after.gyro_y,SensorData.after.gyro_z,
                                                  SensorData.after.accel_x, SensorData.after.accel_y, SensorData.after.accel_z);    
        printf("      %d,%d,%d\r\n",SensorData.direct.gyro_x,SensorData.direct.gyro_y,SensorData.direct.gyro_z);
    
    }
//printf("      %d,%d,%d\r\n",SensorData.direct.gyro_x,SensorData.direct.gyro_y,SensorData.direct.gyro_z);
    //printf("%4f,%4f\r\n",SensorData.after.angle_x,SensorData.after.angle_y);
    //Data_Send_Senser();
    
//    times++;
//    if( times >= 5)
//    {
//        times = 0;
//        printf("%d,%d,%d,  %d,%d,%d\r\n",SensorData.direct.accel_x ,SensorData.direct.accel_y,SensorData.direct.accel_z
//                                        ,SensorData.direct.gyro_x ,SensorData.direct.gyro_y,SensorData.direct.gyro_z);
//    
//    }    
}



