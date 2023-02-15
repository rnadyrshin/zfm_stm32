//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <uart.h>
#include <gpio.h>
#include <timers.h>
#include <delay.h>
#include <zfm_common.h>
#include <zfm.h>
#include "main.h"


#define LedRed_On()             GPIO_ResetBits(ZFM_LedRed_Port, ZFM_LedRed_Pin)
#define LedRed_Off()            GPIO_SetBits(ZFM_LedRed_Port, ZFM_LedRed_Pin)
#define LedGreen_On()           GPIO_ResetBits(ZFM_LedGreen_Port, ZFM_LedGreen_Pin)
#define LedGreen_Off()          GPIO_SetBits(ZFM_LedGreen_Port, ZFM_LedGreen_Pin)
#define LedBlue_On()            GPIO_ResetBits(ZFM_LedBlue_Port, ZFM_LedBlue_Pin)
#define LedBlue_Off()           GPIO_SetBits(ZFM_LedBlue_Port, ZFM_LedBlue_Pin)
#define LedAll_On()             {LedRed_On(); LedGreen_On(); LedBlue_On();}
#define LedAll_Off()            {LedRed_Off(); LedGreen_Off(); LedBlue_Off();}
#define LedState_On()           GPIO_ResetBits(ZFM_LedState_Port, ZFM_LedState_Pin)
#define LedState_Off()          GPIO_SetBits(ZFM_LedState_Port, ZFM_LedState_Pin)

#define GET_EnrollPinState()    GPIO_ReadInputDataBit(ZFM_EnrollNewFinger_Port, ZFM_EnrollNewFinger_Pin)
#define GET_ClearLibState()     GPIO_ReadInputDataBit(ZFM_ClearFingerLib_Port, ZFM_ClearFingerLib_Pin)

#define LED_STATE       0
#define LED_RED         1
#define LED_GREEN       2
#define LED_BLUE        3


uint8_t ZFM_addr[] = {0xFF, 0xFF, 0xFF, 0xFF};  // ����� ZFM �� �������� � ��������
uint8_t ZFM_password[] = {0, 0, 0, 0};
tSensorParams ZFM_Params;
uint16_t TemplateNum = 0;
uint8_t LargeBuff[10024];


//==============================================================================
//
//==============================================================================
void Led_Flash(uint8_t LED, uint16_t OnTimeMs, uint16_t OffTimeMs)
{
  switch (LED)
  {
  case LED_STATE:
    LedState_On();
    delay_ms(OnTimeMs);
    LedState_Off();
    delay_ms(OffTimeMs);
    break;
  case LED_RED:
    LedRed_On();
    delay_ms(OnTimeMs);
    LedRed_Off();
    delay_ms(OffTimeMs);
    break;
  case LED_GREEN:
    LedGreen_On();
    delay_ms(OnTimeMs);
    LedGreen_Off();
    delay_ms(OffTimeMs);
    break;
  case LED_BLUE:
    LedBlue_On();
    delay_ms(OnTimeMs);
    LedBlue_Off();
    delay_ms(OffTimeMs);
    break;
  }
}
//==============================================================================

uint16_t PageID = 0;
uint16_t MatchScore = 0;

//==============================================================================
//
//==============================================================================
void main()
{
  uint8_t ClearLibButtonOld = 1;
  SystemInit();
    
  // ������������� ����� ����������� �� ����� � ��
  gpio_PortClockStart(ZFM_LedState_Port);
  gpio_SetGPIOmode_Out(ZFM_LedState_Port, ZFM_LedState_Pin);
  LedState_Off();
  gpio_PortClockStart(ZFM_LedRed_Port);
  gpio_SetGPIOmode_Out(ZFM_LedRed_Port, ZFM_LedRed_Pin);
  LedRed_Off();
  gpio_PortClockStart(ZFM_LedGreen_Port);
  gpio_SetGPIOmode_Out(ZFM_LedGreen_Port, ZFM_LedGreen_Pin);
  LedGreen_Off();
  gpio_PortClockStart(ZFM_LedBlue_Port);
  gpio_SetGPIOmode_Out(ZFM_LedBlue_Port, ZFM_LedBlue_Pin);
  LedBlue_Off();
  // ������������� ����� ��� �������� � ����� �������� ����� ����������
  gpio_PortClockStart(ZFM_EnrollNewFinger_Port);
  gpio_SetGPIOmode_In(ZFM_EnrollNewFinger_Port, ZFM_EnrollNewFinger_Pin, gpio_PullUp);
  // ������������� ����� ��� ������� ���������� �������� ����������
  gpio_PortClockStart(ZFM_ClearFingerLib_Port);
  gpio_SetGPIOmode_In(ZFM_ClearFingerLib_Port, ZFM_ClearFingerLib_Pin, gpio_PullUp);
  
  int8_t result = zfm_Init(USART1, 57600, ZFM_addr, ZFM_password);
  if (result != 0)
  {
    // ������ ������������� ZFM
    while (1)
    {
      // ������ ����������� �� �����
      Led_Flash(LED_STATE, 100, 200);
    }
  }

  zfm_OpenLED();
  
  while (1)
  {
    // ������ ���������� �������� ���������� � ���������� ������
    result = zfm_ReadTemplateNum(&TemplateNum);

    // ������ ������ ������������ � ������� ���������. LargeBuff - ����� �������� �� ����� 512 �����.
    //result = GetFingerChar(LargeBuff, 1);
    // ������ ���������� ������������ � ������� ���������. LargeBuff - ����� �������� �� ����� 36864 �����.
    // � ������ ����� - 2 ������� � 4-������ ����� (16 �������� ������).
    //result = GetFingerImage(LargeBuff, 1);
    
    uint8_t Button = GET_ClearLibState();
    if ((ClearLibButtonOld) && (!Button))       // ������ ������ ������� ���������� ����������
    {
      LedAll_Off();
      delay_ms(100);
      
      result = zfm_EmptyTemplates();
      
      if (!result)
      {
        Led_Flash(LED_GREEN, 200, 200);
        Led_Flash(LED_GREEN, 200, 200);
        Led_Flash(LED_GREEN, 200, 200);
      }
      else
      {
        Led_Flash(LED_RED, 200, 200);
        Led_Flash(LED_RED, 200, 200);
        Led_Flash(LED_RED, 200, 200);
      }
    }
    ClearLibButtonOld = Button;
        
    if (!GET_EnrollPinState())  // ������ ������ ���������� � ���������� ����� ����������
    {
      LedAll_Off();
      delay_ms(100);

      LedBlue_On();
      // ��������� ������ �� ��������� � ��������� ��� � ���������� ��������
      result = Enroll_Finger(TemplateNum, 10, 1);       // ������� �� ������ ������
      //result = Enroll_Finger_GroupCmd(TemplateNum);     // ������� � ��������� ��������
      LedBlue_Off();
      
      if (!result)
      {
        Led_Flash(LED_GREEN, 200, 200);
        Led_Flash(LED_GREEN, 200, 200);
      }
      else
      {
        Led_Flash(LED_RED, 200, 200);
        Led_Flash(LED_RED, 200, 200);
      }
    }
    else
    {
      // ����������� ����� � ��������� �������� � �����   // ������ ��������
      result = Search_Finger(&PageID, &MatchScore, 1);    // ������� �� ������ ������
      //result = AutoSearch_Finger(&PageID, &MatchScore); // ������� � ��������� ��������
      if (!result)
      {
        LedAll_Off();
        delay_ms(100);
        Led_Flash(LED_GREEN, 1000, 200);
      }
      LedRed_On();
    }

    delay_ms(50);
  }
}
//==============================================================================
