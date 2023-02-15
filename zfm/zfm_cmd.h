//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _ZFM_COMMON_H
#define _ZFM_COMMON_H

#include <stdint.h>


#define ZFM_TimeOutMs           500      // ������� ������� ������ �� ������� � ��
#define ZFM_STARTCODE           0xEF01

#define ZFM_GetDataBlock_DEBUG  1       // ����� ������� ������ ����� ������ �� ZFM (�� ����� ������ � �������� �����)

// ������������ ���� ������
#define ZFM_ERR_NO_ANSVER       (-3)    // ��� ������ - ���� ����-��� �������� ������
#define ZFM_ERR_NO_RX_BUFF      (-4)    // ��� ������ - �� ������� ��������� �� ������� �����
#define ZFM_ERR_WRONG_ARGS      (-5)    // ��� ������ - ������ � ���������� �������
#define ZFM_ERR_CHECK_SUM       (-6)    // ��� ������ - �� �������� ����������� �����
#define ZFM_ERR_TOOSMALL_RXBUFF (-7)    // ��� ������ - �� ������� ����� � ������� ������ ���� ������
#define ZFM_ERR_NOTFULL_PACKET  (-8)    // ��� ������ - �� ������� ����� � ������� ������ ���� ������

// Package identifiers
typedef enum
{
  CommandPacket         = 1,
  DataPacket 	        = 2,
  AckPacket 	        = 7,
  EndDataPacket         = 8
}
eZFM_PID;

// ���� ������
typedef enum
{
  GetImage 		= 0x01, // * ������� �������������� ������ � ��������� ����������� ���������
  Image2TZ 		= 0x02, // *
  Match                 = 0x03, // * ����������� � ���������� arduino
  Search                = 0x04, // * ������� ������ ��������� � ����������. ����������� � ���������� arduino
  RegModel 		= 0x05, // *
  StoreTemplate		= 0x06, // * ������� ������ ������� � ��������� �������� ����������
  LoadTemplate  	= 0x07, // * ������� ������ ������� �� ��������� �������� ����������
  UploadChar 		= 0x08, // * ������� ������ ������ ���������
  DownloadChar          = 0x09, // ������� ������ ������ ���������. �� �����������
  UploadImage           = 0x0A, // * ������� ������ �������� ��������� �� ImageBuffer
  DownloadImage         = 0x0B, // ������� ������ �������� ��������� � ImageBuffer. �� �����������
  DeleteTemplates       = 0x0C, // *
  EmptyTemplates	= 0x0D, // *
  SetSysParam 	        = 0x0E,	// * ����������� � ���������� arduino
  ReadSysParams	        = 0x0F,	// * ����������� � ���������� arduino
  SetPassword 	        = 0x12, // *
  VerifyPassword        = 0x13, // *
  SetAddr 		= 0x15, // *
  GetInfo 		= 0x16,	// * ������� ������ ���������� � ������. ����������� � ���������� arduino � � ������������
  HiSpeedSearch         = 0x1B, // ������� �������� ������ ���������. �� �������� � SFG Demo, ����������� � ������������. �� �����������
  TemplateCount         = 0x1D, // * ������� ������ ���-�� �������� �������� ����������
  ReadConList           = 0x1F, // * ������� ������ ������� ��������� ����� � ���������� �������� ����������
  OpenLED               = 0x50, // * ������� ��������� ���������
  CloseLED              = 0x51, // * ������� ���������� ���������
  GetmageFreeLED        = 0x52, // * ������� �������������� ������ � ��������� ����������� ��������� ��� ���������� ����������
  GetEcho 		= 0x53, // * ������� �������� ���������� ������ � ����� ������
  AutoLogin             = 0x54, // * ��������� ������� ��� ���������� ������ ���������
  AutoSearch            = 0x55,	// * ��������� ������� ������ ������������ ������
  SearchWithResidJud    = 0x56  // * ������� ������ ��������� � ����������. ���������� �� Search ����� ����� �������� residual fingerprint
}
eZFM_CmdCode;

// ���� ������ �� ������� (Confirmation code)
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

// ������ ��������� ���������� ������ (system basic parameter). ������������ ��� ������ ������ ���������� ���������
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
  uint8_t AnswerWaiting         :1;     // ��� �������� ������ �� �������
  uint8_t AnswerReceived        :1;     // ����� �� ������� �������
  uint8_t BufferFull            :1;     // ������� ����� ��������, ���� ��� �� ����������
  uint16_t Len;
}
tZFM_state;
extern tZFM_state zfm_state;


// ��������� �������������� ������
void zfm_InitBuff(void);
// ������� �������� ������ � ZFM
int8_t zfm_Send(uint8_t *pAddr, uint8_t PID, uint8_t InstrCode, uint8_t *pBuff, uint16_t Len);
// ������� ����� ������
int16_t zfm_Recv(uint16_t TimeOutMs);
// ������� �������� ������� � ������� � ��������� ������ � �������� ����� ������
int16_t zfm_SendCommand(uint8_t *pAddr, eZFM_PID PID, eZFM_CmdCode InstrCode, uint8_t *pBuff, uint16_t Len, uint16_t TimeOutMs);
// ������� ������� ������ �� ������
int8_t zfm_ParsePacket(uint8_t *pBuff, int16_t Len, uint8_t *pPID, uint8_t *pConfCode, uint8_t *pRxBuff, uint16_t *pReaded);
// �������-���������� ���� ������� �� ������� (���������� �� ����������� ����������!)
void zfm_Rx(void);
// ������� ������� ACK-������
int8_t zfm_ParseAckBlock(eZFM_ConfCode Confirmation, uint8_t *pOutBuff, uint16_t *pReaded);
// ������� ������� ������� � �������
int8_t zfm_ParseDataBlock(uint8_t *pOutBuff, uint16_t *pReaded);
// ������� ������ ����� ������ �� ZFM � ����� � ������
int8_t zfm_GetDataBlock(eZFM_ConfCode ConfCode, uint8_t *pOutBuff, uint16_t *pReaded);


#endif