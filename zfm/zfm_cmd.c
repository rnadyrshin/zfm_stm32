//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include <stm32f10x_usart.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <delay.h>
#include <uart.h>
#include "zfm.h"
#include "zfm_cmd.h"


tZFM_state zfm_state;

static uint8_t zfm_Addr[] = {0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t TxBuff[256];
static uint8_t RxBuff[300];


//==============================================================================
// Процедура инициализирует буферы
//==============================================================================
void zfm_InitBuff(void)
{
  zfm_state.pAnswerBuff = RxBuff;
  zfm_state.AnswerWaiting = 0;
  zfm_state.AnswerReceived = 0;
  zfm_state.Len = 0;
}
//==============================================================================


//==============================================================================
// Функция расчёта простой 16-битной контрольной суммы
//==============================================================================
uint16_t zfm_CalcSum(uint8_t *pBuff, uint16_t Len)
{
  uint16_t Sum = 0;
  
  while (Len--)
    Sum += *(pBuff++);
  
  return Sum;
}
//==============================================================================


//==============================================================================
// Функция отправки пакета в ZFM
//==============================================================================
int8_t zfm_Send(uint8_t *pAddr, uint8_t PID, uint8_t InstrCode, uint8_t *pBuff, uint16_t Len)
{
  uint8_t *pTxBuff = TxBuff;

  // Заполняем Header
  *(pTxBuff++) = (ZFM_STARTCODE >> 8) & 0xFF;
  *(pTxBuff++) = ZFM_STARTCODE & 0xFF;
  // Заполняем Addr
  memcpy(pTxBuff, pAddr, 4);
  pTxBuff += 4;
  // Заполняем Package identifier
  *(pTxBuff++) = PID;
  // Заполняем Package length
  *(pTxBuff++) = ((Len + 3) >> 8) & 0xFF;
  *(pTxBuff++) = (Len + 3) & 0xFF;
  // Заполняем Instruction Code
  *(pTxBuff++) = InstrCode;
  // Заполняем 
  memcpy(pTxBuff, pBuff, Len);
  pTxBuff += Len;
  // Заполняем Checksum
  uint16_t Sum = zfm_CalcSum(&TxBuff[6], Len + 1 + 2 + 1);
  *(pTxBuff++) = (Sum >> 8) & 0xFF;
  *(pTxBuff++) = Sum & 0xFF;
  
  // Отправляем пакет
  return UART_Send(TxBuff, Len + 2 + 4 + 1 + 2 + 1 + 2);
}
//==============================================================================


//==============================================================================
// Функция приёма ответа
//==============================================================================
int16_t zfm_Recv(uint16_t TimeOutMs)
{
  zfm_state.AnswerWaiting = 1;
  
  uint32_t TOcntr = (uint32_t)TimeOutMs * 1000;
  while (TOcntr)
  {
    if (USART_RxDataToProc())
    {
      if (zfm_state.Len < sizeof(RxBuff))
      {
        *(zfm_state.pAnswerBuff + zfm_state.Len) = USART_GetByteToProc();
        zfm_state.Len++;
      }
      else
      {
        zfm_state.BufferFull = 1;
        return zfm_state.Len;
      }
    }
    
    if (zfm_state.AnswerReceived)
    {
      zfm_state.AnswerReceived = 0;
      return zfm_state.Len;
    }
    
    TOcntr--;
    delay_us(1);
  }

  return ZFM_ERR_NO_ANSVER;
}
//==============================================================================


//==============================================================================
// Функция-обработчик всех посылок из дисплея (вызывается из обработчика прерывания!)
//==============================================================================
void zfm_Rx(void)
{
  zfm_state.AnswerWaiting = 0;
  zfm_state.AnswerReceived = 1;
}
//==============================================================================


//==============================================================================
// Функция освобождения приёмного буфера после обработки
//==============================================================================
void zfm_Processed(uint16_t Len)
{
  if (Len <= zfm_state.Len)
  {
    memmove(RxBuff, &RxBuff[Len], zfm_state.Len - Len);
    zfm_state.Len -= Len;
  }
  else
    zfm_state.BufferFull = 1;
}
//==============================================================================


//==============================================================================
// Функция отправки команды в дисплей с ожиданием ответа с заданным кодом ответа
//==============================================================================
int16_t zfm_SendCommand(uint8_t *pAddr, eZFM_PID PID, eZFM_CmdCode InstrCode, uint8_t *pBuff, uint16_t Len, uint16_t TimeOutMs)
{
  int8_t Result = zfm_Send(pAddr, (uint8_t) PID, InstrCode, pBuff, Len);
  
  if (Result != UART_ERR_OK)
    return Result;
  
  if (!TimeOutMs)
    TimeOutMs = ZFM_TimeOutMs;
  
  zfm_state.BufferFull = 0;
  zfm_state.Len = 0;
  
  return zfm_Recv(TimeOutMs);
}
//==============================================================================


//==============================================================================
// Функция разбора ответа от модуля на команду
//==============================================================================
int8_t zfm_ParsePacket(uint8_t *pBuff, int16_t Len, uint8_t *pPID, uint8_t *pConfCode, uint8_t *pRxBuff, uint16_t *pReaded)
{
  if (pReaded)
    *pReaded = 0;
  
  // Проверяем длину входного буфера
  if (Len < 9) // Длина до поля размера пакета включительно
    return ZFM_ERR_NO_ANSVER;

  // Проверяем префикс
  if ((*pBuff != (ZFM_STARTCODE >> 8)) || (*(pBuff + 1) != (ZFM_STARTCODE & 0xFF)))
    return -1;

  // Проверяем адрес устройства
  if (memcmp(pBuff + 2, zfm_Addr, 4) != 0)  // Ответ получен не от того адреса ZFM
    return -1;

  // Возвращаем PID
  *pPID = *(pBuff + 6);
  // Читаем длину пакета вместе с кодом подтверждения и контрольной суммой
  uint16_t PakLen = (*(pBuff + 7) << 8) | (*(pBuff + 8) & 0xFF);
  if ((PakLen + 9) > Len)
    return ZFM_ERR_NOTFULL_PACKET;
  
  // Возвращаем код подтверждения если пакет типа ACK
  if ((*pPID == AckPacket) && (pConfCode))
    *pConfCode = *(pBuff + 9);
  
  // Читаем контрольную сумму пакета
  uint16_t CheckSum = (*(pBuff + PakLen + 7) << 8) | (*(pBuff + PakLen + 8) & 0xFF);
  uint16_t CalcCheckSum = zfm_CalcSum(pBuff + 6, PakLen + 3 - 2);
  if (CheckSum != CalcCheckSum) // Контрольная сумма не сошлась, возвращаем код ошибки
    return ZFM_ERR_CHECK_SUM;
  
  // Копируем тело пакета в указатель pRxBuff
  if (*pPID == AckPacket)
  {
    PakLen -= 3;  // Уменьшаем длину тела пакета на размер полей кода подтверждения и контрольной суммы 
    if ((pRxBuff) && (PakLen))
      memcpy(pRxBuff, pBuff + 10, PakLen);
  }
  else
  {
    PakLen -= 2;  // Уменьшаем длину тела пакета на размер поля контрольной суммы 
#if !ZFM_GetDataBlock_DEBUG
    if ((pRxBuff) && (PakLen))
      memcpy(pRxBuff, pBuff + 9, PakLen);
#endif
  }

  if (pReaded)
    *pReaded = PakLen;
  
  return 0;
}
//==============================================================================


//==============================================================================
// Функция разбора ACK-пакета
//==============================================================================
int8_t zfm_ParseAckBlock(eZFM_ConfCode Confirmation, uint8_t *pOutBuff, uint16_t *pReaded)
{
  uint16_t Readed;
  uint8_t PID = 0;
  uint8_t ConfCode = 0;

  if (pReaded)
    *pReaded = 0;

  // Читаем пакет
  int8_t result = zfm_ParsePacket(RxBuff, zfm_state.Len, &PID, &ConfCode, pOutBuff, &Readed);
  if (result < 0)
    return result;
  
  zfm_Processed(Readed + 12);
  
  if ((PID != AckPacket) || (ConfCode != Confirmation))
    return -1;
  if (pReaded)
    *pReaded = Readed;
  
  return 0;
}
//==============================================================================


//==============================================================================
// Функция разбора пакетов с данными
//==============================================================================
int8_t zfm_ParseDataBlock(uint8_t *pOutBuff, uint16_t *pReaded)
{
  uint8_t PID = 0;
  uint16_t TotalReaded = 0;
  uint16_t Readed;

  do 
  {
    // Читаем очередной пакет
    int8_t result = zfm_ParsePacket(RxBuff, zfm_state.Len, &PID, 0, pOutBuff + TotalReaded, &Readed);
    if (result < 0)
    {
      if (pReaded)
        *pReaded = TotalReaded;
      return result;
    }

    TotalReaded += Readed;
    zfm_Processed(Readed + 11);
  } while (zfm_state.Len);

  if (pReaded)
    *pReaded = TotalReaded;

  return 0;
}
//==============================================================================


//==============================================================================
// Функция чтения блока данных из ZFM в буфер в памяти
//==============================================================================
int8_t zfm_GetDataBlock(eZFM_ConfCode ConfCode, uint8_t *pOutBuff, uint16_t *pReaded)
{
  uint16_t Readed;
  uint16_t TotalReaded = 0;
  if (pReaded)
    *pReaded = 0;

  // Читаем ACK-пакет
  int8_t result = zfm_ParseAckBlock(ConfCode, pOutBuff, &Readed);
  if (result < 0) 
    return result;

  // Читаем пакеты с данными
  do
  {
    zfm_Recv(200);

    result = zfm_ParseDataBlock(pOutBuff, &Readed);
    TotalReaded += Readed;
    *pReaded = TotalReaded;
    if (pOutBuff)
      pOutBuff += Readed;
  } while (Readed || !result);

  *pReaded = TotalReaded;
  
  return 0;
}
//==============================================================================
