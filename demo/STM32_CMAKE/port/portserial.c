#include "mb.h"
#include "mbport.h"
#include "port_internal.h"

UART_HandleTypeDef          uart_mb; 


/*  Note that UART configuration for ST processors is, well, different:

    https://community.st.com/t5/stm32-mcus-products/uart-parity-and-data-bit-issue-in-stm32c0-series/td-p/713896

    Rest-of-World:  8E1

    ST:
    UartHandle.Init.WordLength = UART_WORDLENGTH_9B; // 8+Parity
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_EVEN;


    Rest-of-World: 7E1

    ST:
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B; // 7+Parity
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_EVEN;
*/


BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    UNUSED( ucPORT );

    // Configure UART for Modbus communication
    uart_mb.Instance          = MB_USART;
    uart_mb.Init.BaudRate     = ulBaudRate;
    uart_mb.Init.StopBits     = UART_STOPBITS_1; // Always use 1 stop bit
    uart_mb.Init.Mode         = UART_MODE_TX_RX;
    uart_mb.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    uart_mb.Init.OverSampling = UART_OVERSAMPLING_16;

    // Configure WordLength and Parity based on data bits and parity
    if( ucDataBits == 8 )
    {
        if( eParity == MB_PAR_NONE )
        {
            uart_mb.Init.WordLength = UART_WORDLENGTH_8B;
            uart_mb.Init.Parity     = UART_PARITY_NONE;
        }
        else
        {
            uart_mb.Init.WordLength = UART_WORDLENGTH_9B; // 8 data bits + parity
            uart_mb.Init.Parity     = (eParity == MB_PAR_ODD) ? UART_PARITY_ODD : UART_PARITY_EVEN;
        }
    }
    else if( ucDataBits == 7 )
    {
        if( eParity == MB_PAR_NONE )
        {
            uart_mb.Init.WordLength = UART_WORDLENGTH_7B;
            uart_mb.Init.Parity     = UART_PARITY_NONE;
        }
        else
        {
            uart_mb.Init.WordLength = UART_WORDLENGTH_8B; // 7 data bits + parity
            uart_mb.Init.Parity     = (eParity == MB_PAR_ODD) ? UART_PARITY_ODD : UART_PARITY_EVEN;
        }
    }
    else
    {
        return FALSE; // Unsupported data bits configuration
    }

    if( HAL_UART_Init( &uart_mb ) != HAL_OK )
    {
        return FALSE; // UART initialization failed
    }

    // Disable RX and TX interrupts initially
    __HAL_UART_DISABLE_IT(&uart_mb, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&uart_mb, UART_IT_TXE);

    return TRUE;
}

void MB_Uart_Init(void)
{
    // Enable UART peripheral clock
    MB_USART_CLK_ENABLE();

    // Enable GPIO clocks
    MB_TX_GPIO_CLK_ENABLE();
    MB_RX_GPIO_CLK_ENABLE();

    // Configure NVIC for UART interrupt
    HAL_NVIC_SetPriority(MB_USART_IRQn, MB_USART_IRQ_priority, MB_USART_IRQ_subpriority);
    HAL_NVIC_DisableIRQ(MB_USART_IRQn);
}

void vMBPortSerialEnable(BOOL rxEnable, BOOL txEnable)
{
    // Disable interrupts during configuration
    HAL_NVIC_DisableIRQ(MB_USART_IRQn);
    
    // Configure receive interrupt
    if( rxEnable )
        MB_USART->CR1 |= USART_CR1_RXNEIE;
    else
        MB_USART->CR1 &= ~USART_CR1_RXNEIE;

    // Configure transmit interrupt
    if( txEnable )
        MB_USART->CR1 |= USART_CR1_TXEIE;
    else
        MB_USART->CR1 &= ~USART_CR1_TXEIE;

    // Re-enable UART interrupt only if at least one direction is active
    if( rxEnable || txEnable )
        HAL_NVIC_EnableIRQ(MB_USART_IRQn);
}

BOOL xMBPortSerialPutByte(CHAR byte)
{
    MB_USART->TDR = byte;
    return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR *byte)
{
    *byte = MB_USART->RDR;
    return TRUE;
}

void MB_USART_IRQHandler(void)
{
    uint32_t isr = MB_USART->ISR;
    uint32_t cr1 = MB_USART->CR1;

    // Check for receive interrupt
    if( (isr & USART_ISR_RXNE) && (cr1 & USART_CR1_RXNEIE) )
    {
        vMBTimerDebugSetLow();
        pxMBFrameCBByteReceived();
    }

    // Check for transmit interrupt
    if( (isr & USART_ISR_TXE) && (cr1 & USART_CR1_TXEIE) )
    {
        pxMBFrameCBTransmitterEmpty();
    }
    
    // Clear error flags - use USART_ICR_NECF instead of USART_ICR_NCF
    MB_USART->ICR = (USART_ICR_PECF | USART_ICR_FECF | USART_ICR_NECF | 
                    USART_ICR_ORECF | USART_ICR_IDLECF);
                    
    // Do NOT call HAL_UART_IRQHandler here as it will interfere with our direct register access
}

/* ----------------------- End of file --------------------------------------*/

