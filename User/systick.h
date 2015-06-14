/****************************************************************************
* Copyright (C), 2009-2010, www.armfly.com
*
* �ļ���: systick.h
* ���ݼ���: ͷ�ļ�
*
* �ļ���ʷ:
* �汾��  ����       ����    ˵��
* v0.1    2009-12-27 armfly  �������ļ�
*
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTICK_H
#define __SYSTICK_H

/* �ȴ���ʱ����ʱ�ڼ䣬������CPU����IDLE״̬�� Ŀǰ�ǿ� */
#define CPU_IDLE()

/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef struct
{
	volatile uint32_t count;	/* ������ */
	volatile uint8_t flag;		/* ��ʱ�����־  */
}SOFT_TMR;

void SysTick_Configuration(void);
void DelayMS(uint32_t n);

void StartTimer(uint8_t _id, uint32_t _period);
uint8_t CheckTimer(uint8_t _id);

#endif
