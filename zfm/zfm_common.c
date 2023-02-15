//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <delay.h>
#include "zfm.h"
#include "zfm_cmd.h"


//==============================================================================
// ������� ������ ������������ � ������� ��������� � ���������� ��������
//==============================================================================
int8_t Search_Finger(uint16_t *pPageID, uint16_t *pMatchScore, uint8_t ManualLED)
{
  uint16_t TemplateNum = 0;
  int8_t result;
  
  // ������ ���������� �������� ���������� � ����������
  result = zfm_ReadTemplateNum(&TemplateNum);
  if (result != 0)
    return result;

  // ����������� ����� � ��������� �������� � �����
  if (ManualLED)
    result = zfm_GetmageFreeLED();
  else
    result = zfm_GetImage();
  if (result != 0)
    return result;

  // ������ ������ � CharBuffer1
  result = zfm_Img2Tz(1);   // ������ ������ � ����� 1
  if (result != 0)
    return result;

  // ��������� ����� ������� ��������� � ���������� ��������
  result = zfm_Search(1, 0, TemplateNum, pPageID, pMatchScore);   
  //result = zfm_SearchWithResidJud(1, 0, TemplateNum, pPageID, pMatchScore);   // ������ ������� (������� ������ � ���. ����� ��������)
  if (result != 0)
    return result;
  
  return 0;
}
//==============================================================================


//==============================================================================
// ������� ������ ������������ � ������� ��������� � ���������� ��������
// � ������� ��������� �������
//==============================================================================
int8_t AutoSearch_Finger(uint16_t *pPageID, uint16_t *pMatchScore)
{
  uint16_t TemplateNum = 0;
  int8_t result;

  // ������ ���������� �������� ���������� � ����������
  result = zfm_ReadTemplateNum(&TemplateNum);
  if (result != 0)
    return result;

  // ��������� ����� ������� ��������� � ���������� ��������
  result = zfm_AutoSearch(31, 0, TemplateNum, pPageID, pMatchScore);   
  if (result != 0)
    return result;
  
  return 0;
}
//==============================================================================


//==============================================================================
// ������� ���������� ������ ������������ � ������� ��������� � ���������� ��������
//==============================================================================
int8_t Enroll_Finger(uint16_t PageID, uint32_t FingerWaitNum, uint8_t ManualLED)
{
  int8_t result;
  uint32_t FingerWaitCnt = 0;
  
  if (!FingerWaitNum)
    return ZFM_ERR_WRONG_ARGS;
  
  // �������� ������ ���������
  while (FingerWaitCnt++ < FingerWaitNum)
  {
    // ����������� ����� � ��������� �������� � �����
    if (ManualLED)
      result = zfm_GetmageFreeLED();
    else
      result = zfm_GetImage();
    if (result == 0)
      break;
  }
  if (result != 0)      // �� ��������� ���-�� ������� ����� ��� � �� ��� ���������
    return result;
  
  // ������ ������ � CharBuffer1
  result = zfm_Img2Tz(1);   // ������ ������ � ����� 1
  if (result != 0)
    return result;

  // �������� ������ ���������
  while (FingerWaitCnt++ < FingerWaitNum)
  {
    // ����������� ����� � ��������� �������� � �����
    if (ManualLED)
      result = zfm_GetmageFreeLED();
    else
      result = zfm_GetImage();
    if (result == 0)
      break;
  }
  if (result != 0)      // �� ��������� ���-�� ������� ����� ��� � �� ��� ���������
    return result;
  
  // ������ ������ � CharBuffer2
  result = zfm_Img2Tz(2);   // ������ ������ � ����� 2
  if (result != 0)
    return result;

  // ������ ������ �� ������
  result = zfm_RegModel(); 
  if (result != 0)
    return result;

  // ��������� ������ ��������� (�� CharBuffer2) � ���������� ��������
  zfm_StoreTemplate(2, PageID);
  if (result != 0)
    return result;

  // ������ ������ ��������� �� ���������� �������� � ����� CharBuff2
  zfm_LoadTemplate(2, PageID);
  if (result != 0)
    return result;

  FingerWaitCnt = 0;
  while (FingerWaitCnt++ < FingerWaitNum)
  {
    // ����������� ����� � ��������� �������� � �����
    if (ManualLED)
      result = zfm_GetmageFreeLED();
    else
      result = zfm_GetImage();
    if (result == 0)
      break;
  }
  
  if (result != 0)      // �� ��������� ���-�� ������� ����� ��� � �� ��� ���������
    return result;

  // ������ ������ � CharBuffer
  result = zfm_Img2Tz(1);   // ������ ������ � ����� 1
  if (result != 0)
    return result;

  // ���������� ������� 2 ����������
  uint16_t MatchScore;
  result = zfm_Match(&MatchScore);
  if (result != 0)
    return result;

  return 0;
}
//==============================================================================


//==============================================================================
// ������� ���������� ������ ������������ � ������� ��������� � ���������� ��������
// � ������� ��������� �������
//==============================================================================
int8_t Enroll_Finger_GroupCmd(uint16_t PageID)
{
  int8_t result;
  
  result = zfm_AutoLogin(31, 3, PageID, 0);
  if (result != 0)
    return result;
  
  return 0;
}
//==============================================================================


//==============================================================================
// ������� ������ ������ ������������ � ������� ���������.
// pBuff - ��������� �� ����� �������� �� ����� 512 �����.
//==============================================================================
int8_t GetFingerChar(uint8_t *pBuff, uint8_t ManualLED)
{
  int8_t result;
  
  if (ManualLED)
    result = zfm_GetmageFreeLED();
  else
    result = zfm_GetImage();
  
  if (result != 0)
    return result;

  result = zfm_Img2Tz(1);   // ������ ������ � ����� 1
  if (result != 0)
    return result;

  uint16_t Readed = 0;
  result = zfm_UploadChar(1, pBuff, &Readed);
  if (result != 0)
    return result;
  
  return Readed == 512 ? 0 : -1;
}
//==============================================================================


//==============================================================================
// ������� ������ ���������� ������������ � ������� ���������.
// pBuff - ��������� �� ����� �������� �� ����� 36864 �����.
// � ������ ����� - 2 ������� � 4-������ ����� (16 �������� ������).
//==============================================================================
int8_t GetFingerImage(uint8_t *pBuff, uint8_t ManualLED)
{
  int8_t result;
  
  if (ManualLED)
    result = zfm_GetmageFreeLED();
  else
    result = zfm_GetImage();

  if (result != 0)
    return result;
    
  uint16_t Readed = 0;
  result = zfm_UploadImage(pBuff, &Readed);
  if (result != 0)
    return result;
  
  return Readed == 36864 ? 0 : -1;
}
//==============================================================================

