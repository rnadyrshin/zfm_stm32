//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _ZFM_H
#define _ZFM_H

#include "zfm_cmd.h"
#include <stm32f10x_usart.h>


// Будет ли использоваться команда GetInfo чтения информации о модуле
#define ZFM_GetInfoFunction     1       // Функция читает модель и производителя сенсора (требует 512 байт ОЗУ)

typedef struct
{
  uint16_t SystemStatusReg;
  uint16_t FingerLibSize;
  uint8_t SecurityLevel;
  uint32_t DeviceAddr;
  eZFM_PaketLen PacketSize;
  eZFM_BaudCode BaudCode;
  char SensorModel[9];          // Модель сенсора (8 символов)
  char SensorVendor[9];         // Производитель сенсора (8 символов)
}
tSensorParams;


// Инициализация ZFM
int8_t zfm_Init(USART_TypeDef* USARTx, uint32_t BaudRate, uint8_t *pAddr, uint8_t *pPassword);
// Функция проверки пароля доступа к устройству
int8_t zfm_VerifyPassword(uint8_t *pPassword);
// Функция установки пароля доступа к устройству
int8_t zfm_SetPassword(uint8_t *pPassword);
// Функция установки адреса модуля
int8_t zfm_SetAddr(uint8_t *pNewAddr);
// Функция записи BaudRate
int8_t zfm_SetBaudRate(eZFM_BaudCode BaudCode);
// Функция записи Security Level
int8_t zfm_SetSecurityLevel(uint8_t SecurityLevel);
// Функция записи Packet Len
int8_t zfm_SetPacketLen(eZFM_PaketLen NewPacketLen);
// Функция чтения всех системных параметров из ZFM
int8_t zfm_ReadSysParam(tSensorParams *pStruct);
// Функция чтения всех системных параметров ZFM. 
// Функция не читает их из ZFM, а использует параметры, сохранённые в структуре zfm_Params в драйвере
void zfm_GetSysParam(tSensorParams *pStruct);
// Функция инициирует детектирование отпечатка и сохранение его в ImageBuffer
int8_t zfm_GetImage(void);
// Функция инициирует свёртку отпечатка из буфера Image Buffer и сохранение её в Char Buffer 1 или Char Buffer 2 
int8_t zfm_Img2Tz(uint8_t CharBufferNum);
// Функция инициирует генерирование шаблона отпечатка по данным в буферах Char Buffer 1 и Char Buffer 2 
int8_t zfm_RegModel(void);
// Функция сохранения шаблона отпечатка в библиотеку шаблонов. 
int8_t zfm_StoreTemplate(uint8_t CharBufferNum, uint16_t PageID);
// Функция чтения шаблона отпечатка из библиотеки шаблонов в буфер CharBuffer
int8_t zfm_LoadTemplate(uint8_t CharBufferNum, uint16_t PageID);
// Функция инициирует сравнение шаблонов в буферах CharBuffer1 и CharBuffer2. 
// Сохраняет по указателю pMatchScore коэффициент совпадения.
int8_t zfm_Match(uint16_t *pMatchScore);
// Функция читает кол-во корректных шаблонов отпечатков
int8_t zfm_ReadTemplateNum(uint16_t *pNum);
// Функция поиска шаблонов в буферах CharBuffer1 и CharBuffer2 в библиотеке
int8_t zfm_Search(uint8_t CharBufferNum, uint16_t StartPage, uint16_t PageNum, uint16_t *pPageID, uint16_t *pMatchScore);
// Функция поиска шаблонов в буферах CharBuffer1 и CharBuffer2 в библиотеке. Отличается от Search новым кодом возврата residual fingerprint
int8_t zfm_SearchWithResidJud(uint8_t CharBufferNum, uint16_t StartPage, uint16_t PageNum, uint16_t *pPageID, uint16_t *pMatchScore);
// Функция поиска приложенного отпечатка в библиотеке шаблонов. Объединяет 3 команды - GetImage, Img2TZ, Search.
int8_t zfm_AutoSearch(uint8_t FingerWaitTime, uint16_t StartPage, uint16_t PageNum, uint16_t *pPageID, uint16_t *pMatchScore);
// Функция удаления нескольких шаблонов отпечатков из библиотеки
int8_t zfm_DeleteTemplates(uint16_t StartPage, uint16_t PageNum);
// Функция удаления одного шаблона отпечатков из библиотеки
int8_t zfm_DeleteTemplate(uint16_t PageID);
// Функция удаления нескольких шаблонов отпечатков из библиотеки
int8_t zfm_EmptyTemplates(void);
// Функция чтения таблицы занятости ячеек в библиотеке шаблонов отпечатков
int8_t zfm_ReadConList(uint8_t IndexPage, uint8_t *pTable);
// Функция проверяет, занята ли ячейка в библиотеке шаблонов отпечатков
int8_t zfm_IsTemplateExists(uint16_t TemplateIndex, uint8_t *pExists);
// Функция включает подсветку сенсора
int8_t zfm_OpenLED(void);
// Функция отключает подсветку сенсора
int8_t zfm_CloseLED(void);
// Функция детектирования отпечатка без управления подсветкой сенсора
int8_t zfm_GetmageFreeLED(void);
// Функция сохранения нового отпечатка (с помощью групповой команды)
int8_t zfm_AutoLogin(uint8_t WaitTime, uint8_t FingerReadNum, uint16_t PageID, uint8_t AllowRepeatedTemplates);
// Функция чтения буфера CharBuffer1 или CharBuffer2 из модуля
int8_t zfm_UploadChar(uint8_t CharBufferNum, uint8_t *pOutBuff, uint16_t *pReaded);
// Функция чтения изображения отпечатка из буфера ImageBuffer из модуля
int8_t zfm_UploadImage(uint8_t *pOutBuff, uint16_t *pReaded);


#endif