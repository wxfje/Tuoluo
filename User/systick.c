#include "stm32f10x.h"
#include <stdio.h>

#include "systick.h"

/* �ڴ˶������ɸ������ʱ��ȫ�ֱ���
ע�⣬��������volatile����Ϊ����������жϺ���������ͬʱ
�����ʣ��п�����ɱ����������Ż���DelayMS����������
*/
#define TMR_COUNT	4		/* �����ʱ���ĸ��� */
SOFT_TMR g_Tmr[TMR_COUNT];

/*******************************************************************************
	��������SoftTimerDec
	��  ��: ��ʱ������ָ��,ÿ��1ms��1
	��  ��:
	����˵����
*/
static void SoftTimerDec(SOFT_TMR *_tmr)
{
	if (_tmr->flag == 0)
	{
		if (_tmr->count > 0)
		{
			if (--_tmr->count == 0)
			{
				_tmr->flag = 1;
			}
		}
	}
}

/*******************************************************************************
	��������SysTick_ISR
	��  ��:
	��  ��:
	����˵����SysTick�жϷ������ÿ��1ms����1��
*/
void SysTick_ISR(void)
{
	uint8_t i;

	for (i = 0; i < TMR_COUNT; i++)
	{
		SoftTimerDec(&g_Tmr[i]);
	}
}

/*******************************************************************************
	��������DelayMS
	��  ��: �ӳٳ��ȣ���λ1 ms. �ӳپ���Ϊ����1ms
	��  ��:
	����˵������ʱ������ռ�������ʱ��0
*/
void DelayMS(uint32_t n)
{
	/* ���� n = 1 �������������� */
	if (n == 1)
	{
		n = 2;
	}
	g_Tmr[0].count = n;
	g_Tmr[0].flag = 0;

	/* while ѭ���������CPU����IDLE״̬���ѽ��͹��� */
	while (1)
	{
		CPU_IDLE();

		if (g_Tmr[0].flag == 1)
		{
			break;
		}
	}
}

/*******************************************************************************
	��������StartTimer
	��  ��: ��ʱ��ID (0 - 3)
	��  ��:
	����˵����
*/
void StartTimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		return;
	}

	g_Tmr[_id].count = _period;
	g_Tmr[_id].flag = 0;
}

/*******************************************************************************
	��������StartTimer
	��  ��: ��ʱ��ID (0 - 3)
	��  ��: ���� 0 ��ʾ��ʱδ���� 1��ʾ��ʱ��
	����˵����
*/
uint8_t CheckTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		return 0;
	}

	return g_Tmr[_id].flag;
}

