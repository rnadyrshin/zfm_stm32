//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <delay.h>
#include "zfm.h"
#include "zfm_cmd.h"


//==============================================================================
// Функция поиска приложенного к сенсору отпечатка в библиотеке шаблонов
//==============================================================================
int8_t Search_Finger(uint16_t *pPageID, uint16_t *pMatchScore, uint8_t ManualLED)
{
  uint16_t TemplateNum = 0;
  int8_t result;
  
  // Читаем количество шаблонов отпечатков в библиотеке
  result = zfm_ReadTemplateNum(&TemplateNum);
  if (result != 0)
    return result;

  // Детектируем палец и сохраняем картинку в буфер
  if (ManualLED)
    result = zfm_GetmageFreeLED();
  else
    result = zfm_GetImage();
  if (result != 0)
    return result;

  // Делаем свёртку в CharBuffer1
  result = zfm_Img2Tz(1);   // Делаем свёртку в буфер 1
  if (result != 0)
    return result;

  // Запускаем поиск шаблона отпечатка в библиотеке шаблонов
  result = zfm_Search(1, 0, TemplateNum, pPageID, pMatchScore);   
  //result = zfm_SearchWithResidJud(1, 0, TemplateNum, pPageID, pMatchScore);   // Другой вариант (отличия внутри в доп. кодах возврата)
  if (result != 0)
    return result;
  
  return 0;
}
//==============================================================================


//==============================================================================
// Функция поиска приложенного к сенсору отпечатка в библиотеке шаблонов
// с помощью групповой команды
//==============================================================================
int8_t AutoSearch_Finger(uint16_t *pPageID, uint16_t *pMatchScore)
{
  uint16_t TemplateNum = 0;
  int8_t result;

  // Читаем количество шаблонов отпечатков в библиотеке
  result = zfm_ReadTemplateNum(&TemplateNum);
  if (result != 0)
    return result;

  // Запускаем поиск шаблона отпечатка в библиотеке шаблонов
  result = zfm_AutoSearch(31, 0, TemplateNum, pPageID, pMatchScore);   
  if (result != 0)
    return result;
  
  return 0;
}
//==============================================================================


//==============================================================================
// Функция добавления нового приложенного к сенсору отпечатка в библиотеку шаблонов
//==============================================================================
int8_t Enroll_Finger(uint16_t PageID, uint32_t FingerWaitNum, uint8_t ManualLED)
{
  int8_t result;
  uint32_t FingerWaitCnt = 0;
  
  if (!FingerWaitNum)
    return ZFM_ERR_WRONG_ARGS;
  
  // Получаем первый отпечаток
  while (FingerWaitCnt++ < FingerWaitNum)
  {
    // Детектируем палец и сохраняем картинку в буфер
    if (ManualLED)
      result = zfm_GetmageFreeLED();
    else
      result = zfm_GetImage();
    if (result == 0)
      break;
  }
  if (result != 0)      // За отведённое кол-во попыток палец так и не был обнаружен
    return result;
  
  // Делаем свёртку в CharBuffer1
  result = zfm_Img2Tz(1);   // Делаем свёртку в буфер 1
  if (result != 0)
    return result;

  // Получаем второй отпечаток
  while (FingerWaitCnt++ < FingerWaitNum)
  {
    // Детектируем палец и сохраняем картинку в буфер
    if (ManualLED)
      result = zfm_GetmageFreeLED();
    else
      result = zfm_GetImage();
    if (result == 0)
      break;
  }
  if (result != 0)      // За отведённое кол-во попыток палец так и не был обнаружен
    return result;
  
  // Делаем свёртку в CharBuffer2
  result = zfm_Img2Tz(2);   // Делаем свёртку в буфер 2
  if (result != 0)
    return result;

  // Делаем шаблон из свёртки
  result = zfm_RegModel(); 
  if (result != 0)
    return result;

  // Сохраняем шаблон отпечатка (из CharBuffer2) в библиотеку шаблонов
  zfm_StoreTemplate(2, PageID);
  if (result != 0)
    return result;

  // Читаем шаблон отпечатка из библиотеки шаблонов в буфер CharBuff2
  zfm_LoadTemplate(2, PageID);
  if (result != 0)
    return result;

  FingerWaitCnt = 0;
  while (FingerWaitCnt++ < FingerWaitNum)
  {
    // Детектируем палец и сохраняем картинку в буфер
    if (ManualLED)
      result = zfm_GetmageFreeLED();
    else
      result = zfm_GetImage();
    if (result == 0)
      break;
  }
  
  if (result != 0)      // За отведённое кол-во попыток палец так и не был обнаружен
    return result;

  // Делаем свёртку в CharBuffer
  result = zfm_Img2Tz(1);   // Делаем свёртку в буфер 1
  if (result != 0)
    return result;

  // Сравниваем шаблоны 2 отпечатков
  uint16_t MatchScore;
  result = zfm_Match(&MatchScore);
  if (result != 0)
    return result;

  return 0;
}
//==============================================================================


//==============================================================================
// Функция добавления нового приложенного к сенсору отпечатка в библиотеку шаблонов
// с помощью групповой команды
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
// Функция чтения свёртки приложенного к сенсору отпечатка.
// pBuff - указатель на буфер размером не менее 512 байта.
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

  result = zfm_Img2Tz(1);   // Делаем свёртку в буфер 1
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
// Функция чтения фотографии приложенного к сенсору отпечатка.
// pBuff - указатель на буфер размером не менее 36864 байта.
// В каждом байте - 2 пикселя в 4-битном цвете (16 градаций серого).
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

