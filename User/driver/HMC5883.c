/******************************************************************************
** File      : HMC5883.c ��������
** Abstruct  : HMC5883 IIC��������/stm32,72MHz
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

#define	HMC5883_Addr   0x3C	  //����������IIC�����еĴӵ�ַ
unsigned char HMC5883_Buffer[8]; 
void DelayMS(uint32_t n);
   
//void  Single_Write_HMC5883(uint8_t REG_Address,uint8_t REG_data);   //����д������
//uint8_t Single_Read_HMC5883(uint8_t REG_Address);                   //������ȡ�ڲ��Ĵ�������
//void  Multiple_Read_HMC5883(void);                                  //�����Ķ�ȡ�ڲ��Ĵ�������


/******************************************************************************
** ��������:  
** ��������:  
** �䡡��  :  
** ��  ��  :  
** ˵  ��  �� ��
** ע  ��  �� ��
******************************************************************************/
static void Single_Write_HMC5883(uint8_t REG_Address,uint8_t REG_data)
{
    I2C_Start();                  //��ʼ�ź�
    I2C_SendByte(HMC5883_Addr);   //�����豸��ַ+д�ź�
    I2C_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf 
    I2C_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf
    I2C_Stop();                   //����ֹͣ�ź�
}

/******************************************************************************
** ��������:  
** ��������: ���ֽڶ�ȡ�ڲ��Ĵ��� 
** �䡡��  :  
** ��  ��  :  
** ˵  ��  �� ��
** ע  ��  �� ��
******************************************************************************/
static uint8_t Single_Read_HMC5883(uint8_t REG_Address)
{  
	uint8_t REG_data;
    if( I2C_Start()== FALSE)printf("Error\r\n");//��ʼ�ź�                       
    I2C_SendByte(HMC5883_Addr);           //�����豸��ַ+д�ź�
	I2C_SendByte(REG_Address);                   //���ʹ洢��Ԫ��ַ����0��ʼ	
	if( I2C_Start()== FALSE)printf("Error\r\n");                          //��ʼ�ź�
	I2C_SendByte(HMC5883_Addr+1);         //�����豸��ַ+���ź�
	REG_data=I2C_RadeByte();              //�����Ĵ�������
	I2C_NoAck();   
	I2C_Stop();                           //ֹͣ�ź�
    return REG_data; 
}
/******************************************************************************
** ��������:  
** ��������: ��������HMC5883�ڲ��Ƕ����ݣ���ַ��Χ0x3~0x5 
** �䡡��  :  
** ��  ��  :  
** ˵  ��  �� ��
** ע  ��  �� ��
******************************************************************************/
static void Multiple_Read_HMC5883(void)
{   
    uint8_t i;
    if( I2C_Start()== FALSE)printf("Error\r\n");                          //��ʼ�ź�
    I2C_SendByte(HMC5883_Addr);           //�����豸��ַ+д�ź�
    I2C_SendByte(0x03);                   //���ʹ洢��Ԫ��ַ����0x3��ʼ	
    if( I2C_Start()== FALSE)printf("Error\r\n");                           //��ʼ�ź�
    I2C_SendByte(HMC5883_Addr+1);         //�����豸��ַ+���ź�
    for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        HMC5883_Buffer[i] = I2C_RadeByte();          //BUF[0]�洢����
        if (i == 5)
        {
            I2C_NoAck();                //���һ��������Ҫ��NOACK
        }
        else
        {
            I2C_Ack();                //��ӦACK
        }
    }
    I2C_Stop();                          //ֹͣ�ź�
}
/******************************************************************************
** ��������:  
** ��������: ��ʼ��HMC5883��������Ҫ��ο�pdf�����޸�
** �䡡��  :  
** ��  ��  :  
** ˵  ��  �� ��
** ע  ��  �� ��
******************************************************************************/
void Init_HMC5883( void )//��ȡHMC ��id��ΪH34
{
	if( Single_Read_HMC5883(10) != 'H')return;
	if( Single_Read_HMC5883(11) != '4')return;
	if( Single_Read_HMC5883(12) != '3')return;

	Single_Write_HMC5883(0x00,0x78);
	Single_Write_HMC5883(0x01,0x60);
	Single_Write_HMC5883(0x02,0x00);  
}

/*unsigned char my_atan(uint8_t x,uint8_t y)  //������ȡ ����ýǶȴ���:atan2(y,x) * (180 / 3.14159265) +180;
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

    Multiple_Read_HMC5883();      //�����������ݣ��洢��BUF��
	//---------��ʾX��
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
//       y=y+180;	//60�ȼ�����Բ�ų�ƫ���������+��С����2�����Ӳ�������Ƕ� */
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
