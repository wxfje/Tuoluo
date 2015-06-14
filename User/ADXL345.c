/******************************************************************************
** File      : ADXL345.c ���ٶ�
** Abstruct  : ADXL345 IIC��������/stm32,72MHz
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
//#define   DataPort P0    //LCD1602���ݶ˿�

#define ADXL345_WR_ADDR  0xA6      //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
#define ADXL345_RD_ADDR  0xA7                                //ALT  ADDRESS���Žӵ�ʱ��ַΪ0xA6���ӵ�Դʱ��ַΪ0x3A
//#define   SlaveAddress   0xa6//0x3a//

typedef unsigned char  BYTE;
typedef unsigned short WORD;

uint8_t ADXL345_Buffer[8];                         //�������ݻ�����
static uchar ge,shi,bai,qian,wan;           //��ʾ����
int  dis_data;                       //����

//void delay(unsigned int k);
//void InitLcd();                      //��ʼ��lcd1602
//void Init_ADXL345(void);             //��ʼ��ADXL345

//void WriteDataLCM(uchar dataW);
//void WriteCommandLCM(uchar CMD,uchar Attribc);
//void DisplayOneChar(uchar X,uchar Y,uchar DData);
//void conversion(uint temp_data);

//void  Single_Write_ADXL345(uchar REG_Address,uchar REG_data);   //����д������
//uchar Single_Read_ADXL345(uchar REG_Address);                   //������ȡ�ڲ��Ĵ�������
//void  Multiple_Read_ADXL345(void);                                  //�����Ķ�ȡ�ڲ��Ĵ�������
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
** ��������:
** ��������: ���ֽ�д��
** �䡡��  :
** ��  ��  :
** ˵  ��  �� ��
** ע  ��  �� ��
******************************************************************************/
static void Single_Write_ADXL345(uchar REG_Address,uchar REG_data)
{
    I2C_Start();                  //��ʼ�ź�
    I2C_SendByte(ADXL345_WR_ADDR);   //�����豸��ַ+д�ź�
    I2C_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ
    I2C_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ
    I2C_Stop();                   //����ֹͣ�ź�
}
/******************************************************************************
** ��������:
** ��������: ���ֽڶ�ȡ
** �䡡��  :
** ��  ��  :
** ˵  ��  �� ��
** ע  ��  �� ��
******************************************************************************/
static uint8_t Single_Read_ADXL345(uint8_t REG_Address)
{
    uchar REG_data;
    I2C_Start();                          //��ʼ�ź�
    I2C_SendByte(ADXL345_WR_ADDR);           //�����豸��ַ+д�ź�
    I2C_SendByte(REG_Address);                   //���ʹ洢��Ԫ��ַ����0��ʼ

    I2C_Start();                          //��ʼ�ź�
    I2C_SendByte(ADXL345_RD_ADDR);         //�����豸��ַ+���ź�
    REG_data=I2C_RadeByte();              //�����Ĵ�������
    I2C_NoAck();
    I2C_Stop();                           //ֹͣ�ź�
    return REG_data;
}


/******************************************************************************
** ��������:
** ��������: ��ʼ��ADXL345��������Ҫ��ο�pdf�����޸�
** �䡡��  :
** ��  ��  :
** ˵  ��  �� ��
** ע  ��  �� ��
******************************************************************************/
//void DelayMS(uint32_t n);
void Init_ADXL345( void )
{
    uint8_t a;
    Single_Write_ADXL345(0x31,0x0B);   //������Χ,����16g��13λģʽ
    Single_Write_ADXL345(0x2C,0x0c);   //�����趨Ϊ400hz �ο�pdf13ҳ
    Single_Write_ADXL345(0x2D,0x08);   //ѡ���Դģʽ   �ο�pdf24ҳ
    Single_Write_ADXL345(0x2E,0x80);   //ʹ�� DATA_READY �ж�
    Single_Write_ADXL345(0x1E,0x00);   //X ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
    Single_Write_ADXL345(0x1F,0x00);   //Y ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
    Single_Write_ADXL345(0x20,0x05);   //Z ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
    //DelayMS(10);
    a = Single_Read_ADXL345(0X00);
    printf("ADXL345 ID:0x%x",a);
}

/******************************************************************************
** ��������:
** ��������: ��������ADXL345�ڲ����ٶ����ݣ���ַ��Χ0x32~0x37
** �䡡��  :
** ��  ��  :
** ˵  ��  �� ��
** ע  ��  �� ��
******************************************************************************/
void Multiple_Read_ADXL345(void)
{
    uchar i;
    unsigned int data;
    I2C_Start();                          //��ʼ�ź�
    I2C_SendByte(ADXL345_WR_ADDR);           //�����豸��ַ+д�ź�
    I2C_SendByte(0x32);                   //���ʹ洢��Ԫ��ַ����0x32��ʼ

    I2C_Start();                          //��ʼ�ź�
    I2C_SendByte(ADXL345_RD_ADDR);         //�����豸��ַ+���ź�
    for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        ADXL345_Buffer[i] = I2C_RadeByte();          //BUF[0]�洢0x32��ַ�е�����
        if (i == 5)
        {
            I2C_NoAck();                 //���һ��������Ҫ��NOACK
        }
        else
        {
            I2C_Ack();                //��ӦACK
        }
    }
    I2C_Stop();                          //ֹͣ�ź�
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
    I2C_Start();                          //��ʼ�ź�
    I2C_SendByte(ADXL345_WR_ADDR);           //�����豸��ַ+д�ź�
    I2C_SendByte(0x32);                   //���ʹ洢��Ԫ��ַ����0x32��ʼ

    I2C_Start();                          //��ʼ�ź�
    I2C_SendByte(ADXL345_RD_ADDR);         //�����豸��ַ+���ź�
    for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        ADXL345_Buffer[i] = I2C_RadeByte();          //BUF[0]�洢0x32��ַ�е�����
        if (i == 5)
        {
            I2C_NoAck();                 //���һ��������Ҫ��NOACK
        }
        else
        {
            I2C_Ack();                //��ӦACK
        }
    }
    I2C_Stop();                          //ֹͣ�ź�
//    Delay5ms();

    *px = (ADXL345_Buffer[1]<<8)+ADXL345_Buffer[0];
    *py = (ADXL345_Buffer[3]<<8)+ADXL345_Buffer[2];
    *pz = (ADXL345_Buffer[5]<<8)+ADXL345_Buffer[4];
}
/*
//***********************************************************************
//��ʾx��
void display_x()
{   float temp;
    dis_data=(ADXL345_Buffer[1]<<8)+ADXL345_Buffer[0];  //�ϳ�����
    if(dis_data<0){
    dis_data=-dis_data;
    DisplayOneChar(2,0,'-');      //��ʾ��������λ
    }
    else DisplayOneChar(2,0,' '); //��ʾ�ո�

    temp=(float)dis_data*3.9;  //�������ݺ���ʾ,�鿼ADXL345�������ŵ�4ҳ
    conversion(temp);          //ת������ʾ��Ҫ������
    DisplayOneChar(0,0,'X');   //��0�У���0�� ��ʾX
    DisplayOneChar(1,0,':');
    DisplayOneChar(3,0,qian);
    DisplayOneChar(4,0,'.');
    DisplayOneChar(5,0,bai);
    DisplayOneChar(6,0,shi);
    DisplayOneChar(7,0,'g');
}

//***********************************************************************
//��ʾy��
void display_y()
{     float temp;
    dis_data=(ADXL345_Buffer[3]<<8)+ADXL345_Buffer[2];  //�ϳ�����
    if(dis_data<0){
    dis_data=-dis_data;
    DisplayOneChar(2,1,'-');      //��ʾ��������λ
    }
    else DisplayOneChar(2,1,' '); //��ʾ�ո�

    temp=(float)dis_data*3.9;  //�������ݺ���ʾ,�鿼ADXL345�������ŵ�4ҳ
    conversion(temp);          //ת������ʾ��Ҫ������
    DisplayOneChar(0,1,'Y');   //��1�У���0�� ��ʾy
    DisplayOneChar(1,1,':');
    DisplayOneChar(3,1,qian);
    DisplayOneChar(4,1,'.');
    DisplayOneChar(5,1,bai);
    DisplayOneChar(6,1,shi);
    DisplayOneChar(7,1,'g');
}

//***********************************************************************
//��ʾz��
void display_z()
{      float temp;
    dis_data=(ADXL345_Buffer[5]<<8)+ADXL345_Buffer[4];    //�ϳ�����
    if(dis_data<0){
    dis_data=-dis_data;
    DisplayOneChar(10,1,'-');       //��ʾ������λ
    }
    else DisplayOneChar(10,1,' ');  //��ʾ�ո�

    temp=(float)dis_data*3.9;  //�������ݺ���ʾ,�鿼ADXL345�������ŵ�4ҳ
    conversion(temp);          //ת������ʾ��Ҫ������
    DisplayOneChar(10,0,'Z');  //��0�У���10�� ��ʾZ
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
//******������********
//*********************************************************
void main()
{
  uchar devid;
  delay(500);                      //�ϵ���ʱ
  InitLcd();                      //Һ����ʼ��ADXL345
  Init_ADXL345();                 //��ʼ��ADXL345
  devid=Single_Read_ADXL345(0X00);//����������Ϊ0XE5,��ʾ��ȷ
  while(1)                         //ѭ��
  {
    Multiple_Read_ADXL345();       //�����������ݣ��洢��BUF��
    display_x();                   //---------��ʾX��
    display_y();                   //---------��ʾY��
    display_z();                   //---------��ʾZ��
    delay(200);                    //��ʱ
  }
} */
