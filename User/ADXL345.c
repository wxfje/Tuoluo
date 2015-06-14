/******************************************************************************
** File      : ADXL345.c 加速度
** Abstruct  : ADXL345 IIC驱动程序/stm32,72MHz
** COPYRIGHT (C) 2013 - 2015,PACE Corp., ShengZhen, China
**
** Change Logs:
** Version      Date            Author      Notes
** v0.1.0       2015-04-19      Emmett_sun      first version,Creat this file
**
**
**
******************************************************************************/
#include <math.h>    //Keil library  
#include <stdio.h>   //Keil library
#include <stdint.h>
#include "I2C.h"
#include "ADXL345.h"

#define   uchar unsigned char
#define   uint unsigned int
//#define   DataPort P0    //LCD1602数据端口

#define ADXL345_WR_ADDR  0xA6      //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
#define ADXL345_RD_ADDR  0xA7                                //ALT  ADDRESS引脚接地时地址为0xA6，接电源时地址为0x3A
//#define   SlaveAddress   0xa6//0x3a//

typedef unsigned char  BYTE;
typedef unsigned short WORD;

uint8_t ADXL345_Buffer[8];                         //接收数据缓存区
static uchar ge,shi,bai,qian,wan;           //显示变量
int  dis_data;                       //变量

//void delay(unsigned int k);
//void InitLcd();                      //初始化lcd1602
//void Init_ADXL345(void);             //初始化ADXL345

//void WriteDataLCM(uchar dataW);
//void WriteCommandLCM(uchar CMD,uchar Attribc);
//void DisplayOneChar(uchar X,uchar Y,uchar DData);
//void conversion(uint temp_data);

//void  Single_Write_ADXL345(uchar REG_Address,uchar REG_data);   //单个写入数据
//uchar Single_Read_ADXL345(uchar REG_Address);                   //单个读取内部寄存器数据
//void  Multiple_Read_ADXL345(void);                                  //连续的读取内部寄存器数据
//------------------------------------
//void Delay5us(void);
//void Delay5ms(void);
//void ADXL345_Start(void);
//void ADXL345_Stop(void);


//void ADXL345_SendByte(BYTE dat);
//BYTE ADXL345_RecvByte(void);
//void ADXL345_ReadPage(void);
//void ADXL345_WritePage(void);
//-----------------------------------


/******************************************************************************
** 函数名称:
** 功能描述: 单字节写入
** 输　入  :
** 输  出  :
** 说  明  ： 无
** 注  意  ： 无
******************************************************************************/
static void Single_Write_ADXL345(uchar REG_Address,uchar REG_data)
{
    I2C_Start();                  //起始信号
    I2C_SendByte(ADXL345_WR_ADDR);   //发送设备地址+写信号
    I2C_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页
    I2C_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页
    I2C_Stop();                   //发送停止信号
}
/******************************************************************************
** 函数名称:
** 功能描述: 单字节读取
** 输　入  :
** 输  出  :
** 说  明  ： 无
** 注  意  ： 无
******************************************************************************/
static uint8_t Single_Read_ADXL345(uint8_t REG_Address)
{
    uchar REG_data;
    I2C_Start();                          //起始信号
    I2C_SendByte(ADXL345_WR_ADDR);           //发送设备地址+写信号
    I2C_SendByte(REG_Address);                   //发送存储单元地址，从0开始

    I2C_Start();                          //起始信号
    I2C_SendByte(ADXL345_RD_ADDR);         //发送设备地址+读信号
    REG_data=I2C_RadeByte();              //读出寄存器数据
    I2C_NoAck();
    I2C_Stop();                           //停止信号
    return REG_data;
}


/******************************************************************************
** 函数名称:
** 功能描述: 初始化ADXL345，根据需要请参考pdf进行修改
** 输　入  :
** 输  出  :
** 说  明  ： 无
** 注  意  ： 无
******************************************************************************/
//void DelayMS(uint32_t n);
void Init_ADXL345( void )
{
    uint8_t a;
    Single_Write_ADXL345(0x31,0x0B);   //测量范围,正负16g，13位模式
    Single_Write_ADXL345(0x2C,0x0c);   //速率设定为400hz 参考pdf13页
    Single_Write_ADXL345(0x2D,0x08);   //选择电源模式   参考pdf24页
    Single_Write_ADXL345(0x2E,0x80);   //使能 DATA_READY 中断
    Single_Write_ADXL345(0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
    Single_Write_ADXL345(0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
    Single_Write_ADXL345(0x20,0x05);   //Z 偏移量 根据测试传感器的状态写入pdf29页
    //DelayMS(10);
    a = Single_Read_ADXL345(0X00);
    printf("ADXL345 ID:0x%x",a);
}

/******************************************************************************
** 函数名称:
** 功能描述: 连续读出ADXL345内部加速度数据，地址范围0x32~0x37
** 输　入  :
** 输  出  :
** 说  明  ： 无
** 注  意  ： 无
******************************************************************************/
void Multiple_Read_ADXL345(void)
{
    uchar i;
    unsigned int data;
    I2C_Start();                          //起始信号
    I2C_SendByte(ADXL345_WR_ADDR);           //发送设备地址+写信号
    I2C_SendByte(0x32);                   //发送存储单元地址，从0x32开始

    I2C_Start();                          //起始信号
    I2C_SendByte(ADXL345_RD_ADDR);         //发送设备地址+读信号
    for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        ADXL345_Buffer[i] = I2C_RadeByte();          //BUF[0]存储0x32地址中的数据
        if (i == 5)
        {
            I2C_NoAck();                 //最后一个数据需要回NOACK
        }
        else
        {
            I2C_Ack();                //回应ACK
        }
    }
    I2C_Stop();                          //停止信号
//    Delay5ms();

    data=(ADXL345_Buffer[1]<<8)+ADXL345_Buffer[0];
    printf("ADXL345 data : %d,",data);

    data=(ADXL345_Buffer[3]<<8)+ADXL345_Buffer[2];
    printf("%d,",data);

    data=(ADXL345_Buffer[5]<<8)+ADXL345_Buffer[4];
    printf("%d\r\n",data);
}
void Read_ADXL345(int16_t *px,int16_t *py,int16_t *pz)
{
    uchar i;
    unsigned int data;
    I2C_Start();                          //起始信号
    I2C_SendByte(ADXL345_WR_ADDR);           //发送设备地址+写信号
    I2C_SendByte(0x32);                   //发送存储单元地址，从0x32开始

    I2C_Start();                          //起始信号
    I2C_SendByte(ADXL345_RD_ADDR);         //发送设备地址+读信号
    for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        ADXL345_Buffer[i] = I2C_RadeByte();          //BUF[0]存储0x32地址中的数据
        if (i == 5)
        {
            I2C_NoAck();                 //最后一个数据需要回NOACK
        }
        else
        {
            I2C_Ack();                //回应ACK
        }
    }
    I2C_Stop();                          //停止信号
//    Delay5ms();

    *px = (ADXL345_Buffer[1]<<8)+ADXL345_Buffer[0];
    *py = (ADXL345_Buffer[3]<<8)+ADXL345_Buffer[2];
    *pz = (ADXL345_Buffer[5]<<8)+ADXL345_Buffer[4];
}
/*
//***********************************************************************
//显示x轴
void display_x()
{   float temp;
    dis_data=(ADXL345_Buffer[1]<<8)+ADXL345_Buffer[0];  //合成数据
    if(dis_data<0){
    dis_data=-dis_data;
    DisplayOneChar(2,0,'-');      //显示正负符号位
    }
    else DisplayOneChar(2,0,' '); //显示空格

    temp=(float)dis_data*3.9;  //计算数据和显示,查考ADXL345快速入门第4页
    conversion(temp);          //转换出显示需要的数据
    DisplayOneChar(0,0,'X');   //第0行，第0列 显示X
    DisplayOneChar(1,0,':');
    DisplayOneChar(3,0,qian);
    DisplayOneChar(4,0,'.');
    DisplayOneChar(5,0,bai);
    DisplayOneChar(6,0,shi);
    DisplayOneChar(7,0,'g');
}

//***********************************************************************
//显示y轴
void display_y()
{     float temp;
    dis_data=(ADXL345_Buffer[3]<<8)+ADXL345_Buffer[2];  //合成数据
    if(dis_data<0){
    dis_data=-dis_data;
    DisplayOneChar(2,1,'-');      //显示正负符号位
    }
    else DisplayOneChar(2,1,' '); //显示空格

    temp=(float)dis_data*3.9;  //计算数据和显示,查考ADXL345快速入门第4页
    conversion(temp);          //转换出显示需要的数据
    DisplayOneChar(0,1,'Y');   //第1行，第0列 显示y
    DisplayOneChar(1,1,':');
    DisplayOneChar(3,1,qian);
    DisplayOneChar(4,1,'.');
    DisplayOneChar(5,1,bai);
    DisplayOneChar(6,1,shi);
    DisplayOneChar(7,1,'g');
}

//***********************************************************************
//显示z轴
void display_z()
{      float temp;
    dis_data=(ADXL345_Buffer[5]<<8)+ADXL345_Buffer[4];    //合成数据
    if(dis_data<0){
    dis_data=-dis_data;
    DisplayOneChar(10,1,'-');       //显示负符号位
    }
    else DisplayOneChar(10,1,' ');  //显示空格

    temp=(float)dis_data*3.9;  //计算数据和显示,查考ADXL345快速入门第4页
    conversion(temp);          //转换出显示需要的数据
    DisplayOneChar(10,0,'Z');  //第0行，第10列 显示Z
    DisplayOneChar(11,0,':');
    DisplayOneChar(11,1,qian);
    DisplayOneChar(12,1,'.');
    DisplayOneChar(13,1,bai);
    DisplayOneChar(14,1,shi);
    DisplayOneChar(15,1,'g');
}
*/
/*
//*********************************************************
//******主程序********
//*********************************************************
void main()
{
  uchar devid;
  delay(500);                      //上电延时
  InitLcd();                      //液晶初始化ADXL345
  Init_ADXL345();                 //初始化ADXL345
  devid=Single_Read_ADXL345(0X00);//读出的数据为0XE5,表示正确
  while(1)                         //循环
  {
    Multiple_Read_ADXL345();       //连续读出数据，存储在BUF中
    display_x();                   //---------显示X轴
    display_y();                   //---------显示Y轴
    display_z();                   //---------显示Z轴
    delay(200);                    //延时
  }
} */
