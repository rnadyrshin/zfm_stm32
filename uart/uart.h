//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _UART_H
#define _UART_H


#define UART_MODE_POLLING       1
#define UART_MODE_IRQ           2

#define UART_TX_MODE            UART_MODE_POLLING

#define UartTxBuffSize          256
#define UartRxBuffSize          512

#if (UART_TX_MODE == UART_MODE_POLLING)
#define UART_XT_TIMEOUT_BYTE    50      // Таймаут в 10 мкс интервалах ожидания отправки байта
#endif


// Возвращаемые коды ошибок
#define UART_ERR_OK             1       // Код ошибки - выполнено без ошибок
#define UART_ERR_BUFF_OVF       (-1)    // Код ошибки - переполнение буфера
#define UART_ERR_HW_TIMEOUT     (-2)    // Код ошибки - истёк тайм-аут низкоуровневых операций


// Тип указателя на функцию обработки входящего пакета
typedef void (*uart_RxFunc)(void);


// Инициализация UART. RxFunc - функция, вызываемая при получении пакета с суффиксом Nextion
void UART_Init(USART_TypeDef* USARTx, uint32_t BaudRate, uart_RxFunc RxFunc);
// Функция отправки данных
int8_t UART_Send(uint8_t *pBuff, uint16_t Len);
// Функция возвращает кол-во байт в Rx буфере
uint16_t USART_RxDataToProc(void);
// Функция получения очередного байта из Rx циклического буфера
uint8_t USART_GetByteToProc(void);


#endif