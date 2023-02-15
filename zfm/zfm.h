//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _ZFM_H
#define _ZFM_H

#include "zfm_cmd.h"
#include <stm32f10x_usart.h>


// ����� �� �������������� ������� GetInfo ������ ���������� � ������
#define ZFM_GetInfoFunction     1       // ������� ������ ������ � ������������� ������� (������� 512 ���� ���)

typedef struct
{
  uint16_t SystemStatusReg;
  uint16_t FingerLibSize;
  uint8_t SecurityLevel;
  uint32_t DeviceAddr;
  eZFM_PaketLen PacketSize;
  eZFM_BaudCode BaudCode;
  char SensorModel[9];          // ������ ������� (8 ��������)
  char SensorVendor[9];         // ������������� ������� (8 ��������)
}
tSensorParams;


// ������������� ZFM
int8_t zfm_Init(USART_TypeDef* USARTx, uint32_t BaudRate, uint8_t *pAddr, uint8_t *pPassword);
// ������� �������� ������ ������� � ����������
int8_t zfm_VerifyPassword(uint8_t *pPassword);
// ������� ��������� ������ ������� � ����������
int8_t zfm_SetPassword(uint8_t *pPassword);
// ������� ��������� ������ ������
int8_t zfm_SetAddr(uint8_t *pNewAddr);
// ������� ������ BaudRate
int8_t zfm_SetBaudRate(eZFM_BaudCode BaudCode);
// ������� ������ Security Level
int8_t zfm_SetSecurityLevel(uint8_t SecurityLevel);
// ������� ������ Packet Len
int8_t zfm_SetPacketLen(eZFM_PaketLen NewPacketLen);
// ������� ������ ���� ��������� ���������� �� ZFM
int8_t zfm_ReadSysParam(tSensorParams *pStruct);
// ������� ������ ���� ��������� ���������� ZFM. 
// ������� �� ������ �� �� ZFM, � ���������� ���������, ���������� � ��������� zfm_Params � ��������
void zfm_GetSysParam(tSensorParams *pStruct);
// ������� ���������� �������������� ��������� � ���������� ��� � ImageBuffer
int8_t zfm_GetImage(void);
// ������� ���������� ������ ��������� �� ������ Image Buffer � ���������� � � Char Buffer 1 ��� Char Buffer 2 
int8_t zfm_Img2Tz(uint8_t CharBufferNum);
// ������� ���������� ������������� ������� ��������� �� ������ � ������� Char Buffer 1 � Char Buffer 2 
int8_t zfm_RegModel(void);
// ������� ���������� ������� ��������� � ���������� ��������. 
int8_t zfm_StoreTemplate(uint8_t CharBufferNum, uint16_t PageID);
// ������� ������ ������� ��������� �� ���������� �������� � ����� CharBuffer
int8_t zfm_LoadTemplate(uint8_t CharBufferNum, uint16_t PageID);
// ������� ���������� ��������� �������� � ������� CharBuffer1 � CharBuffer2. 
// ��������� �� ��������� pMatchScore ����������� ����������.
int8_t zfm_Match(uint16_t *pMatchScore);
// ������� ������ ���-�� ���������� �������� ����������
int8_t zfm_ReadTemplateNum(uint16_t *pNum);
// ������� ������ �������� � ������� CharBuffer1 � CharBuffer2 � ����������
int8_t zfm_Search(uint8_t CharBufferNum, uint16_t StartPage, uint16_t PageNum, uint16_t *pPageID, uint16_t *pMatchScore);
// ������� ������ �������� � ������� CharBuffer1 � CharBuffer2 � ����������. ���������� �� Search ����� ����� �������� residual fingerprint
int8_t zfm_SearchWithResidJud(uint8_t CharBufferNum, uint16_t StartPage, uint16_t PageNum, uint16_t *pPageID, uint16_t *pMatchScore);
// ������� ������ ������������ ��������� � ���������� ��������. ���������� 3 ������� - GetImage, Img2TZ, Search.
int8_t zfm_AutoSearch(uint8_t FingerWaitTime, uint16_t StartPage, uint16_t PageNum, uint16_t *pPageID, uint16_t *pMatchScore);
// ������� �������� ���������� �������� ���������� �� ����������
int8_t zfm_DeleteTemplates(uint16_t StartPage, uint16_t PageNum);
// ������� �������� ������ ������� ���������� �� ����������
int8_t zfm_DeleteTemplate(uint16_t PageID);
// ������� �������� ���������� �������� ���������� �� ����������
int8_t zfm_EmptyTemplates(void);
// ������� ������ ������� ��������� ����� � ���������� �������� ����������
int8_t zfm_ReadConList(uint8_t IndexPage, uint8_t *pTable);
// ������� ���������, ������ �� ������ � ���������� �������� ����������
int8_t zfm_IsTemplateExists(uint16_t TemplateIndex, uint8_t *pExists);
// ������� �������� ��������� �������
int8_t zfm_OpenLED(void);
// ������� ��������� ��������� �������
int8_t zfm_CloseLED(void);
// ������� �������������� ��������� ��� ���������� ���������� �������
int8_t zfm_GetmageFreeLED(void);
// ������� ���������� ������ ��������� (� ������� ��������� �������)
int8_t zfm_AutoLogin(uint8_t WaitTime, uint8_t FingerReadNum, uint16_t PageID, uint8_t AllowRepeatedTemplates);
// ������� ������ ������ CharBuffer1 ��� CharBuffer2 �� ������
int8_t zfm_UploadChar(uint8_t CharBufferNum, uint8_t *pOutBuff, uint16_t *pReaded);
// ������� ������ ����������� ��������� �� ������ ImageBuffer �� ������
int8_t zfm_UploadImage(uint8_t *pOutBuff, uint16_t *pReaded);


#endif