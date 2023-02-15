//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_rcc.h>
#include <delay.h>
#include "uart.h"


#if (UART_TX_MODE == UART_MODE_IRQ)
static uint8_t TxBuff[UartTxBuffSize];
static uint16_t TxRdIdx = 0, TxWrIdx = 0, TxCntr = 0;
#endif

#if (UART_TX_MODE == UART_MODE_POLLING)
static uint32_t TOcntr;
#endif

static uint8_t RxBuff[UartRxBuffSize];
static uint16_t RxRdIdx = 0;
static uint16_t RxWrIdx = 0;
static uart_RxFunc RxDataProcessFunc = 0;
static USART_TypeDef* pUART;

typedef struct
{
  uint8_t TxRun:1;
  uint8_t TxOvr:1;
  uint8_t RxOvr:1;
}
tUARTstate;
tUARTstate UARTstate;

//==============================================================================
// ������������� UART. RxFunc - �������, ���������� ��� ��������� ������ � ��������� Nextion
//==============================================================================
void UART_Init(USART_TypeDef* USARTx, uint32_t BaudRate, uart_RxFunc RxFunc)
{
  pUART = USARTx;
  RxDataProcessFunc = RxFunc;
  
  USART_Cmd(pUART, DISABLE);
  USART_DeInit(pUART);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  if (pUART == USART1)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  else
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  // ����������� ���� Tx,Rx
  GPIO_InitTypeDef initGPIOStruct;
  initGPIOStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  if (pUART == USART1)
    initGPIOStruct.GPIO_Pin = GPIO_Pin_9;
  else
    initGPIOStruct.GPIO_Pin = GPIO_Pin_2;
  initGPIOStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &initGPIOStruct);
  
  if (pUART == USART1)
    initGPIOStruct.GPIO_Pin = GPIO_Pin_10;
  else
    initGPIOStruct.GPIO_Pin = GPIO_Pin_3;
  initGPIOStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &initGPIOStruct);
  
  // ����������� USART
  USART_InitTypeDef initStruct;
  initStruct.USART_BaudRate = BaudRate;
  initStruct.USART_WordLength = USART_WordLength_8b;
  initStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  initStruct.USART_Parity = USART_Parity_No;
  initStruct.USART_StopBits = USART_StopBits_1;
  initStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(pUART, &initStruct);

  USART_ClockInitTypeDef initClockStruct;
  initClockStruct.USART_Clock = USART_Clock_Disable;
  initClockStruct.USART_CPHA = USART_CPHA_1Edge;
  initClockStruct.USART_CPOL = USART_CPOL_High;
  initClockStruct.USART_LastBit = USART_LastBit_Disable;
  USART_ClockInit(pUART, &initClockStruct);
  
  // ��������� ���������� �� USART
  NVIC_InitTypeDef NVIC_InitStructure;
  // Configure the Priority Group to 2 bits
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               // ������������� ���������� ����� � �������� ����������
  
  // Enable the USARTx Interrupt
  if (pUART == USART1)
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;           // ���������� �� USART 
  else
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;           // ���������� �� USART 
  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // ������ ��������� � ������
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // ������ ��������� � ���������
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               // ��������� ����������
  if (pUART == USART1)
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  else
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_Init(&NVIC_InitStructure);

  USART_ITConfig(pUART, USART_IT_RXNE, ENABLE);
  USART_ITConfig(pUART, USART_IT_IDLE, ENABLE);

  // ��������� ������ USART
  USART_Cmd(pUART, ENABLE);
}
//==============================================================================


#if (UART_TX_MODE == UART_MODE_IRQ)
//==============================================================================
// ��������� �������� ������ ������ �� ������������ ������ ���� �������� � ������ ������ �� ���
//==============================================================================
void UART_TxStart(void)
{
  // ���� �������� � ������ ������ �� ���
  if (!UARTstate.TxRun)
  {
    // ����� ������ ���� �� ��������
    USART_SendData(pUART, TxBuff[TxRdIdx++]);
    if (TxRdIdx == UartTxBuffSize)
      TxRdIdx = 0;
    // ��������� ���-�� ���� �� �������� � USART
    TxCntr--;
    // ������ ���� �������� �� USART � ��������
    UARTstate.TxRun = 1;
    // ��������� ���������� �� ������������ �������� �������� �� USART
    USART_ITConfig(pUART, USART_IT_TXE, ENABLE);
  }
}
//==============================================================================


//==============================================================================
// ������� �������� ������ (��������� �� � ����������� �����)
//==============================================================================
int8_t UART_Send(uint8_t *pBuff, uint16_t Len)
{
  UARTstate.TxOvr = 0;
  
  while (Len--)
  {
    TxBuff[TxWrIdx++] = *pBuff++;
    if (TxWrIdx == UartTxBuffSize)
      TxWrIdx = 0;
    
    if (++TxCntr == UartTxBuffSize)     // ����� �� �������� ��������
    {
      UARTstate.TxOvr = 1;              // ������� ���� ������������ ������ �� �������� �� ������ ������
      TxWrIdx = TxCntr = 0;
      return UART_ERR_BUFF_OVF;
    }
  }
  
  UART_TxStart();
  
  return UART_ERR_OK;
}
//==============================================================================
#endif


#if (UART_TX_MODE == UART_MODE_POLLING)
//==============================================================================
// ������� �������� ������ (�������� � UART ��������� ���������)
//==============================================================================
int8_t UART_Send(uint8_t *pBuff, uint16_t Len)
{
  UARTstate.TxOvr = 0;
  
  while (Len--)
  {
    USART_SendData(pUART, *(pBuff++));

    TOcntr = UART_XT_TIMEOUT_BYTE;
    while ((USART_GetFlagStatus(pUART, USART_FLAG_TXE) == RESET) && TOcntr) 
    {
      TOcntr--;
      delay_us(10);
    }
    if (!TOcntr)
      return UART_ERR_HW_TIMEOUT;
  }
  
  return UART_ERR_OK;
}
//==============================================================================
#endif


//==============================================================================
// ������� ���������� ���-�� ���� � Rx ������
//==============================================================================
uint16_t USART_RxDataToProc(void)
{
  return RxWrIdx >= RxRdIdx ? RxWrIdx - RxRdIdx : RxWrIdx + sizeof(RxBuff) - RxRdIdx;
}
//==============================================================================


//==============================================================================
// ������� ��������� ���������� ����� �� Rx ������������ ������
//==============================================================================
uint8_t USART_GetByteToProc(void)
{
  uint8_t Byte = RxBuff[RxRdIdx];
  if (++RxRdIdx == sizeof(RxBuff))
    RxRdIdx = 0;
  
  return Byte;
}
//==============================================================================


//==============================================================================
// ��������� ��������� ���������� UART
//==============================================================================
void USART_IRQProcessFunc(void)
{ 
  if (USART_GetITStatus(pUART, USART_IT_IDLE) == SET) 
  {
    if (RxDataProcessFunc)
      RxDataProcessFunc();
    USART_ReceiveData(pUART);
  }

  if (pUART->SR & (USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE | USART_FLAG_ORE)) // ������
  {
    RxWrIdx = RxRdIdx = 0;
    USART_ReceiveData(pUART);
  }
  
  // ���������� �� ������ ������
  if (USART_GetITStatus(pUART, USART_IT_RXNE) == SET) 
  {
    uint8_t Byte = USART_ReceiveData(pUART) & 0xFF;
    RxBuff[RxWrIdx] = Byte;
    if (++RxWrIdx == sizeof(RxBuff))
      RxWrIdx = 0;
    
    if (RxWrIdx == RxRdIdx)     // ����� ������������
    {
      RxWrIdx = RxRdIdx = 0;
      UARTstate.RxOvr = 1;
    }
    
    if (USART_RxDataToProc() == (UartRxBuffSize - 64))   // ����� ����� ��������
    {
      if (RxDataProcessFunc)
        RxDataProcessFunc();
    }
  }
  
#if (UART_TX_MODE == UART_MODE_IRQ)
  // ���������� �� ��������
  if (USART_GetITStatus(pUART, USART_IT_TXE) == SET) 
  {
    if (TxCntr)                 // ���� ��� ������ � ������ �� ������
    {
      // ����� ��������� ���� �� ��������
      USART_SendData(pUART, TxBuff[TxRdIdx++]);
      if (TxRdIdx == UartTxBuffSize)
        TxRdIdx = 0;
      // ��������� ���-�� ���� �� �������� � USART
      TxCntr--;
    }
    else                        // ��� ������ �� ������ ��������
    {
      // ��������� ���������� �� ������������ �������� �������� �� USART
      USART_ITConfig(pUART, USART_IT_TXE, DISABLE);
      // ������� ���� �������� �� USART � ��������
      UARTstate.TxRun = 0;
    }
  }
#endif
}
//==============================================================================


//==============================================================================
// ���������� ���������� �� UART1
//==============================================================================
void USART1_IRQHandler(void)
{
  if (pUART == USART1)
    USART_IRQProcessFunc();
}
//==============================================================================


//==============================================================================
// ���������� ���������� �� UART2
//==============================================================================
void USART2_IRQHandler(void)
{ 
  if (pUART == USART2)
    USART_IRQProcessFunc();
}
//==============================================================================
