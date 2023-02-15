//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include "timers.h"
#include "tmr2.h"

tmr_handler tmr2_handler;


//==============================================================================
// ��������� ����������� ������
//==============================================================================
void tmr2_init(uint32_t Freq, void (*func)(void))
{
  // ���������� �������, ������� ����� �������� � ����������� ���������� �������
  tmr2_handler = func;
  
  // �������� ������������ ������� (������� ������������ = AHB/2) 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

  // �������� ������� ������ APB1, �� ������� ����� ������� �������� ��� �������
  RCC_ClocksTypeDef rcc_clocks;
  RCC_GetClocksFreq(&rcc_clocks);
  uint32_t APB1 = rcc_clocks.PCLK1_Frequency;
  if (RCC->CFGR & RCC_CFGR_PPRE1)
    APB1 <<= 1;
  
  // ������� ����������� �������� ������� � �������� ��� ���������
  uint32_t Coef = APB1 / Freq;
  uint32_t OC2_value = 100000;
  uint16_t Prescaler = 1;
  
  // ���������� �������� ������� ������� � �����������
  for (uint8_t i = 0; i < 16; i++)
  {
    OC2_value = Coef / Prescaler;
    
    if (OC2_value < 65536)        // �������� ������
      break;
    
    Prescaler <<= 1;            // ��������� ��������
  }

  // ����������� ������������ �������
  TIM_TimeBaseInitTypeDef TIM2_InitStruct;
  TIM2_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; 
  TIM2_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM2_InitStruct.TIM_Prescaler = Prescaler - 1; 
  TIM2_InitStruct.TIM_Period = OC2_value - 1; 
  TIM2_InitStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &TIM2_InitStruct);

  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  
  // ����������� ���������� �������
  NVIC_InitTypeDef nvicStructure;
  nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
  nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
  nvicStructure.NVIC_IRQChannelSubPriority = 1;
  nvicStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvicStructure);
}
//==============================================================================


//==============================================================================
// ��������� �������� ������
//==============================================================================
void tmr2_start(void)
{
  TIM_Cmd(TIM2, ENABLE);
}
//==============================================================================


//==============================================================================
// ��������� ������������� ������
//==============================================================================
void tmr2_stop(void)
{
  TIM_Cmd(TIM2, DISABLE);
}
//==============================================================================

uint32_t Test = 0;

//==============================================================================
// ���������� ���������� �������2
//==============================================================================
void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    tmr2_handler();
    Test++;
  }
}
//==============================================================================
