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
#include "zfm_common.h"


static uint8_t zfm_Addr[] = {0xFF, 0xFF, 0xFF, 0xFF};
tSensorParams zfm_Params;


//==============================================================================
// Функция проверки пароля доступа к устройству
//==============================================================================
int8_t zfm_VerifyPassword(uint8_t *pPassword)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, VerifyPassword, pPassword, 4, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция установки пароля доступа к устройству
//==============================================================================
int8_t zfm_SetPassword(uint8_t *pPassword)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, SetPassword, pPassword, 4, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция установки адреса модуля
//==============================================================================
int8_t zfm_SetAddr(uint8_t *pNewAddr)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, SetAddr, pNewAddr, 4, ZFM_TimeOutMs);
  memcpy(zfm_Addr, pNewAddr, 4);        // Запоминаем новый адрес как текущий
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


#if ZFM_GetInfoFunction
static uint8_t InfoBuff[512];

//==============================================================================
// Функция чтения информации о модуле
//==============================================================================
int8_t zfm_GetInfo(void)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, GetInfo, 0, 0, 1000);
  uint16_t Readed;
  int8_t result = zfm_GetDataBlock(Ok, InfoBuff, &Readed);
  if (result < 0)
    return result;

  if (Readed > 52)
  {
    memcpy(zfm_Params.SensorModel, &InfoBuff[28], 8);
    zfm_Params.SensorModel[8] = 0;
    memcpy(zfm_Params.SensorVendor, &InfoBuff[44], 8);
    zfm_Params.SensorVendor[8] = 0;
  }
  
  return result;
}
//==============================================================================
#endif


//==============================================================================
// Функция записи системного параметра
//==============================================================================
int8_t zfm_SetSysParam(eZFM_SysParamNum ParamNum, uint8_t ParamValue)
{
  uint8_t Buff[2];
  Buff[0] = ParamNum;
  Buff[1] = ParamValue;
	
  zfm_SendCommand(zfm_Addr, CommandPacket, SetSysParam, Buff, 2, ZFM_TimeOutMs);
  int8_t result = zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
  
  // После записи параметра читаем параметры из модуля в структуру zfm_Params в драйвере ZFM
  zfm_ReadSysParam(0);
  return result;
}
//==============================================================================


//==============================================================================
// Функция записи BaudRate
//==============================================================================
int8_t zfm_SetBaudRate(eZFM_BaudCode NewBaudCode)
{
  return zfm_SetSysParam(BaudRate, (uint8_t)NewBaudCode);
}
//==============================================================================


//==============================================================================
// Функция записи Security Level
//==============================================================================
int8_t zfm_SetSecurityLevel(uint8_t NewLevel)
{
  if (SecurityLevel > 5)
    return -5;
  
  return zfm_SetSysParam(SecurityLevel, NewLevel);
}
//==============================================================================


//==============================================================================
// Функция записи Packet Len
//==============================================================================
int8_t zfm_SetPacketLen(eZFM_PaketLen NewPacketLen)
{
  if (SecurityLevel > 5)
    return -5;
  
  return zfm_SetSysParam(SecurityLevel, (uint8_t)NewPacketLen);
}
//==============================================================================


//==============================================================================
// Функция чтения всех системных параметров из ZFM
//==============================================================================
int8_t zfm_ReadSysParam(tSensorParams *pStruct)
{
  uint16_t Readed;
  zfm_SendCommand(zfm_Addr, CommandPacket, ReadSysParams, 0, 0, ZFM_TimeOutMs);
  
  uint8_t Buff[16];
  int8_t result = zfm_ParseAckBlock(Ok, Buff, &Readed);
  if (result < 0)
    return result;
  
  if (Readed == 16)         // Успешный ответ
  {
    uint8_t *pBuff = Buff;
    // Читаем параметры сенсора сначала в структуру zfm_Params
    zfm_Params.SystemStatusReg = *(pBuff++) << 8;
    zfm_Params.SystemStatusReg |= *(pBuff++);
    pBuff += 2;
    zfm_Params.FingerLibSize = *(pBuff++) << 8;
    zfm_Params.FingerLibSize |= *(pBuff++);
    pBuff++;
    zfm_Params.SecurityLevel = *(pBuff++);
    zfm_Params.DeviceAddr = ((uint32_t)*(pBuff++)) << 24;
    zfm_Params.DeviceAddr |= ((uint32_t)*(pBuff++)) << 16;
    zfm_Params.DeviceAddr |= ((uint32_t)*(pBuff++)) << 8;
    zfm_Params.DeviceAddr |= *(pBuff++);
    pBuff++;
    zfm_Params.PacketSize = (eZFM_PaketLen) *(pBuff++);
    pBuff++;
    zfm_Params.BaudCode = (eZFM_BaudCode) *(pBuff++);

    // Переносим параметры в структуру по указателю pStruct
    zfm_GetSysParam(pStruct);
  }
    
  return 0;
}
//==============================================================================


//==============================================================================
// Функция чтения всех системных параметров ZFM. 
// Функция не читает их из ZFM, а использует параметры, сохранённые в структуре zfm_Params в драйвере
//==============================================================================
void zfm_GetSysParam(tSensorParams *pStruct)
{
  if (!pStruct)
    return;

  // Переносим параметры в структуру по указателю pStruct
  pStruct->SystemStatusReg = zfm_Params.SystemStatusReg;
  pStruct->FingerLibSize = zfm_Params.FingerLibSize;
  pStruct->SecurityLevel = zfm_Params.SecurityLevel;
  pStruct->DeviceAddr = zfm_Params.DeviceAddr;
  pStruct->PacketSize = zfm_Params.PacketSize;
  pStruct->BaudCode = zfm_Params.BaudCode;
  strcpy(pStruct->SensorModel, zfm_Params.SensorModel);
  strcpy(pStruct->SensorVendor, zfm_Params.SensorVendor);
}
//==============================================================================


//==============================================================================
// Функция инициирует детектирование отпечатка и сохранение его в ImageBuffer
//==============================================================================
int8_t zfm_GetImage(void)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, GetImage, 0, 0, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция инициирует свёртку отпечатка из буфера Image Buffer и сохранение её 
// в Char Buffer 1 или Char Buffer 2 
//==============================================================================
int8_t zfm_Img2Tz(uint8_t CharBufferNum)
{
  if ((!CharBufferNum) || (CharBufferNum > 2))
    return ZFM_ERR_WRONG_ARGS;
  
  zfm_SendCommand(zfm_Addr, CommandPacket, Image2TZ, &CharBufferNum, 1, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция инициирует генерирование шаблона отпечатка по данным в буферах 
// Char Buffer 1 и Char Buffer 2 
//==============================================================================
int8_t zfm_RegModel(void)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, RegModel, 0, 0, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция сохранения шаблона отпечатка в библиотеку шаблонов
//==============================================================================
int8_t zfm_StoreTemplate(uint8_t CharBufferNum, uint16_t PageID)
{
  if ((!CharBufferNum) || (CharBufferNum > 2))
    return ZFM_ERR_WRONG_ARGS;

  uint8_t Buff[3];
  Buff[0] = CharBufferNum;
  Buff[1] = PageID >> 8;
  Buff[2] = PageID & 0xFF;
  
  zfm_SendCommand(zfm_Addr, CommandPacket, StoreTemplate, Buff, 3, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция чтения шаблона отпечатка из библиотеки шаблонов в буфер CharBuffer
//==============================================================================
int8_t zfm_LoadTemplate(uint8_t CharBufferNum, uint16_t PageID)
{
  if ((!CharBufferNum) || (CharBufferNum > 2))
    return ZFM_ERR_WRONG_ARGS;

  uint8_t Buff[3];
  Buff[0] = CharBufferNum;
  Buff[1] = PageID >> 8;
  Buff[2] = PageID & 0xFF;
  
  zfm_SendCommand(zfm_Addr, CommandPacket, LoadTemplate, Buff, 3, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция инициирует сравнение шаблонов в буферах CharBuffer1 и CharBuffer2. 
// Сохраняет по указателю pMatchScore коэффициент совпадения.
//==============================================================================
int8_t zfm_Match(uint16_t *pMatchScore)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, Match, 0, 0, ZFM_TimeOutMs);

  uint16_t Readed;
  uint8_t Buff[2];
  int8_t result = zfm_ParseAckBlock(Ok, Buff, &Readed);
  if (result < 0)
    return result;
  
  if (Readed == 2)
  {
    if (pMatchScore)
      *pMatchScore = (Buff[0] << 8) | Buff[1];
  }
  return 0;
}
//==============================================================================


//==============================================================================
// Функция читает кол-во корректных шаблонов отпечатков
//==============================================================================
int8_t zfm_ReadTemplateNum(uint16_t *pNum)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, TemplateCount, 0, 0, ZFM_TimeOutMs);

  uint16_t Readed;
  uint8_t Buff[2];
  int8_t result = zfm_ParseAckBlock(Ok, Buff, &Readed);
  if (result < 0)
    return result;
  
  if (Readed == 2)
  {
    if (pNum)
      *pNum = (Buff[0] << 8) | Buff[1];
  }
  return 0;
}
//==============================================================================


//==============================================================================
// Функция поиска шаблонов в буферах CharBuffer1 и CharBuffer2 в библиотеке
//==============================================================================
int8_t zfm_Search(uint8_t CharBufferNum, uint16_t StartPage, uint16_t PageNum, uint16_t *pPageID, uint16_t *pMatchScore)
{
  uint16_t Readed;
  uint8_t Buff[5];
  Buff[0] = CharBufferNum;
  Buff[1] = StartPage >> 8;
  Buff[2] = StartPage & 0xFF;
  Buff[3] = PageNum >> 8;
  Buff[4] = PageNum & 0xFF;
  
  zfm_SendCommand(zfm_Addr, CommandPacket, Search, Buff, 5, 3000);
  uint8_t Buff2[4];
  int8_t result = zfm_ParseAckBlock(Ok, Buff2, &Readed);
  if (result < 0)
    return result;
  
  if (Readed == 4)
  {
    if (pPageID)
      *pPageID = (Buff2[0] << 8) | Buff2[1];
    if (pMatchScore)
      *pMatchScore = (Buff2[2] << 8) | Buff2[3];
  }
  return 0;
}
//==============================================================================


//==============================================================================
// Функция поиска шаблонов в буферах CharBuffer1 и CharBuffer2 в библиотеке.
// Отличается от Search новым кодом возврата residual fingerprint
//==============================================================================
int8_t zfm_SearchWithResidJud(uint8_t CharBufferNum, uint16_t StartPage, uint16_t PageNum, uint16_t *pPageID, uint16_t *pMatchScore)
{
  uint16_t Readed;
  uint8_t Buff[5];
  Buff[0] = CharBufferNum;
  Buff[1] = StartPage >> 8;
  Buff[2] = StartPage & 0xFF;
  Buff[3] = PageNum >> 8;
  Buff[4] = PageNum & 0xFF;
  
  zfm_SendCommand(zfm_Addr, CommandPacket, SearchWithResidJud, Buff, 5, 3000);
  uint8_t Buff2[4];
  int8_t result = zfm_ParseAckBlock(Ok, Buff2, &Readed);
  if (result < 0)
    return result;

  if (Readed == 4)
  {
    if (pPageID)
      *pPageID = (Buff2[0] << 8) | Buff2[1];
    if (pMatchScore)
      *pMatchScore = (Buff2[2] << 8) | Buff2[3];
  }
  return 0;
}
//==============================================================================


//==============================================================================
// Функция поиска приложенного отпечатка в библиотеке шаблонов.
// Объединяет 3 команды - GetImage, Img2TZ, Search.
//==============================================================================
int8_t zfm_AutoSearch(uint8_t FingerWaitTime, uint16_t StartPage, uint16_t PageNum, uint16_t *pPageID, uint16_t *pMatchScore)
{
  uint16_t Readed;
  uint8_t Buff[5];
  Buff[0] = FingerWaitTime;
  Buff[1] = StartPage >> 8;
  Buff[2] = StartPage & 0xFF;
  Buff[3] = PageNum >> 8;
  Buff[4] = PageNum & 0xFF;
  
  zfm_SendCommand(zfm_Addr, CommandPacket, AutoSearch, Buff, 5, 3000);
  uint8_t Buff2[4];
  int8_t result = zfm_ParseAckBlock(Ok, Buff2, &Readed);
  if (result < 0)
    return result;

  if (Readed == 4)
  {
    if (pPageID)
      *pPageID = (Buff2[0] << 8) | Buff2[1];
    if (pMatchScore)
      *pMatchScore = (Buff2[2] << 8) | Buff2[3];
  }
  return 0;
}
//==============================================================================


//==============================================================================
// Функция удаления нескольких шаблонов отпечатков из библиотеки
//==============================================================================
int8_t zfm_DeleteTemplates(uint16_t StartPage, uint16_t PageNum)
{
  uint8_t Buff[4];
  Buff[0] = StartPage >> 8;
  Buff[1] = StartPage & 0xFF;
  Buff[2] = PageNum >> 8;
  Buff[3] = PageNum & 0xFF;
  
  zfm_SendCommand(zfm_Addr, CommandPacket, DeleteTemplates, Buff, 4, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция удаления одного шаблона отпечатков из библиотеки
//==============================================================================
int8_t zfm_DeleteTemplate(uint16_t PageID)
{
  return zfm_DeleteTemplates(PageID, 1);
}
//==============================================================================


//==============================================================================
// Функция удаления нескольких шаблонов отпечатков из библиотеки
//==============================================================================
int8_t zfm_EmptyTemplates(void)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, EmptyTemplates, 0, 0, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция чтения таблицы занятости ячеек в библиотеке шаблонов отпечатков
//==============================================================================
int8_t zfm_ReadConList(uint8_t IndexPage, uint8_t *pTable)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, ReadConList, &IndexPage, 1, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, pTable, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция проверяет, занята ли ячейка в библиотеке шаблонов отпечатков
//==============================================================================
int8_t zfm_IsTemplateExists(uint16_t TemplateIndex, uint8_t *pExists)
{
  if (TemplateIndex > 1024)
    return ZFM_ERR_WRONG_ARGS;

  uint8_t Table[32];
  uint8_t IndexPage = TemplateIndex / 256;
  
  int8_t result = zfm_ReadConList(IndexPage, Table);
  if (result < 0)
    return result;
  
  uint8_t Idx = (uint16_t)TemplateIndex - ((uint16_t)IndexPage * 256);
  uint8_t Byte = Idx >> 3;
  uint8_t Bit = Idx % 8;
  *pExists = Table[Byte] & (1 << Bit) ? 1 : 0; 
    
  return 0;
}
//==============================================================================


//==============================================================================
// Функция включает подсветку сенсора
//==============================================================================
int8_t zfm_OpenLED(void)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, OpenLED, 0, 0, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция отключает подсветку сенсора
//==============================================================================
int8_t zfm_CloseLED(void)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, CloseLED, 0, 0, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция детектирования отпечатка без управления подсветкой сенсора
//==============================================================================
int8_t zfm_GetmageFreeLED(void)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, GetmageFreeLED, 0, 0, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(Ok, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция проверки готовности модуля к приёму команд
//==============================================================================
int8_t zfm_GetEcho(void)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, GetEcho, 0, 0, ZFM_TimeOutMs);
  return zfm_ParseAckBlock(EchoOk, 0, 0) >= 0 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// Функция сохранения нового отпечатка (с помощью групповой команды)
//==============================================================================
int8_t zfm_AutoLogin(uint8_t WaitTime, uint8_t FingerReadNum, uint16_t PageID, uint8_t AllowRepeatedTemplates)
{
  if ((FingerReadNum < 2) || (FingerReadNum > 3))
    return ZFM_ERR_WRONG_ARGS;
  if (AllowRepeatedTemplates > 1)
    return ZFM_ERR_WRONG_ARGS;
    
  uint16_t Readed;
  uint8_t Buff[5];
  Buff[0] = WaitTime;
  Buff[1] = FingerReadNum;
  Buff[2] = PageID >> 8;
  Buff[3] = PageID & 0xFF;
  Buff[4] = AllowRepeatedTemplates;
  
  zfm_SendCommand(zfm_Addr, CommandPacket, AutoLogin, Buff, 5, ZFM_TimeOutMs);

  int8_t result = zfm_ParseAckBlock(FirstFingerOk, 0, &Readed);
  if (result < 0)
    return result;

  if (FingerReadNum == 3)
  {
    zfm_Recv(2000);
    result = zfm_ParseAckBlock(SecondFingerOk, 0, &Readed);
    if (result < 0)
      return result;
 }

  zfm_Recv(2000);
    
  result = zfm_ParseAckBlock(Ok, 0, &Readed);
  if (result < 0)
    return result;
  
  return 0;
}
//==============================================================================


//==============================================================================
// Функция чтения буфера CharBuffer1 или CharBuffer2 из модуля
//==============================================================================
int8_t zfm_UploadChar(uint8_t CharBufferNum, uint8_t *pOutBuff, uint16_t *pReaded)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, UploadChar, &CharBufferNum, 1, 500);
  return zfm_GetDataBlock(Ok, pOutBuff, pReaded);
}
//==============================================================================


//==============================================================================
// Функция чтения изображения отпечатка из буфера ImageBuffer из модуля
//==============================================================================
int8_t zfm_UploadImage(uint8_t *pOutBuff, uint16_t *pReaded)
{
  zfm_SendCommand(zfm_Addr, CommandPacket, UploadImage, 0, 0, 200);
  return zfm_GetDataBlock(Ok, pOutBuff, pReaded);
}
//==============================================================================


//==============================================================================
// Инициализация ZFM
//==============================================================================
int8_t zfm_Init(USART_TypeDef* USARTx, uint32_t BaudRate, uint8_t *pAddr, uint8_t *pPassword)
{
  // Инициализируем буферы
  zfm_InitBuff();
  // Запоминаем текущий адрес модуля
  memcpy(zfm_Addr, pAddr, 4);

  delay_ms(300);

  // Инициализируем UART
  UART_Init(USARTx, BaudRate, zfm_Rx);
  // Ждём в течении 300 мс байт 0x55, означающий, что модуль готов принимать команды
  zfm_Recv(300);

  // Авторизуемся паролем
  int8_t result = zfm_VerifyPassword(pPassword);
  if (result < 0)
    return result;
  
  // Читаем параметры сенсора
  result = zfm_ReadSysParam(0);
  if (result < 0)
    return result;

  // Читаем системные параметры ZFM в структуру в драйвере ZFM
  zfm_ReadSysParam(0);
  
#if ZFM_GetInfoFunction
  // Читаем модель и производителя сенсора
  result = zfm_GetInfo();
#endif
  
  return result;
}
//==============================================================================

