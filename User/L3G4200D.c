/******************************************************************************
** File      : L3G4200D.c ������
** Abstruct  : L3G4200D IIC��������/stm32,72MHz
** COPYRIGHT (C) 2013 - 2015,PACE Corp., ShengZhen, China 
**
** Change Logs:
** Version		Date			Author		Notes
** v0.1.0		2015-04-19		Emmett_sun		first version,Creat this file
**
**
**
******************************************************************************/

#include "stm32f10x.h"
#include  <math.h>    //Keil library  
#include <stdio.h>
#include "L3G4200D.h"
#include "I2C.h"

//GPIO_InitTypeDef GPIO_InitStructure;
//ErrorStatus HSEStartUpStatus;

#define   uchar unsigned char
#define   uint unsigned int	

#define L3G4200D_WR_ADDR    0xd2        //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
#define L3G4200D_RD_ADDR    0xd3
//#define	L3G4200_Addr   0Xd2	 

//**********L3G4200D�ڲ��Ĵ�����ַ*********
#define WHO_AM_I 0x0F
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
#define REFERENCE 0x25
#define OUT_TEMP 0x26
#define STATUS_REG 0x27
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D
#define FIFO_CTRL_REG 0x2E
#define FIFO_SRC_REG 0x2F
#define INT1_CFG 0x30
#define INT1_SRC 0x31
#define INT1_TSH_XH 0x32
#define INT1_TSH_XL 0x33
#define INT1_TSH_YH 0x34
#define INT1_TSH_YL 0x35
#define INT1_TSH_ZH 0x36
#define INT1_TSH_ZL 0x37
#define INT1_DURATION 0x38

unsigned char TX_DATA[4];  
unsigned char BUF[8];                         //�������ݻ�����
char  test=0; 
short T_X,T_Y,T_Z;
float Angle_gx = 0,Angle_gy = 0,Angle_gz = 0;


/* �������� -----------------------------------------------*/
//void RCC_Configuration(void);
//void GPIO_Configuration(void);
//void NVIC_Configuration(void);
//void USART1_Configuration(void);
//void WWDG_Configuration(void);
//void Delay(u32 nTime);
//void Delayms(vu32 m);  




/******************************************************************************
** ��������:  
** ��������: ���ֽ�д�� 
** �䡡��  :  
** ��  ��  :  
** ˵  ��  �� ��
** ע  ��  �� ��
******************************************************************************/
static bool Single_Write(unsigned char REG_Address,unsigned char REG_data)		     //void
{
    I2C_Start();                  //��ʼ�ź�
    I2C_SendByte(L3G4200D_WR_ADDR);   //�����豸��ַ+д�ź�
    I2C_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf 
    I2C_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf
    I2C_Stop();                   //����ֹͣ�ź�
    return TRUE;
}

//���ֽڶ�ȡ*****************************************
static unsigned char Single_Read(unsigned char REG_Address)
{   
 	uint8_t REG_data;
    if( I2C_Start()== FALSE)printf("Error\r\n");//��ʼ�ź�                       
    I2C_SendByte(L3G4200D_WR_ADDR);           //�����豸��ַ+д�ź�
	I2C_SendByte(REG_Address);                   //���ʹ洢��Ԫ��ַ����0��ʼ	
	if( I2C_Start()== FALSE)printf("Error\r\n");                          //��ʼ�ź�
	I2C_SendByte(L3G4200D_RD_ADDR);         //�����豸��ַ+���ź�
	REG_data=I2C_RadeByte();              //�����Ĵ�������
	I2C_NoAck();   
	I2C_Stop();                           //ֹͣ�ź�
    return REG_data;    


}						      

/******************************************************************************
** ��������:  
** ��������:  
** �䡡��  :  
** ��  ��  :  
** ˵  ��  �� ��
** ע  ��  �� ��
******************************************************************************/
void Init_L3G4200D(void)
{

	Single_Write(CTRL_REG1, 0x0f);
	Single_Write(CTRL_REG2, 0x00);
	Single_Write(CTRL_REG3, 0x08);
	//Single_Write(CTRL_REG4, 0x30);	//+-2000dps
    Single_Write(CTRL_REG4, 0x00);//+-250dps
	Single_Write(CTRL_REG5, 0x00);
	//Delayms(100);
	BUF[0] = Single_Read(WHO_AM_I);
	printf( "WHO_AM_I: %x \r\n",BUF[0]);

//	NVIC_Configuration();	
//	EXTI_Configuration();
}	

/******************************************************************************
** ��������:  
** ��������: ��ȡL3G4200D���� 
** �䡡��  :  
** ��  ��  :  
** ˵  ��  �� ��
** ע  ��  �� ��
******************************************************************************/
/*void READ_L3G4200D(void)
{
   BUF[0]=Single_Read(OUT_X_L);
   BUF[1]=Single_Read(OUT_X_H);
   T_X=	(BUF[1]<<8)|BUF[0];
 


   BUF[2]=Single_Read(OUT_Y_L);
   BUF[3]=Single_Read(OUT_Y_H);
   T_Y=	(BUF[3]<<8)|BUF[2];
  

   BUF[4]=Single_Read(OUT_Z_L);
   BUF[5]=Single_Read(OUT_Z_H);
   T_Z=	(BUF[5]<<8)|BUF[4];

}*/

void READ_L3G4(int16_t *px,int16_t *py,int16_t *pz)
{
    int16_t data;
    data = Single_Read(OUT_X_L);
    data |= (int16_t)Single_Read(OUT_X_H)<<8;
    *px = data;

    data = Single_Read(OUT_Y_L);
    data |= (int16_t)Single_Read(OUT_Y_H)<<8;
    *py = data;

    data = Single_Read(OUT_Z_L);
    data |= (int16_t)Single_Read(OUT_Z_H)<<8;
    *pz = data;
}

