//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _ZFM_CMD_H
#define _ZFM_CMD_H



// ������� ������ ������������ � ������� ��������� � ���������� ��������
int8_t Search_Finger(uint16_t *pPageID, uint16_t *pMatchScore, uint8_t ManualLED);
// ������� ������ ������������ � ������� ��������� � ���������� �������� � ������� ��������� �������
int8_t AutoSearch_Finger(uint16_t *pPageID, uint16_t *pMatchScore);
// ������� ���������� ������ ������������ � ������� ��������� � ���������� ��������
int8_t Enroll_Finger(uint16_t PageID, uint32_t FingerWaitNum, uint8_t ManualLED);
// ������� ���������� ������ ������������ � ������� ��������� � ���������� �������� � ������� ��������� �������
int8_t Enroll_Finger_GroupCmd(uint16_t PageID);
// ������� ������ ������ ������������ � ������� ���������.
// pBuff - ��������� �� ����� �������� �� ����� 512 �����.
int8_t GetFingerChar(uint8_t *pBuff, uint8_t ManualLED);
// ������� ������ ���������� ������������ � ������� ���������.
// pBuff - ��������� �� ����� �������� �� ����� 36864 �����.
// � ������ ����� - 2 ������� � 4-������ ����� (16 �������� ������).
int8_t GetFingerImage(uint8_t *pBuff, uint8_t ManualLED);


#endif