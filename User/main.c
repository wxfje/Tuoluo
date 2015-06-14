#include "stm32f10x.h"
#include <stdio.h>
#include "tft_lcd.h"
#include "L3G4200D.h"
#include "I2C.h"
#include "HMC5883.h"
#include "ADXL345.h"
#include "sensor_data.h"
//#include <GPS.h>
//volatile uint8_t RX_Receive[15] = {0};
void LED_GPIO_Configuration(void);
void USART1_Configuration(void);
void USART2_Configuration(void);
void DelayMS(uint32_t n);
void USART_SendByte( USART_TypeDef* USARTx, uint8_t Data);
uint8_t RunTimes = 0;

extern uint8_t Single_Read_ADXL345(uint8_t REG_Address);
volatile uint32_t system_time = 0;

int main(void)
{
	SystemInit();
	
	//LED_GPIO_Configuration();
	
	LCD_Init();		/* 显示器初始化 */
	if (SysTick_Config(SystemCoreClock / 1000))	   //SystemCoreClock
	{ 
		while (1);
	}
	/* 配置串口 */
	USART1_Configuration();
	USART_SendByte( USART1, 0xff);
	I2C_GPIO_Config();
	DelayMS(100);
	Init_L3G4200D();		     //初始化L3G4200D
	DelayMS(10);
	Init_HMC5883( );
	Init_ADXL345( );

    printf( "CPU Run Times: %d\r\n",RunTimes );
//	LCD_CLEAR(0,0,240,320,Green);
//	LCD_DrawCircle(120, 120, 100);
//	LCD_SetTextColor(Red);
//	LCD_DisplayString(10, 10, "GPS Test");
	while (1)
	{
		//printf( "CPU Run Times: %d\r\n",RunTimes );		
		//DelayMS(1000);
		//RunTimes++;
        if( system_time >= 2)
        {
            system_time = 0;
            //L3G4_data( );
            sensor_data_with();
        }
		
		//HMC5883_Read( );
		//Multiple_Read_ADXL345();
	}
}

void LED_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC
			| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG| RCC_APB2Periph_GPIOE,
				ENABLE);

	GPIO_SetBits(GPIOA,  GPIO_Pin_8 );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOE,  GPIO_Pin_2 );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);	


	GPIO_SetBits(GPIOC,  GPIO_Pin_7 );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	/*GPIO_SetBits(GPIOG,  GPIO_Pin_15 );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);*/	

}


void delay_us(__IO uint32_t nCount)
{
  uint8_t i ;
  for(; nCount != 0; nCount--)
    	for(i= 16; i != 0; i--);
}
void delay_ms(__IO uint32_t nCount)
{
  uint16_t i ;
  for(; nCount != 0; nCount--)
  	for(i= 9200; i != 0; i--);
}


