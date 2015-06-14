#ifndef I2C_H
#define I2C_H
#include  <stdint.h> 
//#include "stm32f10x.h"
typedef enum {FALSE = 0, TRUE = !FALSE} bool;
//************************************
/*模拟IIC端口输出输入定义*/
#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_7
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7


extern void I2C_GPIO_Config(void);
extern bool I2C_Start(void);
extern void I2C_Stop(void);
extern void I2C_Ack(void);
extern void I2C_NoAck(void);
extern bool I2C_WaitAck(void);
extern bool I2C_SendByte(uint8_t SendByte);
extern unsigned char I2C_RadeByte(void);

#endif


