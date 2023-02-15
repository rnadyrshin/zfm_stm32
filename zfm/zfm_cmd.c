//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
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
// ��������� �������������� ������
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
// ������� ������� ������� 16-������ ����������� �����
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
// ������� �������� ������ � ZFM
//==============================================================================
int8_t zfm_Send(uint8_t *pAddr, uint8_t PID, uint8_t InstrCode, uint8_t *pBuff, uint16_t Len)
{
  uint8_t *pTxBuff = TxBuff;

  // ��������� Header
  *(pTxBuff++) = (ZFM_STARTCODE >> 8) & 0xFF;
  *(pTxBuff++) = ZFM_STARTCODE & 0xFF;
  // ��������� Addr
  memcpy(pTxBuff, pAddr, 4);
  pTxBuff += 4;
  // ��������� Package identifier
  *(pTxBuff++) = PID;
  // ��������� Package length
  *(pTxBuff++) = ((Len + 3) >> 8) & 0xFF;
  *(pTxBuff++) = (Len + 3) & 0xFF;
  // ��������� Instruction Code
  *(pTxBuff++) = InstrCode;
  // ��������� 
  memcpy(pTxBuff, pBuff, Len);
  pTxBuff += Len;
  // ��������� Checksum
  uint16_t Sum = zfm_CalcSum(&TxBuff[6], Len + 1 + 2 + 1);
  *(pTxBuff++) = (Sum >> 8) & 0xFF;
  *(pTxBuff++) = Sum & 0xFF;
  
  // ���������� �����
  return UART_Send(TxBuff, Len + 2 + 4 + 1 + 2 + 1 + 2);
}
//==============================================================================


//==============================================================================
// ������� ����� ������
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
// �������-���������� ���� ������� �� ������� (���������� �� ����������� ����������!)
//==============================================================================
void zfm_Rx(void)
{
  zfm_state.AnswerWaiting = 0;
  zfm_state.AnswerReceived = 1;
}
//==============================================================================


//==============================================================================
// ������� ������������ �������� ������ ����� ���������
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
// ������� �������� ������� � ������� � ��������� ������ � �������� ����� ������
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
// ������� ������� ������ �� ������ �� �������
//==============================================================================
int8_t zfm_ParsePacket(uint8_t *pBuff, int16_t Len, uint8_t *pPID, uint8_t *pConfCode, uint8_t *pRxBuff, uint16_t *pReaded)
{
  if (pReaded)
    *pReaded = 0;
  
  // ��������� ����� �������� ������
  if (Len < 9) // ����� �� ���� ������� ������ ������������
    return ZFM_ERR_NO_ANSVER;

  // ��������� �������
  if ((*pBuff != (ZFM_STARTCODE >> 8)) || (*(pBuff + 1) != (ZFM_STARTCODE & 0xFF)))
    return -1;

  // ��������� ����� ����������
  if (memcmp(pBuff + 2, zfm_Addr, 4) != 0)  // ����� ������� �� �� ���� ������ ZFM
    return -1;

  // ���������� PID
  *pPID = *(pBuff + 6);
  // ������ ����� ������ ������ � ����� ������������� � ����������� ������
  uint16_t PakLen = (*(pBuff + 7) << 8) | (*(pBuff + 8) & 0xFF);
  if ((PakLen + 9) > Len)
    return ZFM_ERR_NOTFULL_PACKET;
  
  // ���������� ��� ������������� ���� ����� ���� ACK
  if ((*pPID == AckPacket) && (pConfCode))
    *pConfCode = *(pBuff + 9);
  
  // ������ ����������� ����� ������
  uint16_t CheckSum = (*(pBuff + PakLen + 7) << 8) | (*(pBuff + PakLen + 8) & 0xFF);
  uint16_t CalcCheckSum = zfm_CalcSum(pBuff + 6, PakLen + 3 - 2);
  if (CheckSum != CalcCheckSum) // ����������� ����� �� �������, ���������� ��� ������
    return ZFM_ERR_CHECK_SUM;
  
  // �������� ���� ������ � ��������� pRxBuff
  if (*pPID == AckPacket)
  {
    PakLen -= 3;  // ��������� ����� ���� ������ �� ������ ����� ���� ������������� � ����������� ����� 
    if ((pRxBuff) && (PakLen))
      memcpy(pRxBuff, pBuff + 10, PakLen);
  }
  else
  {
    PakLen -= 2;  // ��������� ����� ���� ������ �� ������ ���� ����������� ����� 
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
// ������� ������� ACK-������
//==============================================================================
int8_t zfm_ParseAckBlock(eZFM_ConfCode Confirmation, uint8_t *pOutBuff, uint16_t *pReaded)
{
  uint16_t Readed;
  uint8_t PID = 0;
  uint8_t ConfCode = 0;

  if (pReaded)
    *pReaded = 0;

  // ������ �����
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
// ������� ������� ������� � �������
//==============================================================================
int8_t zfm_ParseDataBlock(uint8_t *pOutBuff, uint16_t *pReaded)
{
  uint8_t PID = 0;
  uint16_t TotalReaded = 0;
  uint16_t Readed;

  do 
  {
    // ������ ��������� �����
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
// ������� ������ ����� ������ �� ZFM � ����� � ������
//==============================================================================
int8_t zfm_GetDataBlock(eZFM_ConfCode ConfCode, uint8_t *pOutBuff, uint16_t *pReaded)
{
  uint16_t Readed;
  uint16_t TotalReaded = 0;
  if (pReaded)
    *pReaded = 0;

  // ������ ACK-�����
  int8_t result = zfm_ParseAckBlock(ConfCode, pOutBuff, &Readed);
  if (result < 0) 
    return result;

  // ������ ������ � �������
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
