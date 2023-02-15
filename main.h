//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef MAIN
#define MAIN


// Ножка светодиода на плате с МК
#define ZFM_LedState_Port               GPIOC
#define ZFM_LedState_Pin                GPIO_Pin_13
// Ножка светодиода на плате с МК
#define ZFM_LedRed_Port                 GPIOB
#define ZFM_LedRed_Pin                  GPIO_Pin_14
// Ножка светодиода на плате с МК
#define ZFM_LedGreen_Port               GPIOB
#define ZFM_LedGreen_Pin                GPIO_Pin_15
// Ножка светодиода на плате с МК
#define ZFM_LedBlue_Port                GPIOA
#define ZFM_LedBlue_Pin                 GPIO_Pin_8
// Ножка для перевода в режим обучения новым отпечаткам
#define ZFM_EnrollNewFinger_Port        GPIOA
#define ZFM_EnrollNewFinger_Pin         GPIO_Pin_0
// Ножка для очистки библиотеки шаблонов отпечатков
#define ZFM_ClearFingerLib_Port         GPIOA
#define ZFM_ClearFingerLib_Pin          GPIO_Pin_1


#endif