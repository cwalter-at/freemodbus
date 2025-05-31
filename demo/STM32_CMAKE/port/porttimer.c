#include "mb.h"
#include "mbport.h"
#include "port_internal.h"
#include "main.h"  // Include for GPIO definitions

static TIM_HandleTypeDef h_tim7;

/* Static variables */
static USHORT           timeout     = 0;
static USHORT           downcounter = 0;

BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    __HAL_RCC_TIM7_CLK_ENABLE();

    TIM_MasterConfigTypeDef sMasterConfig;
    
    h_tim7.Instance = TIM7;
    h_tim7.Init.Prescaler = (HAL_RCC_GetPCLK1Freq() / 1000000) - 1;
    h_tim7.Init.CounterMode = TIM_COUNTERMODE_UP;
    h_tim7.Init.Period = 50 - 1;

    timeout = usTim1Timerout50us;

    if (HAL_TIM_Base_Init(&h_tim7) != HAL_OK)
    {
    return FALSE;
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&h_tim7, &sMasterConfig) != HAL_OK)
    {
    return FALSE;
    }

    /* Configure NVIC for timer interrupt */
    HAL_NVIC_SetPriority(TIM7_IRQn, MB_TIM7_IRQ_priority, MB_TIM7_IRQ_subpriority);
    
    /* Setup debug output pin */
    #if MB_TIMER_DEBUG == 1
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Configure debug GPIO pin */
    GPIO_InitStruct.Pin = MB_TIMER_DEBUG_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MB_TIMER_DEBUG_PORT, &GPIO_InitStruct);
    
    /* Initial state: low */
    vMBTimerDebugSetLow();
    #endif

    return TRUE;
}

void vMBPortTimersEnable( void )
{
  /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
  downcounter = timeout;
  
  /* Set debug pin high to indicate timer is active */
  vMBTimerDebugSetHigh();
  
  HAL_TIM_Base_Start_IT(&h_tim7);
  HAL_NVIC_EnableIRQ(TIM7_IRQn);
}

void vMBPortTimersDisable( void )
{
  /* Disable any pending timers. */
  HAL_TIM_Base_Stop_IT(&h_tim7);
  HAL_NVIC_DisableIRQ(TIM7_IRQn);
  
  /* Set debug pin low to indicate timer is disabled */
  vMBTimerDebugSetLow();
}

/**
 * @brief This function handles TIM7 global interrupt.
 */
void TIM7_IRQHandler( void )
{
    /* Check if update interrupt flag is set */
    if(__HAL_TIM_GET_FLAG(&h_tim7, TIM_FLAG_UPDATE) != RESET && 
       __HAL_TIM_GET_IT_SOURCE(&h_tim7, TIM_IT_UPDATE) != RESET) {
        /* Clear update interrupt flag */
        __HAL_TIM_CLEAR_IT(&h_tim7, TIM_IT_UPDATE);

        /* Decrement down-counter and check if reached zero */
        if (--downcounter == 0) {
            /* Timer expired, call the callback function */
            vMBTimerDebugSetLow();
            pxMBPortCBTimerExpired();
        }
    }

    /* Call the HAL timer IRQ handler */
    HAL_TIM_IRQHandler(&h_tim7);
}

