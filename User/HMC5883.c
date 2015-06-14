/******************************************************************************
** File      : HMC5883.c 电子罗盘
** Abstruct  : HMC5883 IIC驱动程序/stm32,72MHz
** COPYRIGHT (C) 2013 - 2015,PACE Corp., ShengZhen, China 
**
** Change Logs:
** Version		Date			Author		Notes
** v0.1.0		2015-04-19		Emmett_sun		first version,Creat this file
**
**
**
******************************************************************************/
#include <math.h>    
#include <stdio.h>   
#include <stdint.h> 	
#include "I2C.h"
#include "HMC5883.h"

#define	HMC5883_Addr   0x3C	  //定义器件在IIC总线中的从地址
unsigned char HMC5883_Buffer[8]; 
void DelayMS(uint32_t n);
   
//void  Single_Write_HMC5883(uint8_t REG_Address,uint8_t REG_data);   //单个写入数据
//uint8_t Single_Read_HMC5883(uint8_t REG_Address);                   //单个读取内部寄存器数据
//void  Multiple_Read_HMC5883(void);                                  //连续的读取内部寄存器数据


/******************************************************************************
** 函数名称:  
** 功能描述:  
** 输　入  :  
** 输  出  :  
** 说  明  ： 无
** 注  意  ： 无
******************************************************************************/
static void Single_Write_HMC5883(uint8_t REG_Address,uint8_t REG_data)
{
    I2C_Start();                  //起始信号
    I2C_SendByte(HMC5883_Addr);   //发送设备地址+写信号
    I2C_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf 
    I2C_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf
    I2C_Stop();                   //发送停止信号
}

/******************************************************************************
** 函数名称:  
** 功能描述: 单字节读取内部寄存器 
** 输　入  :  
** 输  出  :  
** 说  明  ： 无
** 注  意  ： 无
******************************************************************************/
static uint8_t Single_Read_HMC5883(uint8_t REG_Address)
{  
	uint8_t REG_data;
    if( I2C_Start()== FALSE)printf("Error\r\n");//起始信号                       
    I2C_SendByte(HMC5883_Addr);           //发送设备地址+写信号
	I2C_SendByte(REG_Address);                   //发送存储单元地址，从0开始	
	if( I2C_Start()== FALSE)printf("Error\r\n");                          //起始信号
	I2C_SendByte(HMC5883_Addr+1);         //发送设备地址+读信号
	REG_data=I2C_RadeByte();              //读出寄存器数据
	I2C_NoAck();   
	I2C_Stop();                           //停止信号
    return REG_data; 
}
/******************************************************************************
** 函数名称:  
** 功能描述: 连续读出HMC5883内部角度数据，地址范围0x3~0x5 
** 输　入  :  
** 输  出  :  
** 说  明  ： 无
** 注  意  ： 无
******************************************************************************/
static void Multiple_Read_HMC5883(void)
{   
    uint8_t i;
    if( I2C_Start()== FALSE)printf("Error\r\n");                          //起始信号
    I2C_SendByte(HMC5883_Addr);           //发送设备地址+写信号
    I2C_SendByte(0x03);                   //发送存储单元地址，从0x3开始	
    if( I2C_Start()== FALSE)printf("Error\r\n");                           //起始信号
    I2C_SendByte(HMC5883_Addr+1);         //发送设备地址+读信号
    for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        HMC5883_Buffer[i] = I2C_RadeByte();          //BUF[0]存储数据
        if (i == 5)
        {
            I2C_NoAck();                //最后一个数据需要回NOACK
        }
        else
        {
            I2C_Ack();                //回应ACK
        }
    }
    I2C_Stop();                          //停止信号
}
/******************************************************************************
** 函数名称:  
** 功能描述: 初始化HMC5883，根据需要请参考pdf进行修改
** 输　入  :  
** 输  出  :  
** 说  明  ： 无
** 注  意  ： 无
******************************************************************************/
void Init_HMC5883( void )//读取HMC 的id，为H34
{
	if( Single_Read_HMC5883(10) != 'H')return;
	if( Single_Read_HMC5883(11) != '4')return;
	if( Single_Read_HMC5883(12) != '3')return;

	Single_Write_HMC5883(0x00,0x78);
	Single_Write_HMC5883(0x01,0x60);
	Single_Write_HMC5883(0x02,0x00);  
}

/*unsigned char my_atan(uint8_t x,uint8_t y)  //数据提取 查表法得角度代替:atan2(y,x) * (180 / 3.14159265) +180;
{   
 unsigned int ptan;
 unsigned char point;
	ptan=abs(y*100/x);
	
	if (ptan>0 && ptan<=20)   point=1;
	if (ptan>20 && ptan<=40)  point=2;
	if (ptan>40 && ptan<=65)  point=3;
	if (ptan>65 && ptan<=100) point=4;
	if (ptan>100 && ptan<=150)point=5;
	if (ptan>150 && ptan<=240)point=6;
	if (ptan>240 && ptan<=500)point=7;
	if (ptan>500)             point=8;
	
	if (x>=0 && y>0)point=point;
	if (x<0 && y>=0)point=17-point;
	if (x<=0 && y<0)point=16+point;
	if (x>0 && y<=0)point=33-point;
	
	
	point=32-point;
	if (point==32) point=0;
	return point;
	
}*/

void HMC5883_Read( void )
{ 
   unsigned int i;
   int x,y,z;
   double angle;
	x = 0;y = 0;z = 0;

    Multiple_Read_HMC5883();      //连续读出数据，存储在BUF中
	//---------显示X轴
    x=HMC5883_Buffer[0] << 8 | HMC5883_Buffer[1]; //Combine MSB and LSB of X Data output register
    z=HMC5883_Buffer[2] << 8 | HMC5883_Buffer[3]; //Combine MSB and LSB of Z Data output register
    y=HMC5883_Buffer[4] << 8 | HMC5883_Buffer[5]; //Combine MSB and LSB of Y Data output register

//	if (x>0x7fff)x = (~x +1) &0xffff;
//	if (y>0x7fff)y = (~y +1) &0xffff;
//	if (z>0x7fff)z = (~z +1) &0xffff;

	if (x>0x7fff)x-=0xffff;
	if (y>0x7fff)y-=0xffff;
	if (z>0x7fff)z-=0xffff;
	     
//       x=x+30;
//       y=y+180;	//60等级的椭圆磁场偏移量，最大+最小除以2，不加不能算出角度 */
    //angle= atan2((double)y,(double)x) * (180 / 3.14159265) + 180; // angle in degrees
	//x = 100;y = 100;
	angle= atan2((double)y,(double)x);
	angle *= 180;
	angle /= 3.14159265 ;
	//printf("%f",angle);
	//angle += 180;
	i = angle;
	
 /*     i = my_atan(x,y);*/	
	printf("HMC5883 data: %d,%d,%d",x,y,z);

	printf("  :   %d \r\n",i);
} 
