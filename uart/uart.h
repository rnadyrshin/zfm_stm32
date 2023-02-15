//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _UART_H
#define _UART_H


#define UART_MODE_POLLING       1
#define UART_MODE_IRQ           2

#define UART_TX_MODE            UART_MODE_POLLING

#define UartTxBuffSize          256
#define UartRxBuffSize          512

#if (UART_TX_MODE == UART_MODE_POLLING)
#define UART_XT_TIMEOUT_BYTE    50      // ������� � 10 ��� ���������� �������� �������� �����
#endif


// ������������ ���� ������
#define UART_ERR_OK             1       // ��� ������ - ��������� ��� ������
#define UART_ERR_BUFF_OVF       (-1)    // ��� ������ - ������������ ������
#define UART_ERR_HW_TIMEOUT     (-2)    // ��� ������ - ���� ����-��� �������������� ��������


// ��� ��������� �� ������� ��������� ��������� ������
typedef void (*uart_RxFunc)(void);


// ������������� UART. RxFunc - �������, ���������� ��� ��������� ������ � ��������� Nextion
void UART_Init(USART_TypeDef* USARTx, uint32_t BaudRate, uart_RxFunc RxFunc);
// ������� �������� ������
int8_t UART_Send(uint8_t *pBuff, uint16_t Len);
// ������� ���������� ���-�� ���� � Rx ������
uint16_t USART_RxDataToProc(void);
// ������� ��������� ���������� ����� �� Rx ������������ ������
uint8_t USART_GetByteToProc(void);


#endif