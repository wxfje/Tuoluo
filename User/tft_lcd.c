
#include "stm32f10x.h"
#include <stdio.h>
#include "systick.h"
#include "tft_lcd.h"

#include "fonts_ascii16x24.h"

#define TFT_Back_Open()		GPIO_SetBits(GPIOB, GPIO_Pin_15);
#define TFT_Back_Close()	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���  ��main.c
 * ����    ��LCD FSMC Ӧ�ú����⡣
 *           ʵ�ֵĹ��ܣ����������㡢���ߡ���ʾ���֡��ַ�����ͼƬ������         
 * ʵ��ƽ̨��Ұ��STM32������
 * ��汾  ��ST3.0.0
 *
 * ����    ��fire  QQ: 313303034 
 * ����    ��firestm32.blog.chinaunix.net
**********************************************************************************/
//#include "lcd.h"
#include "ascii.h"		// 12*6
#include "asc_font.h"	//12*8
//#include "sd_fs_app.h"


/* ѡ��BANK1-BORSRAM1 ���� TFT����ַ��ΧΪ0X60000000~0X63FFFFFF
 * FSMC_A11 ��LCD��DC(�Ĵ���/����ѡ��)��
 * 16 bit => FSMC[24:0]��ӦHADDR[25:1]
 * �Ĵ�������ַ = 0X60000000
 * RAM����ַ = 0X60020000 = 0X60000000+2^11*2 = 0X60000000 + 0X1000 = 0X60001000
 * ��ѡ��ͬ�ĵ�ַ��ʱ����ַҪ���¼��㡣
 */
#define Bank1_LCD_D    ((u32)0x60001000)    //Disp Data ADDR
#define Bank1_LCD_C    ((u32)0x60000000)	   //Disp Reg ADDR

#define RED	  0XF800
#define GREEN 0X07E0
#define BLUE  0X001F  
#define BRED  0XF81F
#define GRED  0XFFE0
#define GBLUE 0X07FF
#define BLACK 0X0000
#define WHITE 0XFFFF

u16 POINT_COLOR = BLACK;
uint16_t bkColor = White;
/*
 * ��������LCD_GPIO_Config
 * ����  ������FSMC����LCD��I/O
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����        
 */
static void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable the FSMC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    
    /* config lcd gpio clock base on FSMC */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
                         RCC_APB2Periph_GPIOB |RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    /* config tft back_light gpio base on the PT4101 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;		
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* config tft rst gpio */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ; 	 
    GPIO_Init(GPIOB, &GPIO_InitStructure);  		   
    
    /* config tft data lines base on FSMC
	 * data lines,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
	 */	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /* config tft control lines base on FSMC
	 * PD4-FSMC_NOE  :LCD-RD  
   * PD5-FSMC_NWE  :LCD-WR
	 * PD7-FSMC_NE1  :LCD-CS
   * PD11-FSMC_A16 :LCD-DC PG1 - A11
	 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 
    GPIO_Init(GPIOG, &GPIO_InitStructure);  
    
    /* tft control gpio init */	 
    GPIO_SetBits(GPIOB, GPIO_Pin_15);		 // ����	
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);	 //	RST = 1   
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		 // RD = 1  
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 // WR = 1 
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 //	CS = 1 
    
}

/*
 * ��������LCD_FSMC_Config
 * ����  ��LCD  FSMC ģʽ����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����        
 */

static void LCD_FSMC_Config(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p; 
    
    
    p.FSMC_AddressSetupTime = 0x02;	 //��ַ����ʱ��
    p.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ��
    p.FSMC_DataSetupTime = 0x05;		 //���ݽ���ʱ��
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;
    p.FSMC_AccessMode = FSMC_AccessMode_B;	 // һ��ʹ��ģʽB������LCD
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p; 
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
    /* Enable FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}

static void Delay(__IO u32 nCount)
{
    for(; nCount != 0; nCount--);
}


/*
 * ��������LCD_Rst
 * ����  ��LCD �����λ
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����        
 */


void delay_ms(__IO uint32_t nCount);
static void LCD_Rst(void)
{			
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	delay_ms(20);					   
    GPIO_SetBits(GPIOB, GPIO_Pin_14 );		 	 
	delay_ms(100);	
}		





/*
 * ��������LCD_WR_REG
 * ����  ��ILI9325 д�Ĵ�������
 * ����  ��-index �Ĵ���
 * ����  ���ڲ�����
 */
static __inline void LCD_WR_REG(u16 index)
{
    *(__IO u16 *) (Bank1_LCD_C) = index;
}



/*
 * ��������LCD_WR_REG
 * ����  ����ILI9325�Ĵ���д����
 * ����  ��-index �Ĵ���
 *         -val   д�������
 */
static __inline void LCD_WR_CMD(u16 index, u16 val)
{	
    *(__IO u16 *) (Bank1_LCD_C) = index;	
    *(__IO u16 *) (Bank1_LCD_D) = val;
}



/*
 * ��������LCD_WR_Data
 * ����  ����ILI9325 GRAM д������
 * ����  ��-val д�������,16bit        
 */
static __inline void LCD_WR_Data(unsigned int val)
{   
    *(__IO u16 *) (Bank1_LCD_D) = val; 	
}



/*
 * ��������LCD_WR_REG
 * ����  ���� ILI9325 RAM ����
 * ���  ����ȡ������,16bit *         
 */
static __inline u16 LCD_RD_data(void)
{
    u16 a = 0;
    a = (*(__IO u16 *) (Bank1_LCD_D)); 	//Dummy	
    a = *(__IO u16 *) (Bank1_LCD_D);     //L
    
    return(a);	
}



/*
 *  ��������RGB
 *  ���ܣ�RGB��ɫ��Ϻ���
 *  ���룺R 0-31,G 0-63,B 0-31
 *  �������Ϻ����ɫֵ
 */
static u16 RGB(u8 R,u8 G,u8 B)
{	
    return ( ( (u8)R<<11 ) | ( (u8)G<<5 & 0X07E0 ) | ( (u8)(B&0X1F) ) );
}


void Set_direction(u8 option)
{    
    switch(option)
    {
    	//��Һ�����Ľӿڶ�׼�Լ�
    	case 0:	  LCD_WR_CMD(0x0003,0X1038);break;		//��ֱɨ��
    	case 1:		LCD_WR_CMD(0x0003, 0X1030);break;	 	//ˮƽɨ�� 
    	default:	LCD_WR_CMD(0x0003, 0X1018);break;		//��ֱɨ�� 
    }    
}

/*----------------------------------------����ΪLCDӦ�ú���----------------------------------------------*/


/*
 * ��������LCD_open_windows
 * ����  ������(��x,yΪ������㣬��Ϊlen,��Ϊwid)
 * ����  ��-x -y -len -wid -rgb565
 * ���  ����
 * ����:
 *	   LCD_open_windows(0,0,120,160);	 	   //���Ͻ�  120*160
 *	   LCD_open_windows(0,160,120,160);	   //���Ͻ�	120*160
 *	   LCD_open_windows(120,0,120,160);	   //���½�	 120*160
 *	   LCD_open_windows(120,160,120,160);	   //���½�	120*160
 *
 */
void LCD_open_windows(u8 x,u16 y,u8 len,u16 wid)
{                    
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+len-1); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+wid-1);
    LCD_WR_CMD(0X20, x); 	
    LCD_WR_CMD(0X21, y); 
    
    LCD_WR_REG(0X22);
    
}


/*
 * ��������LCD_draw_rectangle
 * ����  ����x,yΪ������㣬�ڳ�len,��wid�ľ��������������ƶ���rgb565��ɫ
 * ����  ��-x -y -len -wid -rgb565
 * ���  ���� 
 * ����  :  
 *			LCD_draw_rectangle(0,0,120,160,0XF800);		���Ͻ�    	120*160	 	REG 
 *	   		LCD_draw_rectangle(0,160,120,160,0X07E0);	 ���Ͻ�		120*160		GREEN
 *	   		LCD_draw_rectangle(120,0,120,160,0X001F);	 ���½�		120*160	 	BLUE
 *	   		LCD_draw_rectangle(120,160,120,160,0XFF00);	  ���½�	120*160	   	orange
 */
void LCD_draw_rectangle(u8 x,u16 y,u8 len,u16 wid,u16 rgb565)
{                    
    u32 n, temp;
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+len-1); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+wid-1); 
    LCD_WR_REG(0X22);
    
    temp = (u32)len*wid;    
    for(n=0; n<temp; n++)
        LCD_WR_Data( rgb565 ); 
}



/*
 * ��������LCD_CLEAR
 * ����  ����x,yΪ������㣬�ڳ�len,��wid�ķ�Χ������
 * ����  ��-x -y -len -wid
 * ���  ���� 
 */
void LCD_CLEAR(u8 x,u16 y,u8 len,u16 wid,uint16_t Color)
{                    
    u32 n, temp;
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+len-1); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+wid-1); 
    LCD_WR_REG(0X22);
    
    temp = (u32)len*wid;    
    for(n=0; n<temp; n++)
        //LCD_WR_Data( RGB(31,0,0) );
		LCD_WR_Data( Color );  
}

void LCD_Clear(u8 x,u16 y,u8 len,u16 wid)
{                    
    u32 n, temp;
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+len-1); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+wid-1); 
    LCD_WR_REG(0X22);
    
    temp = (u32)len*wid;    
    for(n=0; n<temp; n++)
		LCD_WR_Data( bkColor );  
}


void LCD_WRITE_SATAT(void)
{
    LCD_WR_CMD(0x0050,0); 
    LCD_WR_CMD(0x0051, 239); 
    LCD_WR_CMD(0x0052, 0); 
    LCD_WR_CMD(0x0053, 319); 
    LCD_WR_REG(0X22);
    
}



void MY_LCD_WRITE(u16 rgb565)
{
    LCD_WR_Data(rgb565);
    
}



/*
 * ��������LCD_DrawPoint
 * ����  ��������(x,y)����һ����
 * ����  ��-x 0~239
           -y 0~319
 * ���  ���� 
 */
void LCD_DrawPoint(u8 x,u16 y)
{
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, 0x00EF);
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, 0x013F); 
    LCD_WR_CMD(0X20, x); 	
    LCD_WR_CMD(0X21, y); 
    LCD_WR_REG(0X22);
    
    LCD_WR_Data(POINT_COLOR);	
} 



/*
 * ��������LCD_DrawPoint
 * ����  ��������(x,y)����һ��ָ����ɫ�ĵ�
 * ����  ��-x 0~239
           -y 0~319
 * ���  ���� 
 */
void LCD_Draw_ColorPoint(u8 x,u16 y, u16 Color)
{
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, 0x00EF);
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, 0x013F); 
    LCD_WR_CMD(0X20, x); 	
    LCD_WR_CMD(0X21, y); 
    LCD_WR_REG(0X22);
    
    LCD_WR_Data(Color);	
} 


/*
 * ��������LCD_Show_ColorBar
 * ����  ����ʾ16���ʴ�
 * ����  ����
 * ���  ���� 
 */
void LCD_Show_ColorBar(void)
{
    u32 i, j, temp;
    for(i=0,temp=0; i<16; i++)	 		// ��ʾ16���ʴ�
    {		
        for(j=0; j<4800; j++)			 // 4800 = 320*/16*240
        {
            LCD_WR_Data(temp);
        }
        temp += 4369;							 // 4369 = 65535/15
    }
}



void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{	
	LCD_WR_CMD(0x0020,Xpos); /* Row */
	LCD_WR_CMD(0x0021,Ypos); /* Line */  
}


/*
 * ��������abs
 * ����  �������ֵ
 * ����  ��-res   32bit        
 * ���  ���� 
 */
static u32 abs(s32 res)
{
    if(res < 0)
        return -res;
    else
        return res;
}


/*
 * ��������LCD_DrawLine
 * ����  ����Һ������(x1,y1)Ϊ���,(x2,y2)Ϊ�յ㻭һֱ��
 * ����  ��-x1 0~239
           -y1 0~319
					 -x2 0~239
					 -y2 0~319       
 * ���  ���� 
 */  
void LCD_DrawLine(u8 x1, u16 y1, u8 x2, u16 y2)
{
    u16 x, y, t;
    if( (x1 == x2) && (y1 == y2) )
        LCD_DrawPoint(x1, y1);
    else if( abs(y2 - y1) > abs(x2 - x1) )			//б�ʴ���1 
    {
        if(y1 > y2) 
        {
            t = y1;
            y1 = y2;
            y2 = t; 
            t = x1;
            x1 = x2;
            x2 = t; 
        }
        for(y=y1; y<y2; y++)									//��y��Ϊ��׼ 
        {
            x = (u32)(y-y1)*(x2-x1) / (y2-y1) + x1;
            LCD_DrawPoint(x, y);  
        }
    }
    else     																	//б��С�ڵ���1 
    {
        if(x1 > x2)
        {
            t = y1;
            y1 = y2;
            y2 = t;
            t = x1;
            x1 = x2;
            x2 = t;
        }   
        for(x=x1; x<x2; x++)									//��x��Ϊ��׼ 
        {
            y = (u32)(x-x1)*(y2-y1) / (x2-x1) + y1;
            LCD_DrawPoint(x, y); 
        }
    } 
}



/*
 * ������  ��LCD_ShowChar
 * ����    ����Һ������(x,y)��,��ʾһ��12*6��С���ַ�
 * ����    ��-x 0~(239-6)
             -y 0~(319-12)
						 -mode 0 �޵���Ч��
						       1 �е���Ч��
      		   -acsii Ҫд����ַ�
 * ���    ����
 * ���÷�����TFT_ShowChar(10, 10, 'A');
 *           �ڲ����� 
 */ 
static void LCD_ShowChar(u8 x, u16 y, u8 mode, u8 acsii)	//����
{       
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304 
    u8 temp, t, pos;  
    u16 R_dis_mem;    
    if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
        return;
    Set_direction(0);		 													//ɨ�跽������
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+11); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+5);	 
    LCD_WR_CMD(0X20, x);
    LCD_WR_CMD(0X21, y);
    LCD_WR_REG(0X22);    
    acsii = acsii - ' ';													//�õ�ƫ�ƺ��ֵ
    for(pos=0; pos<12; pos++)
    {
        temp = asc2_1206[acsii][pos];
        for(t=0; t<6; t++)											// ��λ��ʼ,��������λ
        {                 
            if(temp & 0x01)
                LCD_WR_Data(POINT_COLOR);
            else 	
            {
                if ( mode == 0 )
                    LCD_WR_Data(0xffff);				//��ɫ
                else if ( mode == 1 )
                {
                    /* ����Ч�� */
                    LCD_WR_CMD(0X20, x+pos);		//ˮƽ��ʾ����ַ
                    LCD_WR_CMD(0X21, y+t);			//��ֱ��ʾ����ַ
                    LCD_WR_REG(0X22); 
                    
                    R_dis_mem =LCD_RD_data();
                    LCD_WR_Data(R_dis_mem);
                }				
            }   
            temp >>= 1; 
        }
    }
    
    
}


static void LCD_ShowChar2(u8 x, u16 y, u8 mode, u8 acsii)	//����
{       
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304 
    u8 temp, t, pos;  
    u16 R_dis_mem;    
    if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
        return;
    Set_direction(1);																	//ɨ�跽������
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+5); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+11);	 
    LCD_WR_CMD(0X20, x);
    LCD_WR_CMD(0X21, y);
    LCD_WR_REG(0X22);    
    acsii = acsii - ' ';															//�õ�ƫ�ƺ��ֵ
    for(pos=0; pos<12; pos++)
    {
        temp = asc2_1206[acsii][12-pos];
        for(t=0; t<6; t++)														// ��λ��ʼ,��������λ
        {                 
            if(temp & 0x01)
                LCD_WR_Data(POINT_COLOR);
            else 	
            {
                if ( mode == 0 )
                    LCD_WR_Data(0xffff);							//��ɫ
                else if ( mode == 1 )
                {
                    /* ����Ч�� */				
                    LCD_WR_CMD(0X20, x+t);					//ˮƽ��ʾ����ַ
                    LCD_WR_CMD(0X21, y+pos);				//��ֱ��ʾ����ַ
                    LCD_WR_REG(0X22); 
                    
                    R_dis_mem =LCD_RD_data();
                    LCD_WR_Data(R_dis_mem);
                }				
            }   
            temp >>= 1; 
        }
    }
    
    
}


/*
 * ������  ��LCD_Show_8x16_Char
 * ����    ����Һ������(x,y)��,��ʾһ��12*6��С���ַ�
 * ����    ��-x 0~(239-6)
             -y 0~(319-12)
						 -mode 0 �޵���Ч��
						       1 �е���Ч��
      		   -acsii Ҫд����ַ�
 * ���    ����
 * ���÷�����TFT_ShowChar(10, 10, 'A');
 *           �ڲ����� 
 */ 
void LCD_Show_8x16_Char(u8 x, u16 y, u8 mode, u8 acsii)//����
{       
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304 
    u8 temp, t, pos;  
    u16 R_dis_mem;     
    if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
        return;
    Set_direction(0);														//ɨ�跽������
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+15); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+7);	 
    LCD_WR_CMD(0X20, x);
    LCD_WR_CMD(0X21, y);
    LCD_WR_REG(0X22);    
    
    
    for (pos=0;pos<16;pos++)
    {
        temp=ascii_8x16[((acsii-0x20)*16)+pos];
        
	for(t=0; t<8; t++)
        {
            
            if(temp & 0x80)
                LCD_WR_Data(POINT_COLOR);
            else 	
            {
                if ( mode == 0 )
                    LCD_WR_Data(0xffff);					//��ɫ
                else if ( mode == 1 )
                {
                    LCD_WR_CMD(0X20, x+pos);			//ˮƽ��ʾ����ַ
                    LCD_WR_CMD(0X21, y+t);				//��ֱ��ʾ����ַ
                    LCD_WR_REG(0X22); 
                    R_dis_mem = LCD_RD_data();
                    LCD_WR_Data(R_dis_mem);
                }				
            } 
            
            temp <<= 1;	
            
        }
        
        
    }
    
}





void LCD_Show_8x16_Char2(u8 x, u16 y, u8 mode, u8 acsii) //����
{       
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304 
    u8 temp, t, pos;  
    u16 R_dis_mem;     
    if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
        return;
    Set_direction(1);		 													//ɨ�跽������
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+7); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+15);	 
    LCD_WR_CMD(0X20, x);
    LCD_WR_CMD(0X21, y);
    LCD_WR_REG(0X22);    
    
    
    for (pos=0;pos<16;pos++)
    {
        temp=ascii_8x16[((acsii-0x20)*16)+16-pos];
        
	for(t=0; t<8; t++)
        {
            
            if(temp & 0x80)
                LCD_WR_Data(POINT_COLOR);
            else 	
            {
                if ( mode == 0 )
                    LCD_WR_Data(0xffff);					//��ɫ
                else if ( mode == 1 )
                {
                    LCD_WR_CMD(0X20, x+t);				//ˮƽ��ʾ����ַ
                    LCD_WR_CMD(0X21, y+pos);			//��ֱ��ʾ����ַ
                    
                    LCD_WR_REG(0X22); 
                    R_dis_mem = LCD_RD_data();
                    LCD_WR_Data(R_dis_mem);
                }				
            } 
            
            temp <<= 1;	
            
        }
        
        
    }
    
}



/*
 * ������  ��LCD_Show_8x16_String
 * ����    ����Һ������(x,y)��,��ʾһ��16*8��С���ַ���
 * ����    ��-x 0~(239-6)
             -y 0~(319-12)
						 -mode 0 �޵���Ч��
						       1 �е���Ч��
      		   -acsii Ҫд����ַ�
 * ���    ����
 * ���÷�����LCD_Show_8x16_String(130, 130, 1, "Runing");
 *          �ⲿ����
 */ 
void LCD_Show_8x16_String(u16 y, u8 x, u8 mode, u8 *str)	 //����
{
    u8* tmp_str = str;
    u8 Tmp_y=y;
    while(*tmp_str != '\0')
    {
        LCD_Show_8x16_Char(x,Tmp_y,mode,*tmp_str);		
        tmp_str ++ ;
        Tmp_y += 8 ;
    }
    
}



/*
 * ������  ��LCD_Show_8x16_String
 * ����    ����Һ������(x,y)��,��ʾһ��16*8��С���ַ���
 * ����    ��-x 0~(239-8)
             -y 0~(319-16)
						 -mode 0 �޵���Ч��
						       1 �е���Ч��
      		   -acsii Ҫд����ַ�
 * ���    ����
 * ���÷�����LCD_Show_8x16_String(130, 130, 1, "Runing");
 *          �ⲿ����
 */
void LCD_Show_8x16_String2(u8 x, u16 y, u8 mode, u8 *str)	 //����
{
    u8* tmp_str = str;
    u8 Tmp_y=y;
    while(*tmp_str != '\0')
    {
        LCD_Show_8x16_Char2(x,Tmp_y,mode,*tmp_str);		
        tmp_str ++ ;
        x += 8 ;
    }
    
}



//m^n����
static u32 mypow(u8 m, u8 n)
{
    u32 result = 1;	 
    while( n -- )
        result *= m;    
    return result;
}



/*
 * ������  ��LCD_ShowNum
 * ����    ����Һ������(x,y)��,��ʾһ��12*6��С����
 * ����    ��-x 0~(239-6)
 *           -y 0~(319-12)
 *					 -mode 0 �޵���Ч��
 *						     1 �е���Ч��
 *     		   -num 0~65535
 * ���    ����
 * ���÷�����LCD_ShowNum(10, 10, 0, 65535); 
 */  
void LCD_ShowNum(u8 x,u16 y, u8 mode, u32 num)
{      
    u32 res;   	   
    u8 t = 0, t1 = 0;   
    res = num;
    if( !num )
        LCD_ShowChar(x, y, mode, '0');
    while( res )  //�õ����ֳ���
    {
        res/=10;
        t++;
    }
    t1 = t;
    while(t)	//��ʾ����
    {
        res = mypow(10, t-1); 	 
        LCD_ShowChar(x, y+(t1-t)*6, mode, (num/res)%10+'0');
        t -- ;
    }				     
} 



/*
 * ������  ��LCD_ShowString
 * ����    ����Һ���д�����(x,y)��ʼ��ʾ�ַ���
 * ����    �� -y	0~(319-12)
 *						-x 	0~(239-6)
 *           -mode 0 �޵���Ч��
 *                 1 �е���Ч��
 *     		   -p ָ��Ҫд��Һ�����ַ���
 * ���    ����
 * ���÷�����LCD_ShowString(10, 10, 0, ��I LOVE STM32��); 
 */
void LCD_ShowString(u16 y,u8 x, u8 mode, const u8 *p)	 //����
{         
    while(*p != '\0')
    {       
        if(x > MAX_CHAR_POSX) 
        {	// ����
            x = 0;
            y += 12;
        }
        if(y > MAX_CHAR_POSY) 
        {	// һ��
            y = x = 0;
            LCD_Clear(0,0,240,320);
        }
        
        LCD_ShowChar(x ,y, mode, *p);
        y += 6;
        p ++ ;
    }  
}

/*
 * ������  ��LCD_ShowString
 * ����    ����Һ���д�����(x,y)��ʼ��ʾ�ַ���
 * ����    ��-x 0~(239-6)
 *           -y 0~(319-12)
 *           -mode 0 �޵���Ч��
 *                 1 �е���Ч��
 *     		   -p ָ��Ҫд��Һ�����ַ���
 * ���    ����
 * ���÷�����LCD_ShowString(10, 10, 0, ��I LOVE STM32��); 
 */
void LCD_ShowString2(u8 x, u16 y, u8 mode, const u8 *p)	//����
{         
    while(*p != '\0')
    {       
        if(x > MAX_CHAR_POSX) 
        {	// ����
            x = 0;
            y += 12;
        }
        if(y > MAX_CHAR_POSY) 
        {	// һ��
            y = x = 0;
            LCD_Clear(0,0,240,320);
        }
        
        LCD_ShowChar2(x ,y, mode, *p);
        x +=6;
        p ++ ;
    }  
}


/******************************************************************************
* Function Name  : PutChinese1
* Description    : ��Lcd��������λ����ʾһ��������
* Input          : - Xpos: ˮƽ���� 
*                  - Ypos: ��ֱ����  
*				   - str: ��ʾ��������
*				   - Color: �ַ���ɫ   
*				   - bkColor: ������ɫ 
* Output         : None
* Return         : None
* example		 : PutChinese1(200,100,"��",0,0xffff);
* Attention		 : �ڲ�����
*******************************************************************************/
void PutChinese11(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor)	//����
{    
    uint8_t i,j;
    uint8_t buffer[32];
    uint16_t tmp_char=0;
    
//    GetGBKCode_from_sd(buffer,str);	  /* ȡ��ģ���� */
    for (i=0;i<16;i++)
    {
        tmp_char=buffer[i*2];
				tmp_char=(tmp_char<<8);
				tmp_char|=buffer[2*i+1];

        for (j=0;j<16;j++)
        {
            if ( (tmp_char >> 15-j) & 0x01 == 0x01)
            {
                LCD_Draw_ColorPoint(Ypos+i,Xpos+j,Color);
            }
            else
            {
                LCD_Draw_ColorPoint(Ypos+i,Xpos+j,bkColor);
            }
        }
    }
    
}



/******************************************************************************
* Function Name  : PutChinese12
* Description    : ��Lcd��������λ����ʾһ��������
* Input          : - Xpos: ˮƽ���� 
*                  - Ypos: ��ֱ����  
*				   - str: ��ʾ��������
*				   - Color: �ַ���ɫ   
*				   - bkColor: ������ɫ 
* Output         : None
* Return         : None
* example		 : PutChinese1(200,100,"��",0,0xffff);
* Attention		 : �ڲ�����
*******************************************************************************/
void PutChinese12(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor)//����
{    
    uint8_t i,j;
    uint8_t buffer[32];
    uint16_t tmp_char=0;
    
    Set_direction(1); 
//    GetGBKCode_from_sd(buffer,str);	 /* ȡ��ģ���� */
    
    for (i=0;i<16;i++)
    {
        tmp_char=buffer[i*2];
				tmp_char=(tmp_char<<8);
				tmp_char|=buffer[2*i+1];

        for (j=0;j<16;j++)
        {
            if ( (tmp_char >> 15-j) & 0x01 == 0x01)
            {
                LCD_Draw_ColorPoint(Xpos+j,Ypos+16-i,Color);
            }
            else
            {
                LCD_Draw_ColorPoint(Xpos+j,Ypos+16-i,bkColor);
            }
        }
    }    
}



/******************************************************
 * ��������PutChinese21
 * ����  ����ʾ���������ַ���
 * ����  : pos: 0~(319-16)
 *         Ypos: 0~(239-16)
 *				 str: �����ַ�����ַ
 *				 Color: �ַ���ɫ   
 *				 mode: 0--���ֱ���ɫΪ��ɫ   
 *						   1--�������� 
 * ���  ����
 * ����  ��PutChinese21(200,100,"��",0,0);
 * ע��	 ������������1�ĺ����ַ�������ʾ����ضϣ�ֻ��ʾ��ǰ��һ������
 *********************************************************/    
void PutChinese21(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode) //����
{
    uint8_t i,j;
    uint8_t buffer[32];
    uint16_t tmp_char=0;
    Set_direction(0);   
//    GetGBKCode_from_sd(buffer,str); /* ȡ��ģ���� */
    
    for (i=0;i<16;i++)
    {
        tmp_char=buffer[i*2];
				tmp_char=(tmp_char<<8);
				tmp_char|=buffer[2*i+1];
        for (j=0;j<16;j++)
        {            
            if ( (tmp_char >> 15-j) & 0x01 == 0x01)
            {
                LCD_Draw_ColorPoint(Ypos+i,Xpos+j,Color);
            }
            else
            {
                if ( mode == 0 )
                    LCD_Draw_ColorPoint(Ypos+i,Xpos+j,0xffff);	//����ɫ��ʾ��ɫ
                else if ( mode == 1 )
                {
                    //��д��
                }               
                
            }
        }
    }   
}




/******************************************************************************
* Function Name  : PutChinese22
* Description    : ��Lcd��������λ����ʾһ��������
* Input          : - Xpos: ˮƽ���� 
*                  - Ypos: ��ֱ����  
*				   - str: ��ʾ��������
*				   - Color: �ַ���ɫ   
*				   - mode: ģʽѡ��		0--����ɫΪ��ɫ   1--��������
* Output         : None
* Return         : None
* example		 : PutChinese2(200,100,"��",0,1);
* Attention		 : �ڲ�����
*******************************************************************************/
void PutChinese22(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode) //����
{
    
    
    uint8_t i,j;
    uint8_t buffer[32];
    uint16_t tmp_char=0;
    Set_direction(1);																		
//    GetGBKCode_from_sd(buffer,str); /* ȡ��ģ���� */
    
    for (i=0;i<16;i++)
    {
        tmp_char=buffer[i*2];
        tmp_char=(tmp_char<<8);
        tmp_char|=buffer[2*i+1];
        for (j=0;j<16;j++)
        {
            
            if ( (tmp_char >> 15-j) & 0x01 == 0x01)
            {
                LCD_Draw_ColorPoint(Xpos+j,Ypos+16-i,Color);
            }
            else
            {
                if ( mode == 0 )
                    LCD_Draw_ColorPoint(Ypos+j,Xpos+i,0xffff);	//����ɫ��ʾ��ɫ
                else if ( mode == 1 )
                {
                    //��д��
                }	
                
                
            }
        }
    }
    
    
}




/******************************************************************************
* Function Name  : PutChinese_strings1
* Description    : ��Lcd��������λ����ʾһ��������
* Input          : - Xpos: 0-(319-16) 	 
*                  - Ypos: 0-(239-16)  
*				   - str: ��ʾ�������ַ���
*				   - Color: �ַ���ɫ   
*				   - bkColor: ������ɫ 
* Output         : None
* Return         : None
* example		 : PutChinese_strings1(200,100,"����",0,0xffff);
				   PutChinese_strings1(200,150,"����",0xff,0xffff);
* Attention		 : �ⲿ����
*******************************************************************************/
void PutChinese_strings11(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor)	//����
{
    
    uint16_t Tmp_x, Tmp_y;
    uint8_t *tmp_str=str;
    
    Tmp_x = Xpos;
    Tmp_y = Ypos;
    
    
    while(*tmp_str != '\0')
    {
        PutChinese11(Tmp_x,Tmp_y,tmp_str,Color,bkColor);
        
        tmp_str += 2 ;
        Tmp_x += 16 ;	
    }
    
    
    
}



/******************************************************************************
* Function Name  : PutChinese_strings12
* Description    : ��Lcd��������λ����ʾһ��������
* Input          : - Xpos: 0-(239-16)
*                  - Ypos: 0-(319-16)
*				   - str: ��ʾ�������ַ���
*				   - Color: �ַ���ɫ   
*				   - bkColor: ������ɫ 
* Output         : None
* Return         : None
* example		 : PutChinese_strings1(200,100,"����",0,0xffff);
				   PutChinese_strings1(200,150,"����",0xff,0xffff);
* Attention		 : �ⲿ����
*******************************************************************************/
void PutChinese_strings12(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor)	//����
{
    
    uint16_t Tmp_x, Tmp_y;
    uint8_t *tmp_str=str;
    
    Tmp_x = Xpos;
    Tmp_y = Ypos;
    
    
    while(*tmp_str != '\0')
    {
        PutChinese12(Tmp_x,Tmp_y,tmp_str,Color,bkColor);
        
        tmp_str += 2 ;
        Tmp_x += 16 ;	
    }
    
}



/******************************************************
 * ��������PutChinese_strings21
 * ����  ����ʾ�����ַ���
 * ����  : pos: 0~(319-16)
 *         Ypos: 0~(239-16)
 *				 str: �����ַ�����ַ
 *				 Color: �ַ���ɫ   
 *				 mode: 0--���ֱ���ɫΪ��ɫ   
 *						   1--�������� 
 * ���  ����
 * ����  ��PutChinese_strings2(200,100,"����",0,0);
 * ע��	 ����
 *********************************************************/    
void PutChinese_strings21(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode)	//����
{
    
    uint16_t Tmp_x, Tmp_y;
    uint8_t *tmp_str=str;
    Tmp_x = Xpos;
    Tmp_y = Ypos;
    
    while(*tmp_str != '\0')
    {
        PutChinese21(Tmp_x,Tmp_y,tmp_str,Color,mode);
        
        tmp_str += 2 ;
        Tmp_x += 16 ;	
    }       
}



/******************************************************************************
* Function Name  : PutChinese_strings22
* Description    : ��Lcd��������λ����ʾһ��������	 
* Input          : - Xpos: 0~(239-16)
*                  - Ypos: 0~(319-16)
*				   - str: ��ʾ�������ַ���
*				   - Color: �ַ���ɫ   
*				   - mode: ����ģʽѡ��  0--���ֱ���ɫΪ��ɫ   1--�������� 
* Output         : None
* Return         : None
* example		 : PutChinese_strings2(200,100,"����",0,0);
				   		 PutChinese_strings2(200,150,"����",0,1);
* Attention		 : �ⲿ����
*/
void PutChinese_strings22(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode) //����
{    
    uint16_t Tmp_x, Tmp_y;
    uint8_t *tmp_str=str;
    Tmp_x = Xpos;
    Tmp_y = Ypos;
    
    while(*tmp_str != '\0')
    {
        PutChinese22(Tmp_x,Tmp_y,tmp_str,Color,mode);
        
        tmp_str += 2 ;
        Tmp_x += 16 ;	
    }    
}

/* 
 * ��������Put_ascii_chinese_string
 * ����  ����Ӣ�Ļ����ʾ��Ӣ�Ĵ�СΪ16*8
 * ����  ��-Xpos  x ����
 *         -Ypos  y ����
 *         -str   ������Ϣָ��
 *         -Color ������ɫ
 *         -mode  ��ʾģʽ��0 ���ӣ�1 ����
 */
void Put_ascii_chinese_string(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode)
{
    uint16_t Tmp_x, Tmp_y;
    uint8_t *tmp_str=str;
    Tmp_x = Xpos;
    Tmp_y = Ypos;
    
    while(*tmp_str != '\0')
    {
        if(*tmp_str<125)
				{
					LCD_Show_8x16_Char2(Tmp_x,Tmp_y,mode, *tmp_str);
					tmp_str++ ;
					Tmp_x	+= 8 ;
				}
				else
				{
					if(*tmp_str==163)																//163-172 �����Ķ���	 163-187�����ķֺ�
					{
						LCD_Show_8x16_Char2(Tmp_x,Tmp_y,mode, ' ');		//��Ϊ��ʾ1���ո�
						tmp_str += 2 ;
						Tmp_x	+= 8 ;

					}
					PutChinese22(Tmp_x,Tmp_y,tmp_str,Color,mode);
          tmp_str += 2 ;
        	Tmp_x += 16 ;						
        }
    }
}

/*----------------------------------BMPӦ�ú��� start-------------------------------*/
/*
 * ������  ��LCD_ShowBmp
 * ����    ����ʾlenth*wide��16λ���ͼƬ
 * ����    ��-x 
 *           -y 
 *     		   -p ͼƬ�׵�ַ
 * ���    ����
 * ����    ��LCD_ShowBmp(0, 0, 240, 320, (u8 *)p); 
 *           һ����240*320��16bit���ͼ��
 *					 �ⲿ����	   
 */
void LCD_ShowBmp(u8 x, u16 y, u8 lenth, u16 wide, const u8 *p)
{      
    u32 n=0 , size=0, temp=0;
    
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, (u16)x+lenth-1); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+wide-1);
    LCD_WR_CMD(0x0020, x);			 
    LCD_WR_CMD(0x0021, y); 
    LCD_WR_REG(0X22);
    
    size = (u32) lenth * wide * 2; 	 // 16bit���,һ�����������ֽ�
    
    while(n < size) 
    {		
        temp = (uint16_t)(p[n]<<8) + p[n+1];		
        LCD_WR_Data( temp );
        n = n + 2; 
    }
}

void MY_LCD_WR_Data(unsigned int val)
{   
    *(__IO u16 *) (Bank1_LCD_D) = val; 	
}

void bmp(u16 x, u16 y, u16 width, u16 height)
{    
    Set_direction(0);
    LCD_WR_CMD(0X20, x);			//ˮƽ��ʾ����ַ
    LCD_WR_CMD(0X21, y);			//��ֱ��ʾ����ַ
    
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0052, y); 
    
    LCD_WR_CMD(0x0051, height-1+x);//240	
    LCD_WR_CMD(0x0053, width-1+y); //320
    
    LCD_WR_CMD(0x0003, 0X1028);	
    
    LCD_WR_REG(0X22);      
}

void bmp2()
{
    
    Set_direction(0);
    LCD_WR_REG(0X22);
    
}

void bmp3(void)
{
    Set_direction(0);
}


u16 bmp4(u16 x, u16 y)
{
    
    LCD_WR_CMD(0X20, x);	
    LCD_WR_CMD(0X21, y);
 		LCD_WR_REG(0X22);
		
		return  LCD_RD_data(); 
	 //	return (LCD_BGR2RGB(LCD_RD_data())) ;
       
}

/*******************************************************************************
*	������: LCD_DrawCircle
*	��  ��: Xpos ��X����
*			Radius ��Բ�İ뾶
*	��  ��: ��
*	��  ��: ��LCD�ϻ�һ��Բ
*/
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	int32_t  D;			/* Decision Variable */
	uint32_t  CurX;		/* ��ǰ X ֵ */
	uint32_t  CurY;		/* ��ǰ Y ֵ */

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		LCD_Draw_ColorPoint(Xpos + CurX, Ypos + CurY, bkColor);
		LCD_Draw_ColorPoint(Xpos + CurX, Ypos - CurY, bkColor);
		LCD_Draw_ColorPoint(Xpos - CurX, Ypos + CurY, bkColor);
		LCD_Draw_ColorPoint(Xpos - CurX, Ypos - CurY, bkColor);

		LCD_Draw_ColorPoint(Xpos + CurY, Ypos + CurX, bkColor);
		LCD_Draw_ColorPoint(Xpos + CurY, Ypos - CurX, bkColor);
		LCD_Draw_ColorPoint(Xpos - CurY, Ypos + CurX, bkColor);
		LCD_Draw_ColorPoint(Xpos - CurY, Ypos - CurX, bkColor);
		/*LCD_SetCursor(Xpos + CurX, Ypos + CurY);
		LCD_WR_Data(bkColor);

		LCD_SetCursor(Xpos + CurX, Ypos - CurY);
		LCD_WR_Data(bkColor);

		LCD_SetCursor(Xpos - CurX, Ypos + CurY);
		LCD_WR_Data(bkColor);

		LCD_SetCursor(Xpos - CurX, Ypos - CurY);
		LCD_WR_Data(bkColor);

		LCD_SetCursor(Xpos + CurY, Ypos + CurX);
		LCD_WR_Data(bkColor);

		LCD_SetCursor(Xpos + CurY, Ypos - CurX);
		LCD_WR_Data(bkColor);

		LCD_SetCursor(Xpos - CurY, Ypos + CurX);
		LCD_WR_Data(bkColor);

		LCD_SetCursor(Xpos - CurY, Ypos - CurX);
		LCD_WR_Data(bkColor);	*/

		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}


/*----------------------------------BMPӦ�ú��� end-------------------------------*/

/*
 * ��������LCD_Init
 * ����  ��LCD ���� I/O ��ʼ��
 *         LCD FSMC ��ʼ��
 *         LCD ������ ILI9325C ��ʼ��         
 * ����  ��NONE
 * ���  ��NONE
 * ����  ���ⲿ����
 */
void USART_SendWord( USART_TypeDef* USARTx, uint8_t Data);
 void LCD_Init(void)
{
	//GPIO_InitTypeDef GPIO_InitStructure; 
	uint32_t i = 0;   
    LCD_GPIO_Config();
    LCD_FSMC_Config();

	TFT_Back_Close();
	delay_ms(20);
	TFT_Back_Open();		
    LCD_Rst();

	LCD_WR_REG(0x00);
	i = LCD_RD_data();


	//LCD_FSMC_Config();	    
    LCD_WR_CMD(0x00E3, 0x3008); // Set internal timing
    LCD_WR_CMD(0x00E7, 0x0012); // Set internal timing
    LCD_WR_CMD(0x00EF, 0x1231); // Set internal timing
    LCD_WR_CMD(0x0000, 0x0001); // Start Oscillation
    
    LCD_WR_CMD(0x0001, 0x0100); // set SS and SM bit s720->s1(���Ͻ�Ϊ����ԭ��)
    LCD_WR_CMD(0x0060, 0xA700);	// R01�е�SS��R60�е�GSһ���������ԭ��
    
    	//LCD_WR_CMD(0x0001, 0x0100);   // set SS and SM bit s720->s1
    //LCD_WR_CMD(0x0001, 0x0000);   // set SS and SM bit s1->s720		(���½�Ϊ����ԭ��)
    //LCD_WR_CMD(0x0060, 0x2700); // R01�е�SS��R60�е�GSһ���������ԭ��
    
    LCD_WR_CMD(0x0002, 0x0700); // set 1 line inversion
    /*����ɨ�跽��*/
    //	LCD_WR_CMD(0x0003, 0X1018); // set GRAM write direction and BGR=0,262K colors,1 transfers/pixel.
    LCD_WR_CMD(0x0003, 0X1030); 
    
    LCD_WR_CMD(0x0004, 0x0000); // Resize register
    LCD_WR_CMD(0x0008, 0x0202); // set the back porch and front porch
    LCD_WR_CMD(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
    LCD_WR_CMD(0x000A, 0x0000); // FMARK function
    LCD_WR_CMD(0x000C, 0x0000); // RGB interface setting
    LCD_WR_CMD(0x000D, 0x0000); // Frame marker Position
    LCD_WR_CMD(0x000F, 0x0000); // RGB interface polarity
    //Power On sequence 
    LCD_WR_CMD(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_WR_CMD(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
    LCD_WR_CMD(0x0012, 0x0000); // VREG1OUT voltage
    LCD_WR_CMD(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
    Delay(200); // Dis-charge capacitor power voltage
    LCD_WR_CMD(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_WR_CMD(0x0011, 0x0227); // R11h=0x0221 at VCI=3.3V, DC1[2:0], DC0[2:0], VC[2:0]
    Delay(50); // Delay 50ms
    LCD_WR_CMD(0x0012, 0x001C); // External reference voltage= Vci;
    Delay(50); // Delay 50ms
    LCD_WR_CMD(0x0013, 0x1800); // R13=1200 when R12=009D;VDV[4:0] for VCOM amplitude
    LCD_WR_CMD(0x0029, 0x001C); // R29=000C when R12=009D;VCM[5:0] for VCOMH
    LCD_WR_CMD(0x002B, 0x000D); // Frame Rate = 91Hz
    Delay(50); // Delay 50ms
    LCD_WR_CMD(0x0020, 0x0000); // GRAM horizontal Address
    LCD_WR_CMD(0x0021, 0x0000); // GRAM Vertical Address
    //Adjust the Gamma Curve
    LCD_WR_CMD(0x0030, 0x0007);
    LCD_WR_CMD(0x0031, 0x0302);
    LCD_WR_CMD(0x0032, 0x0105);
    LCD_WR_CMD(0x0035, 0x0206);
    LCD_WR_CMD(0x0036, 0x0808);
    LCD_WR_CMD(0x0037, 0x0206);
    LCD_WR_CMD(0x0038, 0x0504);
    LCD_WR_CMD(0x0039, 0x0007);
    LCD_WR_CMD(0x003C, 0x0105);
    LCD_WR_CMD(0x003D, 0x0808);
    //Set GRAM area
    LCD_WR_CMD(0x0050, 0x0000); // Horizontal GRAM Start Address
    LCD_WR_CMD(0x0051, 0x00EF); // Horizontal GRAM End Address
    LCD_WR_CMD(0x0052, 0x0000); // Vertical GRAM Start Address
    LCD_WR_CMD(0x0053, 0x013F); // Vertical GRAM Start Address
    
    LCD_WR_CMD(0x0060, 0xA700); // Gate Scan Line
    //	LCD_WR_CMD(0x0060, 0x2700);		// ��R01�е�SSһ����������ԭ��
    
    LCD_WR_CMD(0x0061, 0x0001); // NDL,VLE, REV
    LCD_WR_CMD(0x006A, 0x0000); // set scrolling line
    //Partial Display Control
    LCD_WR_CMD(0x0080, 0x0000);
    LCD_WR_CMD(0x0081, 0x0000);
    LCD_WR_CMD(0x0082, 0x0000);
    LCD_WR_CMD(0x0083, 0x0000);
    LCD_WR_CMD(0x0084, 0x0000);
    LCD_WR_CMD(0x0085, 0x0000);
    //Panel Control
    LCD_WR_CMD(0x0090, 0x0010);
    LCD_WR_CMD(0x0092, 0x0000);
    LCD_WR_CMD(0x0093, 0x0003);
    LCD_WR_CMD(0x0095, 0x0110);
    LCD_WR_CMD(0x0097, 0x0000);
    LCD_WR_CMD(0x0098, 0x0000);
    
    // important
    LCD_WR_CMD(0x0007, 0x0133); // 262K color and display ON
    


	Set_direction(1);        /* ����LCD��ɨ�跽������Ϊˮƽɨ��  */ 
    // clear the panel	
    LCD_WR_CMD(0x0050, 0);       // Horizontal GRAM Start Address
    LCD_WR_CMD(0x0051, 239);     // Horizontal GRAM End Address
    LCD_WR_CMD(0x0052, 0);       // Vertical GRAM Start Address
    LCD_WR_CMD(0x0053, 319);     // Vertical GRAM Start Address			 
    LCD_WR_CMD(0x0020, 0);			 // set the horizontal initial value of the address counter
    LCD_WR_CMD(0x0021, 0);			 // set the vertical initial value of the address counter
    LCD_WR_REG(0x22);		         // write data to GRAM    
    

    for(i=0; i<76800; i++)		   //320*240=76800		
    {
        LCD_WR_Data(0XF800);	    // RED 
    } 			   
}


/*
 * R01H�е�SS��R60H�е�GS�����������ԭ�㡣
 * R03H��������ˮƽɨ�軹�Ǵ�ֱɨ�裬ˮƽɨ�����ֱɨ����
 * ��Ϊ���ַ�ʽ��ˮƽȡģ��ͼ����Һ��ˮƽɨ��ɨ��ķ�ʽ��
 * ��ʾ�Ļ��������µߵ��������ҵߵ������󣬵���ʾ��ͼƬ��
 * ��ȷ�ģ�����Һ����ֱɨ��ķ�ʽ�Ļ����������롣
 
 * ע      ��ͼ��ˮƽȡģʱ��Һ������������ʾ��ʽ 
             ��ֱȡģʱҲ��������ʾ��ʽ��
 * ˮƽɨ�裺0X1000 ����->����  
 *         : 0X1010	����->����
 *				 : 0X1020	����->����
 *				 : 0X1030	����->����
 * ��ֱɨ�裺0X1008	����->����
 *         : 0X1018	����->����
 *         : 0X1028	����->����
 *         : 0X1038	����->����
 */

/******************** (C) MODIFIED  2011 Ұ��Ƕ��ʽ���������� *****END OF FILE***/







