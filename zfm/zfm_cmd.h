//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _ZFM_COMMON_H
#define _ZFM_COMMON_H

#include <stdint.h>


#define ZFM_TimeOutMs           500      // Таймаут ожиания ответа от дисплея в мс
#define ZFM_STARTCODE           0xEF01

#define ZFM_GetDataBlock_DEBUG  1       // Режим отладки чтения блока данных из ZFM (не пишет ничего в выходной буфер)

// Возвращаемые коды ошибок
#define ZFM_ERR_NO_ANSVER       (-3)    // Код ошибки - истёк тайм-аут ожидания ответа
#define ZFM_ERR_NO_RX_BUFF      (-4)    // Код ошибки - не передан указатель на приёмный буфер
#define ZFM_ERR_WRONG_ARGS      (-5)    // Код ошибки - ошибка в аргументах команды
#define ZFM_ERR_CHECK_SUM       (-6)    // Код ошибки - не сходится контрольная сумма
#define ZFM_ERR_TOOSMALL_RXBUFF (-7)    // Код ошибки - не хватает места в приёмном буфере тела пакета
#define ZFM_ERR_NOTFULL_PACKET  (-8)    // Код ошибки - не хватает места в приёмном буфере тела пакета

// Package identifiers
typedef enum
{
  CommandPacket         = 1,
  DataPacket 	        = 2,
  AckPacket 	        = 7,
  EndDataPacket         = 8
}
eZFM_PID;

// Коды команд
typedef enum
{
  GetImage 		= 0x01, // * Команда детектирования пальца и получения изображения отпечатка
  Image2TZ 		= 0x02, // *
  Match                 = 0x03, // * Отсутствует в библиотеке arduino
  Search                = 0x04, // * Команда поиска отпечатка в библиотеке. Отсутствует в библиотеке arduino
  RegModel 		= 0x05, // *
  StoreTemplate		= 0x06, // * Команда записи шаблона в библитеку шаблонов отпечатков
  LoadTemplate  	= 0x07, // * Команда чтения шаблона из библитеки шаблонов отпечатков
  UploadChar 		= 0x08, // * Команда чтения свёртки отпечатка
  DownloadChar          = 0x09, // Команда записи свёртки отпечатка. Не реализована
  UploadImage           = 0x0A, // * Команда чтения картинки отпечатка из ImageBuffer
  DownloadImage         = 0x0B, // Команда записи картинки отпечатка в ImageBuffer. Не реализована
  DeleteTemplates       = 0x0C, // *
  EmptyTemplates	= 0x0D, // *
  SetSysParam 	        = 0x0E,	// * Отсутствует в библиотеке arduino
  ReadSysParams	        = 0x0F,	// * Отсутствует в библиотеке arduino
  SetPassword 	        = 0x12, // *
  VerifyPassword        = 0x13, // *
  SetAddr 		= 0x15, // *
  GetInfo 		= 0x16,	// * Команда чтения информации о модуле. Отсутствует в библиотеке arduino и в документации
  HiSpeedSearch         = 0x1B, // Команда быстрого поиска отпечатка. Не работает в SFG Demo, отсутствует в документации. Не реализована
  TemplateCount         = 0x1D, // * Команда чтения кол-ва валидных шаблонов отпечатков
  ReadConList           = 0x1F, // * Команда чтения таблицы занятости ячеек в библиотеке шаблонов отпечатков
  OpenLED               = 0x50, // * Команда включения подсветки
  CloseLED              = 0x51, // * Команда отключения подсветки
  GetmageFreeLED        = 0x52, // * Команда детектирования пальца и получения изображения отпечатка без управления подсветкой
  GetEcho 		= 0x53, // * Команда проверки готовности модуля к приёму команд
  AutoLogin             = 0x54, // * Групповая команда для сохранения нового отпечатка
  AutoSearch            = 0x55,	// * Групповая команда поиска приложенного пальца
  SearchWithResidJud    = 0x56  // * Команда поиска отпечатка в библиотеке. Отличается от Search новым кодом возврата residual fingerprint
}
eZFM_CmdCode;

// Коды ответа на команду (Confirmation code)
typedef enum
{
  Ok                    = 0x00,
  PacketReceiveErr      = 0x01,
  NoFinger 		= 0x02,
  ImageFail 		= 0x03,
  ImageMess 		= 0x06,
  FeatureFail 		= 0x07,
  NoMatch 		= 0x08,
  NotFound 		= 0x09,
  EnrollMismatch 	= 0x0A,
  BadLocation 		= 0x0B,
  DBRangeFail 		= 0x0C,
  UploadFeatureFail     = 0x0D,
  PacketResponseFail    = 0x0E,
  UploadFail 		= 0x0F,
  DeleteFail 		= 0x10,
  DBClearFail 		= 0x11,
  PassFail 		= 0x13,
  InvalidImage 		= 0x15,
  FlashErr 		= 0x18,
  InvalidReg 		= 0x1A,
  AddrCode 		= 0x20,
  PassVerify 		= 0x21,
  EchoOk                = 0x55,
  FirstFingerOk         = 0x56,
  SecondFingerOk        = 0x57
} 
eZFM_ConfCode;

// Номера системных параметров модуля (system basic parameter). Используется при записи одного системного параметра
typedef enum
{
  BaudRate 		= 4,
  SecurityLevel         = 5,
  PacketLen 	        = 6	
} 
eZFM_SysParamNum;

typedef enum
{
  baud9600 	= 1,
  baud18200 	= 2,
  baud28800 	= 3,
  baud38400 	= 4,
  baud48000 	= 5,
  baud57600 	= 6,
  baud67200 	= 7,
  baud76800 	= 8,
  baud86400 	= 9,
  baud96000 	= 10,
  baud105600 	= 11,
  baud115200 	= 12
}
eZFM_BaudCode;

typedef enum
{
  pak32bytes	= 0,
  pak64bytes 	= 1,
  pak128bytes  	= 2,
  pak256bytes 	= 3
}
eZFM_PaketLen;

typedef struct
{
  uint8_t *pAnswerBuff;
  uint8_t AnswerWaiting         :1;     // Идёт ожидание ответа на команду
  uint8_t AnswerReceived        :1;     // Ответ на команду получен
  uint8_t BufferFull            :1;     // Приёмный буфер заполнен, приём ещё не закончился
  uint16_t Len;
}
tZFM_state;
extern tZFM_state zfm_state;


// Процедура инициализирует буферы
void zfm_InitBuff(void);
// Функция отправки пакета в ZFM
int8_t zfm_Send(uint8_t *pAddr, uint8_t PID, uint8_t InstrCode, uint8_t *pBuff, uint16_t Len);
// Функция приёма ответа
int16_t zfm_Recv(uint16_t TimeOutMs);
// Функция отправки команды в дисплей с ожиданием ответа с заданным кодом ответа
int16_t zfm_SendCommand(uint8_t *pAddr, eZFM_PID PID, eZFM_CmdCode InstrCode, uint8_t *pBuff, uint16_t Len, uint16_t TimeOutMs);
// Функция разбора ответа от модуля
int8_t zfm_ParsePacket(uint8_t *pBuff, int16_t Len, uint8_t *pPID, uint8_t *pConfCode, uint8_t *pRxBuff, uint16_t *pReaded);
// Функция-обработчик всех посылок из дисплея (вызывается из обработчика прерывания!)
void zfm_Rx(void);
// Функция разбора ACK-пакета
int8_t zfm_ParseAckBlock(eZFM_ConfCode Confirmation, uint8_t *pOutBuff, uint16_t *pReaded);
// Функция разбора пакетов с данными
int8_t zfm_ParseDataBlock(uint8_t *pOutBuff, uint16_t *pReaded);
// Функция чтения блока данных из ZFM в буфер в памяти
int8_t zfm_GetDataBlock(eZFM_ConfCode ConfCode, uint8_t *pOutBuff, uint16_t *pReaded);


#endif