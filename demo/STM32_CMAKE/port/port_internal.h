#ifndef _PORT_INTERNAL_H
#define _PORT_INTERNAL_H

#include "port.h"

/* Detect STM32 family and include appropriate HAL */
#if defined(STM32F3xx)
  #include "stm32f3xx_hal.h"
#elif defined(STM32G4xx) || defined(STM32G431xx)
  #include "stm32g4xx_hal.h"
#elif defined(STM32F1xx)
  #include "stm32f1xx_hal.h"
/* Add other STM32 families as needed */
#else
  #error "No STM32 device family defined!"
#endif

/* define default USART IRQ priority and subpriority if not defined */
#ifndef MB_USART_IRQ_priority
  #define MB_USART_IRQ_priority     3
#endif
#ifndef MB_USART_IRQ_subpriority
  #define MB_USART_IRQ_subpriority  1
#endif

/* define default TIM7 IRQ priority and subpriority if not defined */
#ifndef MB_TIM7_IRQ_priority
  #define MB_TIM7_IRQ_priority     4
#endif
#ifndef MB_TIM7_IRQ_subpriority
  #define MB_TIM7_IRQ_subpriority  1
#endif

/* Default to USART2 if MB_USART_NR is not defined */
#ifndef MB_USART_NR
  #define MB_USART_NR   2
#endif

/* Define macros based on the selected USART */
#if (MB_USART_NR == 1)

  #define MB_USART                        USART1
  #define MB_USART_IRQn                   USART1_IRQn
  #define MB_USART_IRQHandler             USART1_IRQHandler
  #define MB_USART_CLK_ENABLE()           __HAL_RCC_USART1_CLK_ENABLE()
  #define MB_USART_CLK_DISABLE()          __HAL_RCC_USART1_CLK_DISABLE()
  #define MB_TX_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
  #define MB_TX_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOA_CLK_DISABLE()
  #define MB_TX_AF                        GPIO_AF7_USART1
  #define MB_RX_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
  #define MB_RX_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOA_CLK_DISABLE()
  #define MB_RX_AF                        GPIO_AF7_USART1

#elif (MB_USART_NR == 2)

  #define MB_USART                        USART2
  #define MB_USART_IRQn                   USART2_IRQn
  #define MB_USART_IRQHandler             USART2_IRQHandler
  #define MB_USART_CLK_ENABLE()           __HAL_RCC_USART2_CLK_ENABLE()
  #define MB_USART_CLK_DISABLE()          __HAL_RCC_USART2_CLK_DISABLE()
  #define MB_TX_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
  #define MB_TX_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOA_CLK_DISABLE()
  #define MB_TX_AF                        GPIO_AF7_USART2
  #define MB_RX_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
  #define MB_RX_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOA_CLK_DISABLE()
  #define MB_RX_AF                        GPIO_AF7_USART2

#else
  #error "Unsupported MB_USART configuration. Please define MB_USART_NR as 1 or 2."
#endif

//#define MB_TIMER_DEBUG              1
#define MB_TIMER_DEBUG_PORT         GPIOA
#define MB_TIMER_DEBUG_PIN          GPIO_PIN_5  // PA5 is connected to the onboard LED on most Nucleo boards

/* Debug helper functions */
#if MB_TIMER_DEBUG == 1
static inline void vMBTimerDebugSetHigh( void )
{
    HAL_GPIO_WritePin( MB_TIMER_DEBUG_PORT, MB_TIMER_DEBUG_PIN, GPIO_PIN_SET );
}

static inline void vMBTimerDebugSetLow( void )
{
    HAL_GPIO_WritePin( MB_TIMER_DEBUG_PORT, MB_TIMER_DEBUG_PIN, GPIO_PIN_RESET );
}
#else
#define vMBTimerDebugSetHigh()
#define vMBTimerDebugSetLow()
#endif


void                    MB_Uart_Init(void);

#endif // _PORT_INTERNAL_H
