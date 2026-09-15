#include "logger_uart.h"
#include "board.h"
#include <sys/_intsup.h>
#include "systemclock.h"


#define LOG_UART_CLOCK_FREQ_HZ    SYSTEMCLOCK_FREQ_HZ


static USART_TypeDef *LOG_UART_GetInstance(void)
{
    #if LOG_UART_INSTANCE == 1U
        return USART1;

    #elif LOG_UART_INSTANCE == 2U
        return USART2;

    #elif LOG_UART_INSTANCE == 3U
        return USART3;

    #elif LOG_UART_INSTANCE == 4U
        return UART4;

    #elif LOG_UART_INSTANCE == 5U
        return UART5;

    #elif LOG_UART_INSTANCE == 6U
        return LPUART1;

    #else
    #error "Unsupported LOG_UART_INSTANCE"
    #endif
}


static void LOG_UART_ClockEnable(void)
{
    #if LOG_UART_INSTANCE == 1U

        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    #elif LOG_UART_INSTANCE == 2U

        RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    #elif LOG_UART_INSTANCE == 3U

        RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;

    #elif LOG_UART_INSTANCE == 4U

        RCC->APB1ENR1 |= RCC_APB1ENR1_UART4EN;

    #elif LOG_UART_INSTANCE == 5U

        RCC->APB1ENR1 |= RCC_APB1ENR1_UART5EN;

    #elif LOG_UART_INSTANCE == 6U

        RCC->APB1ENR1 |= RCC_APB1ENR1_LPUART1EN;

    #else

    #error "Unsupported LOG_UART_INSTANCE"

    #endif
}


static void LOG_UART_Config(void)
{
    USART_TypeDef *uart = LOG_UART_GetInstance();

    uart->CR1 = 0U;
    uart->CR2 = 0U;
    uart->CR3 = 0U;

    #if LOG_UART_INSTANCE == 6U

        /* LPUART baud-rate configuration */
        uart->BRR = /* LPUART calculation */;

    #else

        uart->BRR = LOG_UART_CLOCK_FREQ_HZ / LOG_UART_BAUDRATE;

    #endif

        /* Parity */
    #if LOG_UART_PARITY == UART_PARITY_NONE

        /* No parity */

    #elif LOG_UART_PARITY == UART_PARITY_EVEN

        uart->CR1 |= USART_CR1_PCE;

    #elif LOG_UART_PARITY == UART_PARITY_ODD

        uart->CR1 |= USART_CR1_PCE | USART_CR1_PS;

    #else

    #error "Unsupported LOG_UART_PARITY"

    #endif

        /* Stop bits */
    #if LOG_UART_STOP_BITS == UART_STOPBITS_1

        /* 1 stop bit */

    #elif LOG_UART_STOP_BITS == UART_STOPBITS_2

        uart->CR2 |= USART_CR2_STOP_1;

    #else

    #error "Unsupported LOG_UART_STOP_BITS"

    #endif

        uart->CR1 |= USART_CR1_TE;
        uart->CR1 |= USART_CR1_UE;
}



static void LOG_UART_WriteChar(char ch)
{
    USART_TypeDef *uart = LOG_UART_GetInstance();

    /* Wait until transmit data register is empty */
    while ((uart->ISR & USART_ISR_TXE_TXFNF) == 0U)
    {
    }

    uart->TDR = (uint8_t)ch;
}

void LOG_UART_Init(void)
{
    // Initialize UART hardware here
    LOG_UART_ClockEnable();
    LOG_UART_Config();

}


void LOG_UART_Write(const char *buffer, size_t length)
{
    // Write the buffer to UART hardware here
    for (size_t i = 0U; i < length; i++)
    {
        LOG_UART_WriteChar(buffer[i]);
    }
}