//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include "timers.h"
#include "tmr2.h"

tmr_handler tmr2_handler;


//==============================================================================
// Процедура настраивает таймер
//==============================================================================
void tmr2_init(uint32_t Freq, void (*func)(void))
{
  // Запоминаем функцию, которую нужно вызывать в обработчике прерывания таймера
  tmr2_handler = func;
  
  // Включаем тактирование таймера (частота тактирования = AHB/2) 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

  // Выясняем частоту работы APB1, от которой будем считать делители для таймера
  RCC_ClocksTypeDef rcc_clocks;
  RCC_GetClocksFreq(&rcc_clocks);
  uint32_t APB1 = rcc_clocks.PCLK1_Frequency;
  if (RCC->CFGR & RCC_CFGR_PPRE1)
    APB1 <<= 1;
  
  // Считаем необходимый делитель таймера и значение для сравнения
  uint32_t Coef = APB1 / Freq;
  uint32_t OC2_value = 100000;
  uint16_t Prescaler = 1;
  
  // Перебираем делители таймера начиная с наименьшего
  for (uint8_t i = 0; i < 16; i++)
  {
    OC2_value = Coef / Prescaler;
    
    if (OC2_value < 65536)        // Делитель выбран
      break;
    
    Prescaler <<= 1;            // Удваиваем делитель
  }

  // Настраиваем тактирование таймера
  TIM_TimeBaseInitTypeDef TIM2_InitStruct;
  TIM2_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; 
  TIM2_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM2_InitStruct.TIM_Prescaler = Prescaler - 1; 
  TIM2_InitStruct.TIM_Period = OC2_value - 1; 
  TIM2_InitStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &TIM2_InitStruct);

  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  
  // Настраиваем прерывание таймера
  NVIC_InitTypeDef nvicStructure;
  nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
  nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
  nvicStructure.NVIC_IRQChannelSubPriority = 1;
  nvicStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvicStructure);
}
//==============================================================================


//==============================================================================
// Процедура стартует таймер
//==============================================================================
void tmr2_start(void)
{
  TIM_Cmd(TIM2, ENABLE);
}
//==============================================================================


//==============================================================================
// Процедура останавливает таймер
//==============================================================================
void tmr2_stop(void)
{
  TIM_Cmd(TIM2, DISABLE);
}
//==============================================================================

uint32_t Test = 0;

//==============================================================================
// Обработчик прерывания таймера2
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
