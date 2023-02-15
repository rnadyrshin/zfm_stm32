//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _ZFM_CMD_H
#define _ZFM_CMD_H



// Функция поиска приложенного к сенсору отпечатка в библиотеке шаблонов
int8_t Search_Finger(uint16_t *pPageID, uint16_t *pMatchScore, uint8_t ManualLED);
// Функция поиска приложенного к сенсору отпечатка в библиотеке шаблонов с помощью групповой команды
int8_t AutoSearch_Finger(uint16_t *pPageID, uint16_t *pMatchScore);
// Функция добавления нового приложенного к сенсору отпечатка в библиотеку шаблонов
int8_t Enroll_Finger(uint16_t PageID, uint32_t FingerWaitNum, uint8_t ManualLED);
// Функция добавления нового приложенного к сенсору отпечатка в библиотеку шаблонов с помощью групповой команды
int8_t Enroll_Finger_GroupCmd(uint16_t PageID);
// Функция чтения свёртки приложенного к сенсору отпечатка.
// pBuff - указатель на буфер размером не менее 512 байта.
int8_t GetFingerChar(uint8_t *pBuff, uint8_t ManualLED);
// Функция чтения фотографии приложенного к сенсору отпечатка.
// pBuff - указатель на буфер размером не менее 36864 байта.
// В каждом байте - 2 пикселя в 4-битном цвете (16 градаций серого).
int8_t GetFingerImage(uint8_t *pBuff, uint8_t ManualLED);


#endif